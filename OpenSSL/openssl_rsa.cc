#include "openssl_utils.hpp"

#include <scopeguard.hpp>

#include <gtest/gtest.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>

#include <memory>

class OpensslRsa : noncopyable
{
public:
    OpensslRsa() = default;
    explicit OpensslRsa(int bits) { generate(bits); }
    ~OpensslRsa() { destroy(); }

    void generate(int bits)
    {
        destroy();
        m_pkey = EVP_RSA_gen(bits);
    }

    auto encrypt(int padding, const std::string &plain) -> std::string
    {
        assert(m_pkey != nullptr);

        auto *ctx = EVP_PKEY_CTX_new(m_pkey, nullptr);
        auto cleanup = scopeGuard([&] {
            if (ctx != nullptr) {
                EVP_PKEY_CTX_free(ctx);
            }
        });

        if (ctx == nullptr) {
            openssl_error();
            return {};
        }
        if (EVP_PKEY_encrypt_init(ctx) <= 0) {
            openssl_error();
            return {};
        }

        if (EVP_PKEY_CTX_set_rsa_padding(ctx, padding) <= 0) {
            openssl_error();
            return {};
        }

        std::string cipher;
        cipher.resize(EVP_PKEY_size(m_pkey));
        size_t outLen = 0;
        if (EVP_PKEY_encrypt(ctx,
                             reinterpret_cast<unsigned char *>(cipher.data()),
                             &outLen,
                             reinterpret_cast<const unsigned char *>(plain.c_str()),
                             static_cast<int>(plain.size()))
            <= 0) {
            openssl_error();
            return {};
        }
        cipher.resize(outLen);
        return cipher;
    }

    auto decrypt(int padding, const std::string &cipher) -> std::string
    {
        assert(m_pkey != nullptr);

        auto *ctx = EVP_PKEY_CTX_new(m_pkey, nullptr);
        auto cleanup = scopeGuard([&] {
            if (ctx != nullptr) {
                EVP_PKEY_CTX_free(ctx);
            }
        });

        if (ctx == nullptr) {
            openssl_error();
            return {};
        }
        if (EVP_PKEY_decrypt_init(ctx) <= 0) {
            openssl_error();
            return {};
        }
        if (EVP_PKEY_CTX_set_rsa_padding(ctx, padding) <= 0) {
            openssl_error();
            return {};
        }

        std::string plain;
        plain.resize(EVP_PKEY_size(m_pkey));
        size_t len = plain.size();
        if (EVP_PKEY_decrypt(ctx,
                             reinterpret_cast<unsigned char *>(plain.data()),
                             &len,
                             reinterpret_cast<const unsigned char *>(cipher.c_str()),
                             static_cast<int>(cipher.size()))
            <= 0) {
            openssl_error();
            return {};
        }
        plain.resize(len);
        return plain;
    }

    [[nodiscard]] auto toPublicPem() const -> std::string
    {
        BIO *bio = BIO_new(BIO_s_mem());
        PEM_write_bio_PUBKEY(bio, m_pkey);
        char *data;
        int len = BIO_get_mem_data(bio, &data);
        std::string ret(data, len);
        BIO_free(bio);
        return ret;
    }

    [[nodiscard]] auto toPrivatePem() const -> std::string
    {
        BIO *bio = BIO_new(BIO_s_mem());
        PEM_write_bio_PrivateKey(bio, m_pkey, nullptr, nullptr, 0, nullptr, nullptr);
        char *data;
        int len = BIO_get_mem_data(bio, &data);
        std::string ret(data, len);
        BIO_free(bio);
        return ret;
    }

private:
    void destroy()
    {
        if (m_pkey != nullptr) {
            EVP_PKEY_free(m_pkey);
            m_pkey = nullptr;
        }
    }

    static auto fromPublicPem(const std::string &pem) -> OpensslRsa *
    {
        std::unique_ptr<OpensslRsa> retPtr(new OpensslRsa);
        BIO *bio = BIO_new_mem_buf(pem.c_str(), -1);
        if (bio == nullptr) {
            return nullptr;
        }
        retPtr->m_pkey = PEM_read_bio_PUBKEY(bio, nullptr, nullptr, nullptr);
        BIO_free(bio);
        return retPtr.release();
    }

    static auto fromPrivatePem(const std::string &pem) -> OpensslRsa *
    {
        std::unique_ptr<OpensslRsa> retPtr(new OpensslRsa);
        BIO *bio = BIO_new_mem_buf(pem.c_str(), -1);
        if (bio == nullptr) {
            return nullptr;
        }
        retPtr->m_pkey = PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr);
        BIO_free(bio);
        return retPtr.release();
    }

    friend class OpensslRsaPublicKey;
    friend class OpensslRsaPrivateKey;

    EVP_PKEY *m_pkey = nullptr;
};

class OpensslRsaPublicKey : noncopyable
{
public:
    explicit OpensslRsaPublicKey(const std::string &pem) { setPublicPem(pem); }
    ~OpensslRsaPublicKey() = default;

    void setPublicPem(const std::string &pem)
    {
        m_opensslRsaPtr.reset(OpensslRsa::fromPublicPem(pem));
    }

    [[nodiscard]] auto toPem() const -> std::string
    {
        assert(m_opensslRsaPtr != nullptr);
        return m_opensslRsaPtr->toPublicPem();
    }

    [[nodiscard]] auto encrypt(int padding, const std::string &plain) const -> std::string
    {
        assert(m_opensslRsaPtr != nullptr);
        return m_opensslRsaPtr->encrypt(padding, plain);
    }

private:
    std::unique_ptr<OpensslRsa> m_opensslRsaPtr;
};

class OpensslRsaPrivateKey : noncopyable
{
public:
    explicit OpensslRsaPrivateKey(const std::string &pem) { setPrivatePem(pem); }
    ~OpensslRsaPrivateKey() = default;

    void setPrivatePem(const std::string &pem)
    {
        m_opensslRsaPtr.reset(OpensslRsa::fromPrivatePem(pem));
    }

    [[nodiscard]] auto toPem() const -> std::string
    {
        assert(m_opensslRsaPtr != nullptr);
        return m_opensslRsaPtr->toPrivatePem();
    }

    [[nodiscard]] auto decrypt(int padding, const std::string &cipher) const -> std::string
    {
        assert(m_opensslRsaPtr != nullptr);
        return m_opensslRsaPtr->decrypt(padding, cipher);
    }

private:
    std::unique_ptr<OpensslRsa> m_opensslRsaPtr;
};

TEST(OpensslRsa, test)
{
    OpensslRsa opensslRsa(2048);
    std::cout << "Rsa public key: \n" << opensslRsa.toPublicPem() << '\n';
    std::cout << "Rsa private key: \n" << opensslRsa.toPrivatePem() << '\n';

    const std::string plain = "hello world";
    auto cipher = opensslRsa.encrypt(RSA_PKCS1_PADDING, plain);
    auto plain2 = opensslRsa.decrypt(RSA_PKCS1_PADDING, cipher);

    EXPECT_EQ(plain, plain2);
}

TEST(OpensslRsa, test2)
{
    OpensslRsa opensslRsa(2048);
    OpensslRsaPublicKey opensslRsaPublicKey(opensslRsa.toPublicPem());
    OpensslRsaPrivateKey opensslRsaPrivateKey(opensslRsa.toPrivatePem());
    std::cout << "Rsa public key: \n" << opensslRsaPublicKey.toPem() << '\n';
    std::cout << "Rsa private key: \n" << opensslRsaPrivateKey.toPem() << '\n';

    const std::string plain = "hello world";
    auto cipher = opensslRsaPublicKey.encrypt(RSA_PKCS1_PADDING, plain);
    auto plain2 = opensslRsaPrivateKey.decrypt(RSA_PKCS1_PADDING, cipher);

    EXPECT_EQ(plain, plain2);
}

auto main() -> int
{
    openssl_version();

    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}

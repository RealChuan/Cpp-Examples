#include "openssl_utils.hpp"

#include <openssl/pem.h>
#include <openssl/rsa.h>

#include <gtest/gtest.h>

// RSA generate key
void rsa_generate_key(std::string &pub_key, std::string &pri_key)
{
    RSA *rsa = RSA_new();
    if (rsa == nullptr) {
        openssl_error();
        return;
    }

    BIGNUM *e = BN_new();
    if (e == nullptr) {
        openssl_error();
        RSA_free(rsa);
        return;
    }

    if (BN_set_word(e, RSA_F4) != 1) {
        openssl_error();
        BN_free(e);
        RSA_free(rsa);
        return;
    }

    if (RSA_generate_key_ex(rsa, 2048, e, nullptr) != 1) { // RSA-2048
        openssl_error();
        BN_free(e);
        RSA_free(rsa);
        return;
    }

    BIO *bio = BIO_new(BIO_s_mem());
    if (bio == nullptr) {
        openssl_error();
        BN_free(e);
        RSA_free(rsa);
        return;
    }

    if (PEM_write_bio_RSAPublicKey(bio, rsa) != 1) {
        openssl_error();
        BIO_free(bio);
        BN_free(e);
        RSA_free(rsa);
        return;
    }

    int len = BIO_pending(bio);
    pub_key.resize(len);

    if (BIO_read(bio, &pub_key[0], len) != len) {
        openssl_error();
        BIO_free(bio);
        BN_free(e);
        RSA_free(rsa);
        return;
    }

    BIO_free(bio);

    bio = BIO_new(BIO_s_mem());
    if (bio == nullptr) {
        openssl_error();
        BN_free(e);
        RSA_free(rsa);
        return;
    }

    if (PEM_write_bio_RSAPrivateKey(bio, rsa, nullptr, nullptr, 0, nullptr, nullptr) != 1) {
        openssl_error();
        BIO_free(bio);
        BN_free(e);
        RSA_free(rsa);
        return;
    }

    len = BIO_pending(bio);
    pri_key.resize(len);

    if (BIO_read(bio, &pri_key[0], len) != len) {
        openssl_error();
        BIO_free(bio);
        BN_free(e);
        RSA_free(rsa);
        return;
    }

    BIO_free(bio);
    BN_free(e);
    RSA_free(rsa);
}

// RSA encrypt
auto rsa_encrypt(const std::string &pub_key, const std::string &plain) -> std::string
{
    std::string cipher;

    BIO *bio = BIO_new(BIO_s_mem());
    if (bio == nullptr) {
        openssl_error();
        return cipher;
    }

    if (BIO_write(bio, pub_key.c_str(), static_cast<int>(pub_key.size()))
        != static_cast<int>(pub_key.size())) {
        openssl_error();
        BIO_free(bio);
        return cipher;
    }

    RSA *rsa = PEM_read_bio_RSAPublicKey(bio, nullptr, nullptr, nullptr);
    if (rsa == nullptr) {
        openssl_error();
        BIO_free(bio);
        return cipher;
    }

    int rsa_len = RSA_size(rsa);
    cipher.resize(rsa_len);

    int len = RSA_public_encrypt(static_cast<int>(plain.size()),
                                 reinterpret_cast<const unsigned char *>(plain.c_str()),
                                 reinterpret_cast<unsigned char *>(&cipher[0]),
                                 rsa,
                                 RSA_PKCS1_PADDING);
    if (len == -1) {
        openssl_error();
        BIO_free(bio);
        RSA_free(rsa);
        return cipher;
    }

    cipher.resize(len);

    BIO_free(bio);
    RSA_free(rsa);

    return cipher;
}

// RSA decrypt
auto rsa_decrypt(const std::string &pri_key, const std::string &cipher) -> std::string
{
    std::string plain;

    BIO *bio = BIO_new(BIO_s_mem());
    if (bio == nullptr) {
        openssl_error();
        return plain;
    }

    if (BIO_write(bio, pri_key.c_str(), static_cast<int>(pri_key.size()))
        != static_cast<int>(pri_key.size())) {
        openssl_error();
        BIO_free(bio);
        return plain;
    }

    RSA *rsa = PEM_read_bio_RSAPrivateKey(bio, nullptr, nullptr, nullptr);
    if (rsa == nullptr) {
        openssl_error();
        BIO_free(bio);
        return plain;
    }

    int rsa_len = RSA_size(rsa);
    plain.resize(rsa_len);

    int len = RSA_private_decrypt(static_cast<int>(cipher.size()),
                                  reinterpret_cast<const unsigned char *>(cipher.c_str()),
                                  reinterpret_cast<unsigned char *>(&plain[0]),
                                  rsa,
                                  RSA_PKCS1_PADDING);
    if (len == -1) {
        openssl_error();
        BIO_free(bio);
        RSA_free(rsa);
        return plain;
    }

    plain.resize(len);

    BIO_free(bio);
    RSA_free(rsa);

    return plain;
}

TEST(openssl_rsa, rsa_encrypt_decrypt)
{
    auto plain = "hello world";
    std::string pub_key, pri_key;
    rsa_generate_key(pub_key, pri_key);
    std::cout << "rsa pub key: " << pub_key << std::endl;
    std::cout << "rsa pri key: " << pri_key << std::endl;
    auto cipher = rsa_encrypt(pub_key, plain);
    std::cout << "rsa cipher: " << toHex(cipher) << std::endl;
    auto plain2 = rsa_decrypt(pri_key, cipher);
    std::cout << "rsa plain: " << plain2 << std::endl;

    EXPECT_EQ(plain, plain2);
}

auto main() -> int
{
    openssl_version();

    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}

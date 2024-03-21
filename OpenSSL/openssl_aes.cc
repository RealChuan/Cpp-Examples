#include "openssl_utils.hpp"

#include <gtest/gtest.h>
#include <openssl/evp.h>

#include <random>

// AES generate key and iv
void aes_generate_key_iv(std::string &key, std::string &iv)
{
    key.resize(32);
    iv.resize(16);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    for (auto &c : key) {
        c = static_cast<char>(dis(gen));
    }

    for (auto &c : iv) {
        c = static_cast<char>(dis(gen));
    }
}

// AES encrypt
auto aes_encrypt(const std::string &key, const std::string &iv, const std::string &plain)
    -> std::string
{
    std::string cipher;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (ctx == nullptr) {
        openssl_error();
        return cipher;
    }

    if (EVP_EncryptInit_ex(ctx,
                           EVP_aes_256_cbc(), // AES-256-CBC
                           nullptr,
                           reinterpret_cast<const unsigned char *>(key.c_str()),
                           reinterpret_cast<const unsigned char *>(iv.c_str()))
        != 1) {
        openssl_error();
        EVP_CIPHER_CTX_free(ctx);
        return cipher;
    }

    int len = 0;
    int plain_len = static_cast<int>(plain.size());
    int max_cipher_len = plain_len + EVP_CIPHER_CTX_block_size(ctx);
    cipher.resize(max_cipher_len);

    if (EVP_EncryptUpdate(ctx,
                          reinterpret_cast<unsigned char *>(cipher.data()),
                          &len,
                          reinterpret_cast<const unsigned char *>(plain.c_str()),
                          plain_len)
        != 1) {
        openssl_error();
        EVP_CIPHER_CTX_free(ctx);
        return cipher;
    }

    int cipher_len = len;

    if (EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char *>(cipher.data()) + len, &len)
        != 1) {
        openssl_error();
        EVP_CIPHER_CTX_free(ctx);
        return cipher;
    }

    cipher_len += len;
    cipher.resize(cipher_len);

    EVP_CIPHER_CTX_free(ctx);

    return cipher;
}

// AES decrypt
auto aes_decrypt(const std::string &key, const std::string &iv, const std::string &cipher)
    -> std::string
{
    std::string plain;

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (ctx == nullptr) {
        openssl_error();
        return plain;
    }

    if (EVP_DecryptInit_ex(ctx,
                           EVP_aes_256_cbc(), // AES-256-CBC
                           nullptr,
                           reinterpret_cast<const unsigned char *>(key.c_str()),
                           reinterpret_cast<const unsigned char *>(iv.c_str()))
        != 1) {
        openssl_error();
        EVP_CIPHER_CTX_free(ctx);
        return plain;
    }

    int len = 0;
    int cipher_len = static_cast<int>(cipher.size());
    int max_plain_len = cipher_len + EVP_CIPHER_CTX_block_size(ctx);
    plain.resize(max_plain_len);

    if (EVP_DecryptUpdate(ctx,
                          reinterpret_cast<unsigned char *>(plain.data()),
                          &len,
                          reinterpret_cast<const unsigned char *>(cipher.c_str()),
                          cipher_len)
        != 1) {
        openssl_error();
        EVP_CIPHER_CTX_free(ctx);
        return plain;
    }

    int plain_len = len;

    if (EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char *>(plain.data()) + len, &len) != 1) {
        openssl_error();
        EVP_CIPHER_CTX_free(ctx);
        return plain;
    }

    plain_len += len;
    plain.resize(plain_len);

    EVP_CIPHER_CTX_free(ctx);

    return plain;
}

TEST(openssl_aes, openssl_aes)
{
    std::string plain = "hello world";

    std::string key;
    std::string iv;
    aes_generate_key_iv(key, iv);

    std::string cipher = aes_encrypt(key, iv, plain);
    std::string result = aes_decrypt(key, iv, cipher);

    EXPECT_EQ(plain, result);
}

auto main() -> int
{
    openssl_version();

    testing::InitGoogleTest();
    return RUN_ALL_TESTS();
}

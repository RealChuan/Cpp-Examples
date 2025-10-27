#pragma once

#include <openssl/aes.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/rsa.h>

#include <string>
#include <vector>

std::string toHex(const std::string &str);
std::string fromHex(const std::string &str);
void handleOpenSSLError();

struct AesKey
{
    std::string key; // 256位AES密钥
    std::string iv;  // 128位初始化向量
};

struct RsaKeyPair
{
    std::string publicKey;
    std::string privateKey;
};

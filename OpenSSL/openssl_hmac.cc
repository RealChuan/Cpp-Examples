#include "openssl_utils.hpp"

#include <openssl/hmac.h>

#include <cstring>
#include <random>

// HMAC generate key
auto hmac_generate_key() -> std::string
{
    std::string key;

    key.resize(32);

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);

    for (auto &c : key) {
        c = static_cast<char>(dis(gen));
    }

    return key;
}

// HMAC
auto hmac(const std::string &key, const std::string &plain) -> std::string
{
    std::string hmac;

    unsigned int len = 0;
    unsigned char md[EVP_MAX_MD_SIZE];

    HMAC(EVP_sha256(),
         key.c_str(),
         static_cast<int>(key.size()),
         reinterpret_cast<const unsigned char *>(plain.c_str()),
         plain.size(),
         md,
         &len); // SHA-256

    hmac.resize(len);
    memcpy(&hmac[0], md, len);

    return hmac;
}

auto main() -> int
{
    openssl_version();

    // HMAC
    auto key = hmac_generate_key();
    std::cout << "hmac key: " << toHex(key) << std::endl;
    auto hmac_ = hmac(key, "hello world");
    std::cout << "hmac: " << toHex(hmac_) << std::endl;
    std::cout << std::endl;

    return 0;
}

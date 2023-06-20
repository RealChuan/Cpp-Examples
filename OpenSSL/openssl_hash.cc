#include "openssl_utils.hpp"

#include <openssl/pem.h>

// hash
auto hash(const std::string &plain) -> std::string
{
    std::string hash;

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (ctx == nullptr) {
        openssl_error();
        return hash;
    }

    if (EVP_DigestInit_ex(ctx, EVP_sha256(), nullptr) != 1) { // SHA-256
        openssl_error();
        EVP_MD_CTX_free(ctx);
        return hash;
    }

    if (EVP_DigestUpdate(ctx, plain.c_str(), plain.size()) != 1) {
        openssl_error();
        EVP_MD_CTX_free(ctx);
        return hash;
    }

    unsigned char md[EVP_MAX_MD_SIZE];
    unsigned int md_len = 0;

    if (EVP_DigestFinal_ex(ctx, md, &md_len) != 1) {
        openssl_error();
        EVP_MD_CTX_free(ctx);
        return hash;
    }

    hash.resize(md_len);
    memcpy(&hash[0], md, md_len);

    EVP_MD_CTX_free(ctx);

    return hash;
}

auto main() -> int
{
    openssl_version();

    // hash
    std::cout << "hash: " << toHex(hash("hello world")) << std::endl;
    std::cout << std::endl;

    return 0;
}
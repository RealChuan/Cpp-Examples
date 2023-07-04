#include "openssl_utils.hpp"

#include <openssl/pem.h>

// hash
auto hash(const std::string &plain, const EVP_MD *type) -> std::string
{
    std::string hash;

    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (ctx == nullptr) {
        openssl_error();
        return hash;
    }

    if (EVP_DigestInit_ex(ctx, type, nullptr) != 1) {
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

    std::string plain = "hello world";
    std::cout << "MD5: " << toHex(hash(plain, EVP_md5())) << std::endl;
    std::cout << "SHA1: " << toHex(hash(plain, EVP_sha1())) << std::endl;
    std::cout << "SHA256: " << toHex(hash(plain, EVP_sha256())) << std::endl;
    std::cout << "SHA512: " << toHex(hash(plain, EVP_sha512())) << std::endl;
    std::cout << "SHA3-256: " << toHex(hash(plain, EVP_sha3_256())) << std::endl;
    std::cout << "SHA3-512: " << toHex(hash(plain, EVP_sha3_512())) << std::endl;
    std::cout << "BLAKE2s-256: " << toHex(hash(plain, EVP_blake2s256())) << std::endl;
    std::cout << "BLAKE2b-512: " << toHex(hash(plain, EVP_blake2b512())) << std::endl;
    std::cout << "RIPEMD160: " << toHex(hash(plain, EVP_ripemd160())) << std::endl;
    std::cout << "SM3: " << toHex(hash(plain, EVP_sm3())) << std::endl;
    std::cout << "MD4: " << toHex(hash(plain, EVP_md4())) << std::endl;
    std::cout << "MD5-SHA1: " << toHex(hash(plain, EVP_md5_sha1())) << std::endl;
    std::cout << "SHA3-224: " << toHex(hash(plain, EVP_sha3_224())) << std::endl;
    std::cout << "SHA3-384: " << toHex(hash(plain, EVP_sha3_384())) << std::endl;

    return 0;
}
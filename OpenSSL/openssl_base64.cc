#include "openssl_utils.hpp"

#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/evp.h>

// base64 encode
auto base64_encode(const std::string &plain) -> std::string
{
    std::string base64;

    BIO *bio = BIO_new(BIO_f_base64());
    if (bio == nullptr) {
        openssl_error();
        return base64;
    }

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);

    BIO *bmem = BIO_new(BIO_s_mem());
    if (bmem == nullptr) {
        openssl_error();
        BIO_free(bio);
        return base64;
    }

    bio = BIO_push(bio, bmem);

    if (BIO_write(bio, plain.c_str(), static_cast<int>(plain.size()))
        != static_cast<int>(plain.size())) {
        openssl_error();
        BIO_free_all(bio);
        return base64;
    }

    if (BIO_flush(bio) != 1) {
        openssl_error();
        BIO_free_all(bio);
        return base64;
    }

    BUF_MEM *bptr = nullptr;
    BIO_get_mem_ptr(bio, &bptr);

    base64.assign(bptr->data, bptr->length);

    BIO_free_all(bio);

    return base64;
}

// base64 decode
auto base64_decode(const std::string &base64) -> std::string
{
    std::string plain;

    BIO *bio = BIO_new(BIO_f_base64());
    if (bio == nullptr) {
        openssl_error();
        return plain;
    }

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);

    BIO *bmem = BIO_new_mem_buf(base64.c_str(), static_cast<int>(base64.size()));
    if (bmem == nullptr) {
        openssl_error();
        BIO_free(bio);
        return plain;
    }

    bio = BIO_push(bio, bmem);

    plain.resize(base64.size());

    int len = BIO_read(bio, &plain[0], static_cast<int>(plain.size()));
    if (len < 0) {
        openssl_error();
        BIO_free_all(bio);
        return plain;
    }

    plain.resize(len);

    BIO_free_all(bio);

    return plain;
}

auto main() -> int
{
    openssl_version();

    // base64 encode and decode
    auto base64 = base64_encode("hello world");
    std::cout << "base64: " << base64 << std::endl;
    std::cout << "base64 decode: " << base64_decode(base64) << std::endl;
    std::cout << std::endl;

    return 0;
}
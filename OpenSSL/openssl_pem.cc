#include "openssl_utils.hpp"

#include <openssl/pem.h>

// PEM encode
auto pem_encode(const std::string &type, const std::string &data) -> std::string
{
    std::string pem;

    BIO *bio = BIO_new(BIO_s_mem());
    if (bio == nullptr) {
        openssl_error();
        return pem;
    }

    char *name = const_cast<char *>(type.c_str());

    if (!PEM_write_bio(bio,
                       name,
                       nullptr,
                       reinterpret_cast<const unsigned char *>(data.c_str()),
                       static_cast<int>(data.size()))) {
        openssl_error();
        BIO_free(bio);
        return pem;
    }

    int len = BIO_pending(bio);
    pem.resize(len);

    if (BIO_read(bio, &pem[0], len) != len) {
        openssl_error();
        BIO_free(bio);
        return pem;
    }

    BIO_free(bio);

    return pem;
}

// PEM decode
auto pem_decode(const std::string &type, const std::string &pem) -> std::string
{
    std::string data;

    BIO *bio = BIO_new(BIO_s_mem());
    if (bio == nullptr) {
        openssl_error();
        return data;
    }

    if (BIO_write(bio, pem.c_str(), static_cast<int>(pem.size())) != static_cast<int>(pem.size())) {
        openssl_error();
        BIO_free(bio);
        return data;
    }

    char *name = const_cast<char *>(type.c_str());
    char *header = nullptr;
    unsigned char *data_ = nullptr;
    long len = 0;
    if (!PEM_read_bio(bio, &name, &header, &data_, &len)) {
        openssl_error();
        BIO_free(bio);
        return data;
    }

    data.resize(len);
    memcpy(&data[0], data_, len);

    OPENSSL_free(header);
    OPENSSL_free(data_);

    BIO_free(bio);

    return data;
}

auto main() -> int
{
    openssl_version();

    // PEM encode and decode
    auto pem = pem_encode("RSA PRIVATE KEY", "hello world");
    std::cout << "pem: " << pem << std::endl;
    auto pem_data = pem_decode("RSA PRIVATE KEY", pem);
    std::cout << "pem data: " << pem_data << std::endl;

    return 0;
}

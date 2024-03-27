#include "openssl_utils.hpp"

#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#ifdef _WIN32
#include <openssl/applink.c>
#endif

// x509 生成证书
void x509_generate_certificate(const std::string &x509Path, const std::string &pkeyPath)
{
    // 生成私钥
    auto *pKey = EVP_RSA_gen(2048);

    // 生成证书
    X509 *x509 = X509_new();
    X509_set_version(x509, 2);
    ASN1_INTEGER_set(X509_get_serialNumber(x509), 1);
    X509_gmtime_adj(X509_get_notBefore(x509), 0);
    X509_gmtime_adj(X509_get_notAfter(x509), 31536000L);
    X509_set_pubkey(x509, pKey);

    // 设置证书信息
    X509_NAME *name = X509_get_subject_name(x509);
    X509_NAME_add_entry_by_txt(name,
                               "C",
                               MBSTRING_ASC,
                               reinterpret_cast<const unsigned char *>("CN"),
                               -1,
                               -1,
                               0);
    X509_NAME_add_entry_by_txt(name,
                               "O",
                               MBSTRING_ASC,
                               reinterpret_cast<const unsigned char *>("CN"),
                               -1,
                               -1,
                               0);
    X509_NAME_add_entry_by_txt(name,
                               "CN",
                               MBSTRING_ASC,
                               reinterpret_cast<const unsigned char *>("CN"),
                               -1,
                               -1,
                               0);
    X509_set_issuer_name(x509, name);

    // 签名证书
    X509_sign(x509, pKey, EVP_sha1());

    // 保存证书
    auto *out = BIO_new_file(x509Path.c_str(), "w");
    if (out != nullptr) {
        PEM_write_bio_X509(out, x509);
        BIO_free(out);
    }

    // 保存私钥
    out = BIO_new_file(pkeyPath.c_str(), "w");
    if (out != nullptr) {
        PEM_write_bio_PrivateKey(out, pKey, nullptr, nullptr, 0, nullptr, nullptr);
        BIO_free(out);
    }

    // 释放资源
    X509_free(x509);
    EVP_PKEY_free(pKey);
}

// x509 读取证书 并验证
void x509_read_certificate(const std::string &x509Path, const std::string &pkeyPath)
{
    // 读取证书
    auto *in = BIO_new_file(x509Path.c_str(), "r");
    if (in == nullptr) {
        std::cerr << "read certificate failed" << '\n';
        return;
    }
    X509 *x509 = PEM_read_bio_X509(in, nullptr, nullptr, nullptr);
    BIO_free(in);

    // 读取私钥
    in = BIO_new_file(pkeyPath.c_str(), "r");
    if (in == nullptr) {
        std::cerr << "read pkey failed" << '\n';
        return;
    }
    EVP_PKEY *pKey = PEM_read_bio_PrivateKey(in, nullptr, nullptr, nullptr);
    BIO_free(in);

    // 验证证书
    if (X509_verify(x509, pKey) == 1) {
        std::cout << "verify success" << '\n';
    } else {
        std::cout << "verify failed" << '\n';
    }

    // 释放资源
    X509_free(x509);
    EVP_PKEY_free(pKey);
}

auto main() -> int
{
    openssl_version();

    x509_generate_certificate("x509.pem", "pkey.pem");
    x509_read_certificate("x509.pem", "pkey.pem");

    return 0;
}

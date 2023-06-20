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
    EVP_PKEY *pkey = EVP_PKEY_new();
    EVP_PKEY_assign_RSA(pkey, RSA_generate_key(2048, RSA_F4, NULL, NULL));

    // 生成证书
    X509 *x509 = X509_new();
    X509_set_version(x509, 2);
    ASN1_INTEGER_set(X509_get_serialNumber(x509), 1);
    X509_gmtime_adj(X509_get_notBefore(x509), 0);
    X509_gmtime_adj(X509_get_notAfter(x509), 31536000L);
    X509_set_pubkey(x509, pkey);

    // 设置证书信息
    X509_NAME *name = X509_get_subject_name(x509);
    X509_NAME_add_entry_by_txt(name, "C", MBSTRING_ASC, (const unsigned char *) "CN", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "O", MBSTRING_ASC, (const unsigned char *) "CN", -1, -1, 0);
    X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC, (const unsigned char *) "CN", -1, -1, 0);
    X509_set_issuer_name(x509, name);

    // 签名证书
    X509_sign(x509, pkey, EVP_sha1());

    // 保存证书
    FILE *fp = fopen(x509Path.c_str(), "wb");
    PEM_write_X509(fp, x509);
    fclose(fp);

    // 保存私钥
    fp = fopen(pkeyPath.c_str(), "wb");
    PEM_write_PrivateKey(fp, pkey, NULL, NULL, 0, NULL, NULL);
    fclose(fp);

    // 释放资源
    X509_free(x509);
    EVP_PKEY_free(pkey);
}

// x509 读取证书 并验证
void x509_read_certificate(const std::string &x509Path, const std::string &pkeyPath)
{
    // 读取证书
    FILE *fp = fopen(x509Path.c_str(), "rb");
    X509 *x509 = PEM_read_X509(fp, NULL, NULL, NULL);
    fclose(fp);

    // 读取私钥
    fp = fopen(pkeyPath.c_str(), "rb");
    EVP_PKEY *pkey = PEM_read_PrivateKey(fp, NULL, NULL, NULL);
    fclose(fp);

    // 验证证书
    if (X509_verify(x509, pkey) == 1) {
        std::cout << "verify success" << std::endl;
    } else {
        std::cout << "verify failed" << std::endl;
    }

    // 释放资源
    X509_free(x509);
    EVP_PKEY_free(pkey);
}

auto main() -> int
{
    openssl_version();

    x509_generate_certificate("x509.pem", "pkey.pem");
    x509_read_certificate("x509.pem", "pkey.pem");

    return 0;
}

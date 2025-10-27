#include "openssl_common.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <stdexcept>

RsaKeyPair generateRsaKey()
{
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
    if (!ctx)
        handleOpenSSLError();

    if (EVP_PKEY_keygen_init(ctx) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        handleOpenSSLError();
    }

    if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 2048) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        handleOpenSSLError();
    }

    EVP_PKEY *pkey = nullptr;
    if (EVP_PKEY_keygen(ctx, &pkey) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        handleOpenSSLError();
    }

    EVP_PKEY_CTX_free(ctx);

    // 导出公钥
    BIO *pubBio = BIO_new(BIO_s_mem());
    if (PEM_write_bio_PUBKEY(pubBio, pkey) != 1) {
        BIO_free(pubBio);
        EVP_PKEY_free(pkey);
        handleOpenSSLError();
    }

    char *pubData = nullptr;
    long pubLen = BIO_get_mem_data(pubBio, &pubData);
    std::string publicKey(pubData, pubLen);
    BIO_free(pubBio);

    // 导出私钥
    BIO *privBio = BIO_new(BIO_s_mem());
    if (PEM_write_bio_PrivateKey(privBio, pkey, nullptr, nullptr, 0, nullptr, nullptr) != 1) {
        BIO_free(privBio);
        EVP_PKEY_free(pkey);
        handleOpenSSLError();
    }

    char *privData = nullptr;
    long privLen = BIO_get_mem_data(privBio, &privData);
    std::string privateKey(privData, privLen);
    BIO_free(privBio);

    EVP_PKEY_free(pkey);

    return RsaKeyPair{publicKey, privateKey};
}

std::string rsaEncrypt(const std::string &publicKeyPem, const std::string &plaintext)
{
    // 检查输入长度
    // 对于2048位RSA使用OAEP填充，最大加密长度是 2048/8 - 42 = 214字节
    const size_t maxRsaPlaintextLength = 214;
    if (plaintext.length() > maxRsaPlaintextLength) {
        throw std::invalid_argument(
            "Plaintext too long for RSA encryption: " + std::to_string(plaintext.length())
            + " bytes (max: " + std::to_string(maxRsaPlaintextLength) + " bytes)");
    }

    BIO *bio = BIO_new_mem_buf(publicKeyPem.data(), publicKeyPem.size());
    if (!bio)
        handleOpenSSLError();

    EVP_PKEY *pkey = PEM_read_bio_PUBKEY(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);

    if (!pkey)
        handleOpenSSLError();

    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pkey, nullptr);
    if (!ctx) {
        EVP_PKEY_free(pkey);
        handleOpenSSLError();
    }

    std::string encrypted;
    try {
        if (EVP_PKEY_encrypt_init(ctx) <= 0) {
            throw std::runtime_error("EVP_PKEY_encrypt_init failed");
        }

        if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING) <= 0) {
            throw std::runtime_error("EVP_PKEY_CTX_set_rsa_padding failed");
        }

        size_t outlen;
        if (EVP_PKEY_encrypt(ctx,
                             nullptr,
                             &outlen,
                             reinterpret_cast<const unsigned char *>(plaintext.data()),
                             plaintext.size())
            <= 0) {
            throw std::runtime_error("EVP_PKEY_encrypt (size) failed");
        }

        encrypted.resize(outlen);
        if (EVP_PKEY_encrypt(ctx,
                             reinterpret_cast<unsigned char *>(encrypted.data()),
                             &outlen,
                             reinterpret_cast<const unsigned char *>(plaintext.data()),
                             plaintext.size())
            <= 0) {
            throw std::runtime_error("EVP_PKEY_encrypt failed");
        }

        encrypted.resize(outlen);

        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
    } catch (...) {
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        throw;
    }

    return encrypted;
}

std::string rsaDecrypt(const std::string &privateKeyPem, const std::string &ciphertext)
{
    BIO *bio = BIO_new_mem_buf(privateKeyPem.data(), privateKeyPem.size());
    if (!bio)
        handleOpenSSLError();

    EVP_PKEY *pkey = PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr);
    BIO_free(bio);

    if (!pkey)
        handleOpenSSLError();

    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pkey, nullptr);
    if (!ctx) {
        EVP_PKEY_free(pkey);
        handleOpenSSLError();
    }

    std::string decrypted;
    try {
        if (EVP_PKEY_decrypt_init(ctx) <= 0) {
            throw std::runtime_error("EVP_PKEY_decrypt_init failed");
        }

        if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING) <= 0) {
            throw std::runtime_error("EVP_PKEY_CTX_set_rsa_padding failed");
        }

        size_t outlen;
        if (EVP_PKEY_decrypt(ctx,
                             nullptr,
                             &outlen,
                             reinterpret_cast<const unsigned char *>(ciphertext.data()),
                             ciphertext.size())
            <= 0) {
            throw std::runtime_error("EVP_PKEY_decrypt (size) failed");
        }

        decrypted.resize(outlen);
        if (EVP_PKEY_decrypt(ctx,
                             reinterpret_cast<unsigned char *>(decrypted.data()),
                             &outlen,
                             reinterpret_cast<const unsigned char *>(ciphertext.data()),
                             ciphertext.size())
            <= 0) {
            throw std::runtime_error("EVP_PKEY_decrypt failed");
        }

        decrypted.resize(outlen);

        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
    } catch (...) {
        EVP_PKEY_CTX_free(ctx);
        EVP_PKEY_free(pkey);
        throw;
    }

    return decrypted;
}

class RsaTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // 初始化OpenSSL
        OpenSSL_add_all_algorithms();
        ERR_load_crypto_strings();

        // 生成测试用的RSA密钥对
        rsaKeys = generateRsaKey();

        // 测试数据
        plainText = "Hello, RSA Testing World! This is a test message.";
        shortText = "Short";
        emptyText = "";
        specialChars = "Special!@#$%^&*()_+{}\":?><,./;'[]\\|-=~`";

        // 最大加密长度（对于2048位RSA，使用OAEP填充）
        // 2048/8 = 256字节 - 42字节OAEP填充 = 214字节
        maxEncryptLength = 214;
    }

    void TearDown() override
    {
        // 清理OpenSSL
        EVP_cleanup();
        ERR_free_strings();
    }

    RsaKeyPair rsaKeys;
    std::string plainText;
    std::string shortText;
    std::string emptyText;
    std::string specialChars;
    int maxEncryptLength; // 改为int类型
};

// 测试RSA密钥生成
TEST_F(RsaTest, KeyGeneration)
{
    RsaKeyPair keys = generateRsaKey();

    // 验证公钥和私钥不为空
    EXPECT_FALSE(keys.publicKey.empty());
    EXPECT_FALSE(keys.privateKey.empty());

    // 验证公钥包含RSA公钥标识
    EXPECT_TRUE(keys.publicKey.find("PUBLIC KEY") != std::string::npos);

    // 验证私钥包含RSA私钥标识
    EXPECT_TRUE(keys.privateKey.find("PRIVATE KEY") != std::string::npos);

    // 验证两次生成的密钥不同（随机性）
    RsaKeyPair keys2 = generateRsaKey();
    EXPECT_NE(keys.publicKey, keys2.publicKey);
    EXPECT_NE(keys.privateKey, keys2.privateKey);
}

// 测试RSA加密解密基本功能
TEST_F(RsaTest, EncryptDecryptBasic)
{
    std::string encrypted = rsaEncrypt(rsaKeys.publicKey, plainText);
    std::string decrypted = rsaDecrypt(rsaKeys.privateKey, encrypted);

    // 验证解密后文本与原始文本一致
    EXPECT_EQ(decrypted, plainText);

    // 验证加密后的文本不为空且与原文不同
    EXPECT_FALSE(encrypted.empty());
    EXPECT_NE(encrypted, plainText);
}

// 测试短文本加密解密
TEST_F(RsaTest, EncryptDecryptShortText)
{
    std::string encrypted = rsaEncrypt(rsaKeys.publicKey, shortText);
    std::string decrypted = rsaDecrypt(rsaKeys.privateKey, encrypted);

    EXPECT_EQ(decrypted, shortText);
}

// 测试空文本加密解密
TEST_F(RsaTest, EncryptDecryptEmptyText)
{
    std::string encrypted = rsaEncrypt(rsaKeys.publicKey, emptyText);
    std::string decrypted = rsaDecrypt(rsaKeys.privateKey, encrypted);

    EXPECT_EQ(decrypted, emptyText);
}

// 测试特殊字符加密解密
TEST_F(RsaTest, EncryptDecryptSpecialCharacters)
{
    std::string encrypted = rsaEncrypt(rsaKeys.publicKey, specialChars);
    std::string decrypted = rsaDecrypt(rsaKeys.privateKey, encrypted);

    EXPECT_EQ(decrypted, specialChars);
}

// 测试最大长度文本加密解密
TEST_F(RsaTest, EncryptDecryptMaxLength)
{
    // 创建接近最大长度的文本
    std::string maxLengthText(static_cast<size_t>(maxEncryptLength), 'A');

    std::string encrypted = rsaEncrypt(rsaKeys.publicKey, maxLengthText);
    std::string decrypted = rsaDecrypt(rsaKeys.privateKey, encrypted);

    EXPECT_EQ(decrypted, maxLengthText);
}

// 测试超长文本加密（应该失败）
TEST_F(RsaTest, EncryptTooLongText)
{
    // 创建超过最大长度的文本
    std::string tooLongText(static_cast<size_t>(maxEncryptLength + 10), 'A');

    // 现在应该明确抛出异常
    EXPECT_THROW({ rsaEncrypt(rsaKeys.publicKey, tooLongText); }, std::invalid_argument);
}

// 测试二进制数据加密解密
TEST_F(RsaTest, EncryptDecryptBinaryData)
{
    // 创建包含二进制数据的字符串（可能包含空字符等）
    std::vector<unsigned char> binaryData = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                             0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                                             0xFF, 0xFE, 0xFD, 0xFC, 0x00, 0x7F, 0x80, 0x81};
    std::string binaryText(binaryData.begin(), binaryData.end());

    std::string encrypted = rsaEncrypt(rsaKeys.publicKey, binaryText);
    std::string decrypted = rsaDecrypt(rsaKeys.privateKey, encrypted);

    EXPECT_EQ(decrypted, binaryText);
}

// 测试相同明文不同加密结果（由于RSA填充的随机性）
TEST_F(RsaTest, DifferentEncryptionResults)
{
    std::string encrypted1 = rsaEncrypt(rsaKeys.publicKey, plainText);
    std::string encrypted2 = rsaEncrypt(rsaKeys.publicKey, plainText);

    // 由于使用随机填充，两次加密结果应该不同
    EXPECT_NE(encrypted1, encrypted2);

    // 但解密后应该得到相同原文
    std::string decrypted1 = rsaDecrypt(rsaKeys.privateKey, encrypted1);
    std::string decrypted2 = rsaDecrypt(rsaKeys.privateKey, encrypted2);

    EXPECT_EQ(decrypted1, plainText);
    EXPECT_EQ(decrypted2, plainText);
    EXPECT_EQ(decrypted1, decrypted2);
}

// 测试错误公钥加密
TEST_F(RsaTest, EncryptWithWrongPublicKey)
{
    // 生成错误的密钥对
    RsaKeyPair wrongKeys = generateRsaKey();

    std::string encrypted = rsaEncrypt(wrongKeys.publicKey, plainText);

    // 使用正确私钥解密应该失败
    try {
        std::string decrypted = rsaDecrypt(rsaKeys.privateKey, encrypted);
        // 如果解密"成功"，结果应该与原文不同
        EXPECT_NE(decrypted, plainText);
    } catch (const std::exception &e) {
        // 抛出异常也是预期的行为
        SUCCEED();
    }
}

// 测试错误私钥解密
TEST_F(RsaTest, DecryptWithWrongPrivateKey)
{
    std::string encrypted = rsaEncrypt(rsaKeys.publicKey, plainText);

    // 生成错误的密钥对
    RsaKeyPair wrongKeys = generateRsaKey();

    // 使用错误私钥解密应该失败
    try {
        std::string decrypted = rsaDecrypt(wrongKeys.privateKey, encrypted);
        // 如果解密"成功"，结果应该与原文不同
        EXPECT_NE(decrypted, plainText);
    } catch (const std::exception &e) {
        // 抛出异常也是预期的行为
        SUCCEED();
    }
}

// 测试损坏的密文解密
TEST_F(RsaTest, DecryptCorruptedCiphertext)
{
    std::string encrypted = rsaEncrypt(rsaKeys.publicKey, plainText);

    // 损坏密文（修改一些字节）
    if (encrypted.length() > 20) {
        std::string corrupted = encrypted;
        corrupted[10] = ~corrupted[10]; // 翻转一个字节

        try {
            std::string decrypted = rsaDecrypt(rsaKeys.privateKey, corrupted);
            // 如果解密"成功"，结果应该与原文不同
            EXPECT_NE(decrypted, plainText);
        } catch (const std::exception &e) {
            // 抛出异常也是可以接受的行为
            SUCCEED();
        }
    }
}

// 测试无效公钥加密
TEST_F(RsaTest, EncryptWithInvalidPublicKey)
{
    std::string invalidPublicKey = "This is not a valid public key";

    EXPECT_THROW({ rsaEncrypt(invalidPublicKey, plainText); }, std::exception);
}

// 测试无效私钥解密
TEST_F(RsaTest, DecryptWithInvalidPrivateKey)
{
    std::string encrypted = rsaEncrypt(rsaKeys.publicKey, plainText);
    std::string invalidPrivateKey = "This is not a valid private key";

    EXPECT_THROW({ rsaDecrypt(invalidPrivateKey, encrypted); }, std::exception);
}

// 测试空密文解密
TEST_F(RsaTest, DecryptEmptyCiphertext)
{
    EXPECT_THROW({ rsaDecrypt(rsaKeys.privateKey, ""); }, std::exception);
}

// 测试十六进制转换与RSA配合使用
TEST_F(RsaTest, WithHexConversion)
{
    std::string encrypted = rsaEncrypt(rsaKeys.publicKey, plainText);

    // 转换为十六进制
    std::string hexEncrypted = toHex(encrypted);

    // 从十六进制转换回
    std::string fromHexEncrypted = fromHex(hexEncrypted);

    // 验证转换前后一致
    EXPECT_EQ(encrypted, fromHexEncrypted);

    // 解密应该仍然工作
    std::string decrypted = rsaDecrypt(rsaKeys.privateKey, fromHexEncrypted);
    EXPECT_EQ(decrypted, plainText);
}

// 测试多次加密解密一致性
TEST_F(RsaTest, MultipleEncryptDecryptConsistency)
{
    const int iterations = 10;

    for (int i = 0; i < iterations; ++i) {
        std::string testText = "Test iteration " + std::to_string(i);
        std::string encrypted = rsaEncrypt(rsaKeys.publicKey, testText);
        std::string decrypted = rsaDecrypt(rsaKeys.privateKey, encrypted);

        EXPECT_EQ(decrypted, testText);
    }
}

// 测试不同密钥对之间的隔离
TEST_F(RsaTest, KeyPairIsolation)
{
    // 生成第二对密钥
    RsaKeyPair keys2 = generateRsaKey();

    // 使用第一对密钥加密
    std::string encryptedWithKey1 = rsaEncrypt(rsaKeys.publicKey, plainText);

    // 使用第二对密钥解密应该失败
    try {
        std::string decryptedWithKey2 = rsaDecrypt(keys2.privateKey, encryptedWithKey1);
        EXPECT_NE(decryptedWithKey2, plainText);
    } catch (const std::exception &e) {
        SUCCEED();
    }

    // 使用第二对密钥加密
    std::string encryptedWithKey2 = rsaEncrypt(keys2.publicKey, plainText);

    // 使用第一对密钥解密应该失败
    try {
        std::string decryptedWithKey1 = rsaDecrypt(rsaKeys.privateKey, encryptedWithKey2);
        EXPECT_NE(decryptedWithKey1, plainText);
    } catch (const std::exception &e) {
        SUCCEED();
    }
}

// 性能测试：多次加密解密操作
TEST_F(RsaTest, PerformanceTest)
{
    const int iterations = 50; // RSA操作较慢，减少迭代次数
    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iterations; ++i) {
        std::string testText = "Performance test " + std::to_string(i);
        std::string encrypted = rsaEncrypt(rsaKeys.publicKey, testText);
        std::string decrypted = rsaDecrypt(rsaKeys.privateKey, encrypted);
        EXPECT_EQ(decrypted, testText);
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    // 输出性能信息
    std::cout << "RSA Performance: " << iterations << " iterations took " << duration.count()
              << " ms" << std::endl;

    // 可以根据需要设置性能阈值
    EXPECT_LE(duration.count(), 10000); // 10秒内完成50次操作
}

// 测试密钥序列化和反序列化
TEST_F(RsaTest, KeySerialization)
{
    // 使用生成的公钥重新加密
    std::string encrypted = rsaEncrypt(rsaKeys.publicKey, plainText);
    std::string decrypted = rsaDecrypt(rsaKeys.privateKey, encrypted);

    EXPECT_EQ(decrypted, plainText);

    // 验证公钥格式
    EXPECT_TRUE(rsaKeys.publicKey.find("-----BEGIN PUBLIC KEY-----") != std::string::npos);
    EXPECT_TRUE(rsaKeys.publicKey.find("-----END PUBLIC KEY-----") != std::string::npos);

    // 验证私钥格式
    EXPECT_TRUE(rsaKeys.privateKey.find("-----BEGIN PRIVATE KEY-----") != std::string::npos);
    EXPECT_TRUE(rsaKeys.privateKey.find("-----END PRIVATE KEY-----") != std::string::npos);
}

// 测试各种长度的文本加密
TEST_F(RsaTest, VariousLengthEncryption)
{
    std::vector<int> lengths = {1, 5, 10, 50, 100, 150, maxEncryptLength};

    for (int length : lengths) {
        if (length <= maxEncryptLength) {
            std::string testText(static_cast<size_t>(length), 'X');
            std::string encrypted = rsaEncrypt(rsaKeys.publicKey, testText);
            std::string decrypted = rsaDecrypt(rsaKeys.privateKey, encrypted);

            EXPECT_EQ(decrypted, testText) << "Failed for text length: " << length;
        }
    }
}

// 测试密钥重用
TEST_F(RsaTest, KeyReuse)
{
    // 多次使用同一对密钥
    std::vector<std::string> testMessages = {"Message 1",
                                             "Message 2",
                                             "Message 3",
                                             "Message 4",
                                             "Message 5"};

    for (const auto &message : testMessages) {
        std::string encrypted = rsaEncrypt(rsaKeys.publicKey, message);
        std::string decrypted = rsaDecrypt(rsaKeys.privateKey, encrypted);
        EXPECT_EQ(decrypted, message);
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
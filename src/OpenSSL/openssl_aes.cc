#include "openssl_common.hpp"

#include <gtest/gtest.h>

#include <stdexcept>

AesKey generateAesKey()
{
    unsigned char key[32]; // 256-bit key
    unsigned char iv[16];  // 128-bit IV

    if (RAND_bytes(key, sizeof(key)) != 1 || RAND_bytes(iv, sizeof(iv)) != 1) {
        handleOpenSSLError();
    }

    AesKey aesKey;
    aesKey.key = std::string(reinterpret_cast<char *>(key), sizeof(key));
    aesKey.iv = std::string(reinterpret_cast<char *>(iv), sizeof(iv));

    return aesKey;
}

std::string aesEncryptWithIV(const std::string &key,
                             const std::string &iv,
                             const std::string &plaintext)
{
    if (key.length() != 32) {
        throw std::invalid_argument("AES key must be 32 bytes (256-bit), got: "
                                    + std::to_string(key.length()));
    }

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
        handleOpenSSLError();

    // 使用提供的IV或生成随机IV
    unsigned char actual_iv[16];
    if (iv.empty()) {
        if (RAND_bytes(actual_iv, sizeof(actual_iv)) != 1) {
            EVP_CIPHER_CTX_free(ctx);
            handleOpenSSLError();
        }
    } else {
        if (iv.length() != 16) {
            EVP_CIPHER_CTX_free(ctx);
            throw std::invalid_argument("IV must be 16 bytes (128-bit)");
        }
        memcpy(actual_iv, iv.data(), 16);
    }

    std::string encrypted;
    try {
        if (EVP_EncryptInit_ex(ctx,
                               EVP_aes_256_cbc(),
                               nullptr,
                               reinterpret_cast<const unsigned char *>(key.data()),
                               actual_iv)
            != 1) {
            throw std::runtime_error("EVP_EncryptInit_ex failed");
        }

        int len;
        encrypted.resize(plaintext.size() + AES_BLOCK_SIZE);

        if (EVP_EncryptUpdate(ctx,
                              reinterpret_cast<unsigned char *>(encrypted.data()),
                              &len,
                              reinterpret_cast<const unsigned char *>(plaintext.data()),
                              plaintext.size())
            != 1) {
            throw std::runtime_error("EVP_EncryptUpdate failed");
        }
        int encrypted_len = len;

        if (EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char *>(encrypted.data()) + len, &len)
            != 1) {
            throw std::runtime_error("EVP_EncryptFinal_ex failed");
        }
        encrypted_len += len;

        encrypted.resize(encrypted_len);

        // 将IV添加到加密数据前面
        std::string result(reinterpret_cast<char *>(actual_iv), sizeof(actual_iv));
        result += encrypted;
        encrypted = result;

        EVP_CIPHER_CTX_free(ctx);
    } catch (...) {
        EVP_CIPHER_CTX_free(ctx);
        throw;
    }

    return encrypted;
}

std::string aesEncrypt(const std::string &key, const std::string &plaintext)
{
    return aesEncryptWithIV(key, "", plaintext); // 使用随机IV
}

std::string aesDecrypt(const std::string &key, const std::string &ciphertext)
{
    if (key.length() != 32) {
        throw std::invalid_argument("AES key must be 32 bytes (256-bit), got: "
                                    + std::to_string(key.length()));
    }
    if (ciphertext.length() <= 16) {
        throw std::invalid_argument("Ciphertext too short, must contain IV and encrypted data");
    }

    // 提取IV（前16字节）
    std::string iv_str = ciphertext.substr(0, 16);
    std::string actual_ciphertext = ciphertext.substr(16);

    EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
    if (!ctx)
        handleOpenSSLError();

    std::string decrypted;
    try {
        if (EVP_DecryptInit_ex(ctx,
                               EVP_aes_256_cbc(),
                               nullptr,
                               reinterpret_cast<const unsigned char *>(key.data()),
                               reinterpret_cast<const unsigned char *>(iv_str.data()))
            != 1) {
            throw std::runtime_error("EVP_DecryptInit_ex failed");
        }

        int len;
        decrypted.resize(actual_ciphertext.size() + AES_BLOCK_SIZE);

        if (EVP_DecryptUpdate(ctx,
                              reinterpret_cast<unsigned char *>(decrypted.data()),
                              &len,
                              reinterpret_cast<const unsigned char *>(actual_ciphertext.data()),
                              actual_ciphertext.size())
            != 1) {
            throw std::runtime_error("EVP_DecryptUpdate failed");
        }
        int decrypted_len = len;

        if (EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char *>(decrypted.data()) + len, &len)
            != 1) {
            throw std::runtime_error("EVP_DecryptFinal_ex failed");
        }
        decrypted_len += len;

        decrypted.resize(decrypted_len);

        EVP_CIPHER_CTX_free(ctx);
    } catch (...) {
        EVP_CIPHER_CTX_free(ctx);
        throw;
    }

    return decrypted;
}

class AesTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // 初始化OpenSSL
        OpenSSL_add_all_algorithms();
        ERR_load_crypto_strings();

        // 生成测试用的AES密钥
        aesKey = generateAesKey();
        testPlaintext = "Hello, AES Testing World! This is a test message.";
        shortText = "Short";
        emptyText = "";
    }

    void TearDown() override
    {
        // 清理OpenSSL
        EVP_cleanup();
        ERR_free_strings();
    }

    AesKey aesKey;
    std::string testPlaintext;
    std::string shortText;
    std::string emptyText;
};

// 测试AES密钥生成
TEST_F(AesTest, KeyGeneration)
{
    AesKey keys = generateAesKey();

    // 验证密钥长度应为32字节（256位）
    EXPECT_EQ(keys.key.length(), 32);
    // 验证IV长度应为16字节（128位）
    EXPECT_EQ(keys.iv.length(), 16);

    // 验证生成的密钥和IV不为空
    EXPECT_FALSE(keys.key.empty());
    EXPECT_FALSE(keys.iv.empty());

    // 验证两次生成的密钥不同（随机性）
    AesKey keys2 = generateAesKey();
    EXPECT_NE(keys.key, keys2.key);
    EXPECT_NE(keys.iv, keys2.iv);
}

// 测试AES加密解密基本功能
TEST_F(AesTest, EncryptDecryptBasic)
{
    std::string encrypted = aesEncrypt(aesKey.key, testPlaintext);
    std::string decrypted = aesDecrypt(aesKey.key, encrypted);

    // 验证解密后文本与原始文本一致
    EXPECT_EQ(decrypted, testPlaintext);

    // 验证加密后的文本不为空且与原文不同
    EXPECT_FALSE(encrypted.empty());
    EXPECT_NE(encrypted, testPlaintext);

    // 验证加密后长度至少包含IV（16字节）
    EXPECT_GE(encrypted.length(), 16);
}

// 测试短文本加密解密
TEST_F(AesTest, EncryptDecryptShortText)
{
    std::string encrypted = aesEncrypt(aesKey.key, shortText);
    std::string decrypted = aesDecrypt(aesKey.key, encrypted);

    EXPECT_EQ(decrypted, shortText);
}

// 测试空文本加密解密
TEST_F(AesTest, EncryptDecryptEmptyText)
{
    std::string encrypted = aesEncrypt(aesKey.key, emptyText);
    std::string decrypted = aesDecrypt(aesKey.key, encrypted);

    EXPECT_EQ(decrypted, emptyText);
}

// 测试长文本加密解密
TEST_F(AesTest, EncryptDecryptLongText)
{
    // 生成长文本
    std::string longText(10000, 'A');
    longText += "MiddlePart";
    longText += std::string(10000, 'Z');

    std::string encrypted = aesEncrypt(aesKey.key, longText);
    std::string decrypted = aesDecrypt(aesKey.key, encrypted);

    EXPECT_EQ(decrypted, longText);
}

// 测试特殊字符文本加密解密
TEST_F(AesTest, EncryptDecryptSpecialCharacters)
{
    std::string specialText = "Special!@#$%^&*()_+{}\":?><,./;'[]\\|-=~`";
    std::string encrypted = aesEncrypt(aesKey.key, specialText);
    std::string decrypted = aesDecrypt(aesKey.key, encrypted);

    EXPECT_EQ(decrypted, specialText);
}

// 测试二进制数据加密解密
TEST_F(AesTest, EncryptDecryptBinaryData)
{
    // 创建包含二进制数据的字符串（可能包含空字符等）
    std::vector<unsigned char> binaryData = {0x00, 0x01, 0x02, 0x03, 0xFF, 0xFE, 0xFD, 0x00};
    std::string binaryText(binaryData.begin(), binaryData.end());

    std::string encrypted = aesEncrypt(aesKey.key, binaryText);
    std::string decrypted = aesDecrypt(aesKey.key, encrypted);

    EXPECT_EQ(decrypted, binaryText);
}

// 测试相同明文不同加密结果（由于随机IV）
TEST_F(AesTest, DifferentEncryptionResults)
{
    std::string encrypted1 = aesEncrypt(aesKey.key, testPlaintext);
    std::string encrypted2 = aesEncrypt(aesKey.key, testPlaintext);

    // 由于使用随机IV，两次加密结果应该不同
    EXPECT_NE(encrypted1, encrypted2);

    // 但解密后应该得到相同原文
    std::string decrypted1 = aesDecrypt(aesKey.key, encrypted1);
    std::string decrypted2 = aesDecrypt(aesKey.key, encrypted2);

    EXPECT_EQ(decrypted1, testPlaintext);
    EXPECT_EQ(decrypted2, testPlaintext);
    EXPECT_EQ(decrypted1, decrypted2);
}

// 测试错误密钥解密
TEST_F(AesTest, DecryptWithWrongKey)
{
    std::string encrypted = aesEncrypt(aesKey.key, testPlaintext);

    // 生成错误的密钥
    AesKey wrongKeys = generateAesKey();

    // 使用错误密钥解密应该失败或得到错误结果
    try {
        std::string decrypted = aesDecrypt(wrongKeys.key, encrypted);
        // 如果解密"成功"，结果应该与原文不同
        EXPECT_NE(decrypted, testPlaintext);
    } catch (const std::exception &e) {
        // 抛出异常也是预期的行为
        SUCCEED();
    }
}

// 测试损坏的密文解密
TEST_F(AesTest, DecryptCorruptedCiphertext)
{
    std::string encrypted = aesEncrypt(aesKey.key, testPlaintext);

    // 损坏密文（修改一些字节）
    if (encrypted.length() > 20) {
        std::string corrupted = encrypted;
        corrupted[10] = ~corrupted[10]; // 翻转一个字节

        try {
            std::string decrypted = aesDecrypt(aesKey.key, corrupted);
            // 如果解密"成功"，结果应该与原文不同
            EXPECT_NE(decrypted, testPlaintext);
        } catch (const std::exception &e) {
            // 抛出异常也是可以接受的行为
            SUCCEED();
        }
    }
}

// 测试损坏的IV解密
TEST_F(AesTest, DecryptCorruptedIV)
{
    std::string encrypted = aesEncrypt(aesKey.key, testPlaintext);

    // 损坏IV部分（前16字节）
    if (encrypted.length() > 16) {
        std::string corrupted = encrypted;
        corrupted[5] = ~corrupted[5]; // 翻转IV中的一个字节

        try {
            std::string decrypted = aesDecrypt(aesKey.key, corrupted);
            // 如果解密"成功"，结果应该与原文不同
            EXPECT_NE(decrypted, testPlaintext);
        } catch (const std::exception &e) {
            // 抛出异常也是可以接受的行为
            SUCCEED();
        }
    }
}

// 测试密钥长度验证
TEST_F(AesTest, InvalidKeyLength)
{
    std::string shortKey(16, 'K'); // 16字节密钥（应该是32字节）
    std::string longKey(64, 'K');  // 64字节密钥

    // 测试过短密钥
    EXPECT_THROW({ aesEncrypt(shortKey, testPlaintext); }, std::invalid_argument);

    // 测试过长密钥
    EXPECT_THROW({ aesEncrypt(longKey, testPlaintext); }, std::invalid_argument);
}

// 测试空密文解密
TEST_F(AesTest, DecryptEmptyCiphertext)
{
    EXPECT_THROW({ aesDecrypt(aesKey.key, ""); }, std::invalid_argument);
}

// 测试十六进制转换配合AES
TEST_F(AesTest, WithHexConversion)
{
    std::string encrypted = aesEncrypt(aesKey.key, testPlaintext);

    // 转换为十六进制
    std::string hexEncrypted = toHex(encrypted);

    // 从十六进制转换回
    std::string fromHexEncrypted = fromHex(hexEncrypted);

    // 验证转换前后一致
    EXPECT_EQ(encrypted, fromHexEncrypted);

    // 解密应该仍然工作
    std::string decrypted = aesDecrypt(aesKey.key, fromHexEncrypted);
    EXPECT_EQ(decrypted, testPlaintext);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

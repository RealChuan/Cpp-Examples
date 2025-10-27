#include "openssl_common.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <stdexcept>

std::string base64Encode(const std::string &input)
{
    BIO *bmem = nullptr;
    BIO *b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

    bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);

    if (BIO_write(b64, input.data(), input.size()) != static_cast<int>(input.size())) {
        BIO_free_all(b64);
        handleOpenSSLError();
    }

    if (BIO_flush(b64) != 1) {
        BIO_free_all(b64);
        handleOpenSSLError();
    }

    char *outputData = nullptr;
    long outputLen = BIO_get_mem_data(bmem, &outputData);
    std::string output(outputData, outputLen);

    BIO_free_all(b64);
    return output;
}

std::string base64Decode(const std::string &input)
{
    BIO *b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

    BIO *bmem = BIO_new_mem_buf(input.data(), input.size());
    bmem = BIO_push(b64, bmem);

    std::string output;
    output.resize(input.size());

    int decodedLen = BIO_read(bmem, output.data(), output.size());
    if (decodedLen < 0) {
        BIO_free_all(bmem);
        handleOpenSSLError();
    }

    output.resize(decodedLen);
    BIO_free_all(bmem);

    return output;
}

class Base64Test : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // 初始化OpenSSL
        OpenSSL_add_all_algorithms();
        ERR_load_crypto_strings();

        // 测试数据
        plainText = "Hello, Base64 Testing World! This is a test message.";
        shortText = "Short";
        emptyText = "";
        specialChars = "Special!@#$%^&*()_+{}\":?><,./;'[]\\|-=~`";
    }

    void TearDown() override
    {
        // 清理OpenSSL
        EVP_cleanup();
        ERR_free_strings();
    }

    std::string plainText;
    std::string shortText;
    std::string emptyText;
    std::string specialChars;
};

// 测试Base64编码解码基本功能
TEST_F(Base64Test, EncodeDecodeBasic)
{
    std::string encoded = base64Encode(plainText);
    std::string decoded = base64Decode(encoded);

    EXPECT_EQ(decoded, plainText);
    EXPECT_FALSE(encoded.empty());
    EXPECT_NE(encoded, plainText);
}

// 测试短文本编码解码
TEST_F(Base64Test, EncodeDecodeShortText)
{
    std::string encoded = base64Encode(shortText);
    std::string decoded = base64Decode(encoded);

    EXPECT_EQ(decoded, shortText);
}

// 测试空文本编码解码
TEST_F(Base64Test, EncodeDecodeEmptyText)
{
    std::string encoded = base64Encode(emptyText);
    std::string decoded = base64Decode(encoded);

    EXPECT_EQ(decoded, emptyText);
    EXPECT_TRUE(decoded.empty());
}

// 测试长文本编码解码
TEST_F(Base64Test, EncodeDecodeLongText)
{
    std::string longText(10000, 'A');
    longText += "MiddlePart";
    longText += std::string(10000, 'Z');

    std::string encoded = base64Encode(longText);
    std::string decoded = base64Decode(encoded);

    EXPECT_EQ(decoded, longText);
}

// 测试特殊字符编码解码
TEST_F(Base64Test, EncodeDecodeSpecialCharacters)
{
    std::string encoded = base64Encode(specialChars);
    std::string decoded = base64Decode(encoded);

    EXPECT_EQ(decoded, specialChars);
}

// 测试二进制数据编码解码
TEST_F(Base64Test, EncodeDecodeBinaryData)
{
    // 创建包含二进制数据的字符串（可能包含空字符等）
    std::vector<unsigned char> binaryData = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                             0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                                             0xFF, 0xFE, 0xFD, 0xFC, 0x00, 0x7F, 0x80, 0x81};
    std::string binaryText(binaryData.begin(), binaryData.end());

    std::string encoded = base64Encode(binaryText);
    std::string decoded = base64Decode(encoded);

    EXPECT_EQ(decoded, binaryText);
}

// 测试填充情况
TEST_F(Base64Test, EncodeDecodePadding)
{
    // 测试不同长度的输入以验证填充行为
    std::string text1 = "1";    // 需要2个填充字符
    std::string text2 = "12";   // 需要1个填充字符
    std::string text3 = "123";  // 不需要填充字符
    std::string text4 = "1234"; // 需要2个填充字符

    EXPECT_EQ(base64Decode(base64Encode(text1)), text1);
    EXPECT_EQ(base64Decode(base64Encode(text2)), text2);
    EXPECT_EQ(base64Decode(base64Encode(text3)), text3);
    EXPECT_EQ(base64Decode(base64Encode(text4)), text4);
}

// 测试Base64编码的幂等性（多次编码解码）
TEST_F(Base64Test, EncodeDecodeIdempotent)
{
    std::string encoded1 = base64Encode(plainText);
    std::string encoded2 = base64Encode(plainText);

    // 相同输入应该产生相同的Base64输出
    EXPECT_EQ(encoded1, encoded2);

    // 多次编码解码应该得到相同结果
    std::string decoded1 = base64Decode(encoded1);
    std::string doubleDecoded = base64Decode(base64Encode(decoded1));

    EXPECT_EQ(decoded1, plainText);
    EXPECT_EQ(doubleDecoded, plainText);
}

// 测试十六进制转换与Base64配合使用
TEST_F(Base64Test, WithHexConversion)
{
    std::string encoded = base64Encode(plainText);

    // 转换为十六进制
    std::string hexEncoded = toHex(encoded);

    // 从十六进制转换回
    std::string fromHexEncoded = fromHex(hexEncoded);

    // 验证转换前后一致
    EXPECT_EQ(encoded, fromHexEncoded);

    // 解码应该仍然工作
    std::string decoded = base64Decode(fromHexEncoded);
    EXPECT_EQ(decoded, plainText);
}

// 测试已知的Base64编码解码值
TEST_F(Base64Test, KnownValues)
{
    // 测试一些标准的Base64编码值
    struct TestCase
    {
        std::string plain;
        std::string base64;
    };

    std::vector<TestCase> testCases
        = {{"", ""},
           {"f", "Zg=="},
           {"fo", "Zm8="},
           {"foo", "Zm9v"},
           {"foob", "Zm9vYg=="},
           {"fooba", "Zm9vYmE="},
           {"foobar", "Zm9vYmFy"},
           {"Hello World!", "SGVsbG8gV29ybGQh"},
           {"The quick brown fox jumps over the lazy dog",
            "VGhlIHF1aWNrIGJyb3duIGZveCBqdW1wcyBvdmVyIHRoZSBsYXp5IGRvZw=="}};

    for (const auto &testCase : testCases) {
        std::string encoded = base64Encode(testCase.plain);
        std::string decoded = base64Decode(testCase.base64);

        EXPECT_EQ(encoded, testCase.base64);
        EXPECT_EQ(decoded, testCase.plain);
    }
}

// 测试无效Base64字符串解码
TEST_F(Base64Test, DecodeInvalidBase64)
{
    // 包含无效字符的Base64字符串
    std::string invalidBase64 = "Invalid!!!Base64@@@String###";

    try {
        std::string decoded = base64Decode(invalidBase64);
        // 如果解码"成功"，我们至少验证它不是原始数据
        EXPECT_NE(decoded, invalidBase64);
    } catch (const std::exception &e) {
        // 抛出异常也是可以接受的行为
        SUCCEED();
    }
}

// 测试不完整填充的Base64字符串
TEST_F(Base64Test, DecodeIncompletePadding)
{
    // 不完整填充的Base64字符串
    std::string incompleteBase64 = "SGVsbG8gV29ybGQ"; // 缺少填充

    try {
        std::string decoded = base64Decode(incompleteBase64);
        // 可能成功解码，验证内容
        EXPECT_EQ(decoded, "Hello World");
    } catch (const std::exception &e) {
        // 抛出异常也是可以接受的行为
        SUCCEED();
    }
}

// 性能测试：多次编码解码
TEST_F(Base64Test, PerformanceTest)
{
    const int iterations = 1000;
    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iterations; ++i) {
        std::string encoded = base64Encode(plainText);
        std::string decoded = base64Decode(encoded);
        EXPECT_EQ(decoded, plainText);
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    // 输出性能信息
    std::cout << "Base64 Performance: " << iterations << " iterations took " << duration.count()
              << " ms" << std::endl;

    // 可以根据需要设置性能阈值
    EXPECT_LE(duration.count(), 1000); // 1秒内完成1000次操作
}

// 测试Base64编码的字符集
TEST_F(Base64Test, Base64CharacterSet)
{
    std::string encoded = base64Encode(plainText);

    // Base64应该只包含有效字符：A-Z, a-z, 0-9, +, /, =
    for (char c : encoded) {
        bool isValidChar = (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')
                           || (c >= '0' && c <= '9') || (c == '+') || (c == '/') || (c == '=');
        EXPECT_TRUE(isValidChar) << "Invalid Base64 character: " << c;
    }
}

// 测试编码解码随机数据
TEST_F(Base64Test, EncodeDecodeRandomData)
{
    const int dataSize = 256;
    std::vector<unsigned char> randomData(dataSize);

    // 生成随机数据
    if (RAND_bytes(randomData.data(), dataSize) == 1) {
        std::string randomText(randomData.begin(), randomData.end());

        std::string encoded = base64Encode(randomText);
        std::string decoded = base64Decode(encoded);

        EXPECT_EQ(decoded, randomText);
    } else {
        std::cout << "Warning: Random data generation failed, skipping random test" << std::endl;
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

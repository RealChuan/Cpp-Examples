#include "openssl_common.hpp"

#include <gtest/gtest.h>

#include <chrono>
#include <stdexcept>

std::string hash(const std::string &plain, const EVP_MD *type)
{
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx)
        handleOpenSSLError();

    if (EVP_DigestInit_ex(ctx, type, nullptr) != 1) {
        EVP_MD_CTX_free(ctx);
        handleOpenSSLError();
    }

    if (EVP_DigestUpdate(ctx, plain.data(), plain.size()) != 1) {
        EVP_MD_CTX_free(ctx);
        handleOpenSSLError();
    }

    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int digestLen;
    if (EVP_DigestFinal_ex(ctx, digest, &digestLen) != 1) {
        EVP_MD_CTX_free(ctx);
        handleOpenSSLError();
    }

    EVP_MD_CTX_free(ctx);

    return std::string(reinterpret_cast<char *>(digest), digestLen);
}

class HashTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // 初始化OpenSSL
        OpenSSL_add_all_algorithms();
        ERR_load_crypto_strings();

        // 测试数据
        plainText = "Hello, Hash Testing World! This is a test message.";
        shortText = "Short";
        emptyText = "";
        specialChars = "Special!@#$%^&*()_+{}\":?><,./;'[]\\|-=~`";

        // 已知的哈希测试向量
        testVectors[""] = {{EVP_md5(), "d41d8cd98f00b204e9800998ecf8427e"},
                           {EVP_sha1(), "da39a3ee5e6b4b0d3255bfef95601890afd80709"},
                           {EVP_sha256(),
                            "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"},
                           {EVP_sha512(),
                            "cf83e1357eefb8bdf1542850d66d8007d620e4050b5715dc83f4a921d36ce9ce47d0d1"
                            "3c5d85f2b0ff8318d2877eec2f63b931bd47417a81a538327af927da3e"}};

        testVectors["abc"] = {{EVP_md5(), "900150983cd24fb0d6963f7d28e17f72"},
                              {EVP_sha1(), "a9993e364706816aba3e25717850c26c9cd0d89d"},
                              {EVP_sha256(),
                               "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"},
                              {EVP_sha512(),
                               "ddaf35a193617abacc417349ae20413112e6fa4e89a97ea20a9eeee64b55d39a219"
                               "2992a274fc1a836ba3c23a3feebbd454d4423643ce80e2a9ac94fa54ca49f"}};

        testVectors["The quick brown fox jumps over the lazy dog"]
            = {{EVP_md5(), "9e107d9d372bb6826bd81d3542a419d6"},
               {EVP_sha1(), "2fd4e1c67a2d28fced849ee1bb76e7391b93eb12"},
               {EVP_sha256(), "d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592"},
               {EVP_sha512(),
                "07e547d9586f6a73f73fbac0435ed76951218fb7d0c8d788a309d785436bbb642e93a252a954f23912"
                "547d1e8a3b5ed6e1bfd7097821233fa0538f3db854fee6"}};
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

    // 测试向量：输入文本 -> 算法 -> 期望的哈希值(十六进制)
    std::map<std::string, std::map<const EVP_MD *, std::string>> testVectors;
};

// 测试MD5哈希
TEST_F(HashTest, MD5Hash)
{
    std::string hashResult = hash(plainText, EVP_md5());
    std::string hexResult = toHex(hashResult);

    EXPECT_FALSE(hashResult.empty());
    EXPECT_EQ(hashResult.length(), 16); // MD5 produces 128-bit (16-byte) hash
    EXPECT_FALSE(hexResult.empty());
}

// 测试SHA1哈希
TEST_F(HashTest, SHA1Hash)
{
    std::string hashResult = hash(plainText, EVP_sha1());
    std::string hexResult = toHex(hashResult);

    EXPECT_FALSE(hashResult.empty());
    EXPECT_EQ(hashResult.length(), 20); // SHA1 produces 160-bit (20-byte) hash
    EXPECT_FALSE(hexResult.empty());
}

// 测试SHA256哈希
TEST_F(HashTest, SHA256Hash)
{
    std::string hashResult = hash(plainText, EVP_sha256());
    std::string hexResult = toHex(hashResult);

    EXPECT_FALSE(hashResult.empty());
    EXPECT_EQ(hashResult.length(), 32); // SHA256 produces 256-bit (32-byte) hash
    EXPECT_FALSE(hexResult.empty());
}

// 测试SHA512哈希
TEST_F(HashTest, SHA512Hash)
{
    std::string hashResult = hash(plainText, EVP_sha512());
    std::string hexResult = toHex(hashResult);

    EXPECT_FALSE(hashResult.empty());
    EXPECT_EQ(hashResult.length(), 64); // SHA512 produces 512-bit (64-byte) hash
    EXPECT_FALSE(hexResult.empty());
}

// 测试空字符串哈希
TEST_F(HashTest, EmptyStringHash)
{
    std::vector<const EVP_MD *> algorithms = {EVP_md5(), EVP_sha1(), EVP_sha256(), EVP_sha512()};

    for (const auto algo : algorithms) {
        std::string hashResult = hash(emptyText, algo);
        EXPECT_FALSE(hashResult.empty());

        // 验证哈希长度
        if (algo == EVP_md5()) {
            EXPECT_EQ(hashResult.length(), 16);
        } else if (algo == EVP_sha1()) {
            EXPECT_EQ(hashResult.length(), 20);
        } else if (algo == EVP_sha256()) {
            EXPECT_EQ(hashResult.length(), 32);
        } else if (algo == EVP_sha512()) {
            EXPECT_EQ(hashResult.length(), 64);
        }
    }
}

// 测试短字符串哈希
TEST_F(HashTest, ShortStringHash)
{
    std::string hashResult = hash(shortText, EVP_sha256());
    std::string hexResult = toHex(hashResult);

    EXPECT_FALSE(hashResult.empty());
    EXPECT_EQ(hashResult.length(), 32);
    EXPECT_FALSE(hexResult.empty());
}

// 测试特殊字符哈希
TEST_F(HashTest, SpecialCharactersHash)
{
    std::string hashResult = hash(specialChars, EVP_sha256());
    std::string hexResult = toHex(hashResult);

    EXPECT_FALSE(hashResult.empty());
    EXPECT_EQ(hashResult.length(), 32);
    EXPECT_FALSE(hexResult.empty());
}

// 测试长字符串哈希
TEST_F(HashTest, LongStringHash)
{
    std::string longText(10000, 'A');
    longText += "MiddlePart";
    longText += std::string(10000, 'Z');

    std::string hashResult = hash(longText, EVP_sha256());
    std::string hexResult = toHex(hashResult);

    EXPECT_FALSE(hashResult.empty());
    EXPECT_EQ(hashResult.length(), 32);
    EXPECT_FALSE(hexResult.empty());
}

// 测试二进制数据哈希
TEST_F(HashTest, BinaryDataHash)
{
    // 创建包含二进制数据的字符串（可能包含空字符等）
    std::vector<unsigned char> binaryData = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                                             0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F,
                                             0xFF, 0xFE, 0xFD, 0xFC, 0x00, 0x7F, 0x80, 0x81};
    std::string binaryText(binaryData.begin(), binaryData.end());

    std::string hashResult = hash(binaryText, EVP_sha256());
    std::string hexResult = toHex(hashResult);

    EXPECT_FALSE(hashResult.empty());
    EXPECT_EQ(hashResult.length(), 32);
    EXPECT_FALSE(hexResult.empty());
}

// 测试哈希的确定性（相同输入产生相同输出）
TEST_F(HashTest, HashDeterministic)
{
    std::string hash1 = hash(plainText, EVP_sha256());
    std::string hash2 = hash(plainText, EVP_sha256());
    std::string hash3 = hash(plainText, EVP_sha256());

    EXPECT_EQ(hash1, hash2);
    EXPECT_EQ(hash2, hash3);
    EXPECT_EQ(hash1, hash3);
}

// 测试不同输入产生不同哈希（避免碰撞的基本测试）
TEST_F(HashTest, DifferentInputsDifferentHashes)
{
    std::string hash1 = hash("input1", EVP_sha256());
    std::string hash2 = hash("input2", EVP_sha256());
    std::string hash3 = hash("input3", EVP_sha256());

    EXPECT_NE(hash1, hash2);
    EXPECT_NE(hash1, hash3);
    EXPECT_NE(hash2, hash3);
}

// 测试已知的哈希值（标准测试向量）
TEST_F(HashTest, KnownHashVectors)
{
    for (const auto &[input, algorithms] : testVectors) {
        for (const auto &[algo, expectedHex] : algorithms) {
            std::string hashResult = hash(input, algo);
            std::string actualHex = toHex(hashResult);

            EXPECT_EQ(actualHex, expectedHex) << "Hash mismatch for input: '" << input
                                              << "' with algorithm: " << EVP_MD_name(algo);
        }
    }
}

// 测试所有支持的哈希算法
TEST_F(HashTest, AllSupportedAlgorithms)
{
    struct AlgorithmTest
    {
        const EVP_MD *algorithm;
        size_t expectedLength;
        const char *name;
    };

    std::vector<AlgorithmTest> algorithms = {{EVP_md5(), 16, "MD5"},
                                             {EVP_sha1(), 20, "SHA1"},
                                             {EVP_sha224(), 28, "SHA224"},
                                             {EVP_sha256(), 32, "SHA256"},
                                             {EVP_sha384(), 48, "SHA384"},
                                             {EVP_sha512(), 64, "SHA512"}};

    for (const auto &test : algorithms) {
        // 检查算法是否可用
        if (test.algorithm != nullptr) {
            std::string hashResult = hash(plainText, test.algorithm);
            EXPECT_FALSE(hashResult.empty());
            EXPECT_EQ(hashResult.length(), test.expectedLength)
                << "Wrong hash length for algorithm: " << test.name;

            std::string hexResult = toHex(hashResult);
            EXPECT_FALSE(hexResult.empty());
        }
    }
}

// 测试哈希与十六进制转换的配合
TEST_F(HashTest, HashWithHexConversion)
{
    std::string hashResult = hash(plainText, EVP_sha256());

    // 转换为十六进制
    std::string hexResult = toHex(hashResult);

    // 从十六进制转换回
    std::string fromHexResult = fromHex(hexResult);

    // 验证转换前后一致
    EXPECT_EQ(hashResult, fromHexResult);
    EXPECT_EQ(hashResult.length(), fromHexResult.length());
}

// 测试多次哈希的一致性
TEST_F(HashTest, MultipleHashingConsistency)
{
    const int iterations = 100;
    std::string firstHash = hash(plainText, EVP_sha256());

    for (int i = 0; i < iterations; ++i) {
        std::string currentHash = hash(plainText, EVP_sha256());
        EXPECT_EQ(firstHash, currentHash) << "Hash inconsistency at iteration: " << i;
    }
}

// 性能测试：多次哈希操作
TEST_F(HashTest, PerformanceTest)
{
    const int iterations = 1000;
    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < iterations; ++i) {
        std::string hashResult = hash(plainText, EVP_sha256());
        EXPECT_FALSE(hashResult.empty());
        EXPECT_EQ(hashResult.length(), 32);
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

    // 输出性能信息
    std::cout << "Hash Performance: " << iterations << " SHA256 iterations took "
              << duration.count() << " ms" << std::endl;

    // 可以根据需要设置性能阈值
    EXPECT_LE(duration.count(), 1000); // 1秒内完成1000次哈希操作
}

// 测试哈希算法的唯一性（基本测试）
TEST_F(HashTest, HashUniqueness)
{
    // 测试一些相似但不相同的输入
    std::vector<std::string> similarInputs
        = {"password", "Password", "password1", "password ", " password", "pass word"};

    std::set<std::string> hashes;
    for (const auto &input : similarInputs) {
        std::string hashResult = hash(input, EVP_sha256());
        std::string hexResult = toHex(hashResult);
        hashes.insert(hexResult);
    }

    // 所有哈希值应该是唯一的
    EXPECT_EQ(hashes.size(), similarInputs.size());
}

// 测试大文件模拟哈希（分块处理）
TEST_F(HashTest, LargeDataHash)
{
    // 模拟大文件哈希 - 创建大量数据
    const size_t chunkSize = 4096;
    const size_t numChunks = 100;

    std::string largeData;
    largeData.reserve(chunkSize * numChunks);

    // 生成伪随机但确定性的数据
    for (size_t i = 0; i < numChunks; ++i) {
        std::string chunk(chunkSize, 'A' + (i % 26));
        largeData += chunk;
    }

    // 哈希大量数据
    std::string hashResult = hash(largeData, EVP_sha256());
    std::string hexResult = toHex(hashResult);

    EXPECT_FALSE(hashResult.empty());
    EXPECT_EQ(hashResult.length(), 32);
    EXPECT_FALSE(hexResult.empty());

    // 验证相同数据产生相同哈希
    std::string hashResult2 = hash(largeData, EVP_sha256());
    EXPECT_EQ(hashResult, hashResult2);
}

// 测试null算法处理（应该抛出异常或返回错误）
TEST_F(HashTest, NullAlgorithm)
{
    try {
        std::string hashResult = hash(plainText, nullptr);
        // 如果函数没有抛出异常，至少验证结果为空或处理了错误情况
        EXPECT_TRUE(hashResult.empty());
    } catch (const std::exception &e) {
        // 抛出异常是预期的行为
        SUCCEED();
    } catch (...) {
        FAIL() << "Unexpected exception type for null algorithm";
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

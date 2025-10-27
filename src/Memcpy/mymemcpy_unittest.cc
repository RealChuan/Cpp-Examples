#include "mymemcpy.hpp"

#include <gtest/gtest.h>

#include <cstring>

class MyMemcpyTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // 测试前的设置
    }

    void TearDown() override
    {
        // 测试后的清理
    }

    // 辅助函数：比较两个内存区域是否相同
    bool memoryEqual(const void *ptr1, const void *ptr2, size_t n)
    {
        return memcmp(ptr1, ptr2, n) == 0;
    }
};

// 测试1: 正常字符串复制
TEST_F(MyMemcpyTest, CopyNormalString)
{
    const char src[] = "Hello, World!";
    char dest_my[20] = {0};
    char dest_std[20] = {0};

    void *result_my = my_memcpy(dest_my, src, strlen(src) + 1);
    void *result_std = memcpy(dest_std, src, strlen(src) + 1);

    // 验证返回值正确
    EXPECT_EQ(result_my, dest_my);

    // 验证复制结果正确
    EXPECT_STREQ(dest_my, src);
    EXPECT_STREQ(dest_my, dest_std);

    // 验证内存内容相同
    EXPECT_TRUE(memoryEqual(dest_my, dest_std, strlen(src) + 1));
}

// 测试2: 整数数组复制
TEST_F(MyMemcpyTest, CopyIntArray)
{
    int src[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    int dest_my[10] = {0};
    int dest_std[10] = {0};

    my_memcpy(dest_my, src, sizeof(src));
    memcpy(dest_std, src, sizeof(src));

    EXPECT_TRUE(memoryEqual(dest_my, dest_std, sizeof(src)));

    // 验证每个元素都正确复制
    for (int i = 0; i < 10; i++) {
        EXPECT_EQ(dest_my[i], src[i]);
    }
}

// 测试3: 零字节复制
TEST_F(MyMemcpyTest, CopyZeroBytes)
{
    char src[] = "Source";
    char dest_my[] = "Destination";
    char dest_std[] = "Destination";

    void *result_my = my_memcpy(dest_my, src, 0);
    void *result_std = memcpy(dest_std, src, 0);

    EXPECT_EQ(result_my, dest_my);
    EXPECT_STREQ(dest_my, dest_std); // 目标不应改变
}

// 测试4: 部分复制
TEST_F(MyMemcpyTest, CopyPartialData)
{
    const char src[] = "Hello, World!";
    char dest_my[20] = {0};
    char dest_std[20] = {0};

    // 只复制前5个字节
    my_memcpy(dest_my, src, 5);
    memcpy(dest_std, src, 5);

    EXPECT_TRUE(memoryEqual(dest_my, dest_std, 5));

    // 验证只复制了指定数量的字节
    EXPECT_EQ(dest_my[0], 'H');
    EXPECT_EQ(dest_my[4], 'o');
    // 第6个字节应该是初始值（0）
    EXPECT_EQ(dest_my[5], 0);
}

// 测试5: 结构体复制
TEST_F(MyMemcpyTest, CopyStruct)
{
    struct TestStruct
    {
        int id;
        char name[20];
        double value;
    };

    TestStruct src = {42, "Test Name", 3.14159};
    TestStruct dest_my = {0};
    TestStruct dest_std = {0};

    my_memcpy(&dest_my, &src, sizeof(TestStruct));
    memcpy(&dest_std, &src, sizeof(TestStruct));

    EXPECT_TRUE(memoryEqual(&dest_my, &dest_std, sizeof(TestStruct)));
    EXPECT_EQ(dest_my.id, src.id);
    EXPECT_STREQ(dest_my.name, src.name);
    EXPECT_DOUBLE_EQ(dest_my.value, src.value);
}

// 测试6: NULL目标指针（应该返回NULL）
TEST_F(MyMemcpyTest, NullDestination)
{
    char src[] = "Test";
    void *result = my_memcpy(NULL, src, 5);
    EXPECT_EQ(result, nullptr);
}

// 测试7: NULL源指针（应该返回目标指针）
TEST_F(MyMemcpyTest, NullSource)
{
    char dest[10] = {0};
    void *result = my_memcpy(dest, NULL, 5);
    EXPECT_EQ(result, dest);
}

// 测试8: 两个NULL指针（应该返回NULL）
TEST_F(MyMemcpyTest, BothNull)
{
    void *result = my_memcpy(NULL, NULL, 5);
    EXPECT_EQ(result, nullptr);
}

// 测试9: 单字节复制
TEST_F(MyMemcpyTest, CopySingleByte)
{
    char src = 'A';
    char dest_my = 'B';
    char dest_std = 'B';

    my_memcpy(&dest_my, &src, 1);
    memcpy(&dest_std, &src, 1);

    EXPECT_EQ(dest_my, 'A');
    EXPECT_EQ(dest_my, dest_std);
}

// 测试10: 大内存块复制
TEST_F(MyMemcpyTest, CopyLargeBlock)
{
    const size_t SIZE = 10000;
    char *src = new char[SIZE];
    char *dest_my = new char[SIZE];
    char *dest_std = new char[SIZE];

    // 初始化源数据
    for (size_t i = 0; i < SIZE; i++) {
        src[i] = static_cast<char>(i % 256);
    }

    my_memcpy(dest_my, src, SIZE);
    memcpy(dest_std, src, SIZE);

    EXPECT_TRUE(memoryEqual(dest_my, dest_std, SIZE));

    delete[] src;
    delete[] dest_my;
    delete[] dest_std;
}

// 测试11: 重叠内存测试（memcpy不保证处理重叠，但我们的实现应该与标准行为一致）
TEST_F(MyMemcpyTest, NonOverlappingMemory)
{
    char buffer[] = "1234567890";
    char expected[] = "1234567890";

    // 复制到不重叠的位置
    my_memcpy(buffer + 5, buffer, 5);
    memcpy(expected + 5, expected, 5);

    EXPECT_TRUE(memoryEqual(buffer, expected, sizeof(buffer)));
}

// 主函数
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

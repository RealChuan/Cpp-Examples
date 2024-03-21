#include "memcpy.hpp"

#include <gtest/gtest.h>

#include <array>
#include <iostream>

TEST(custom_memcpy, normal)
{
    const char *src = "www.baidu.com";
    std::array<char, 50> dest = {0};
    custom_memcpy(dest.data(), src, strlen(src) + 1);
    EXPECT_STREQ(dest.data(), src);
}

TEST(custom_memcpy, normal2)
{
    const char *src = "www.baidu.com";
    std::array<char, 50> dest = {0};
    custom_memcpy(dest.data(), src + 4, 5);
    EXPECT_STREQ(dest.data(), "baidu");
}

TEST(custom_memcpy, normal3)
{
    const char *src = "hello world";
    std::array<char, 50> dest = {0};
    custom_memcpy(dest.data(), src, strlen(src) + 1);
    EXPECT_STREQ(dest.data(), src);
}

TEST(custom_memcpy, normal4)
{
    const char *src = "hello world";
    std::array<char, 50> dest = {0};
    custom_memcpy(dest.data(), src, 6);
    EXPECT_STREQ(dest.data(), "hello ");
}

TEST(custom_memcpy, null)
{
    const char *src = nullptr;
    char *dest = nullptr;
    custom_memcpy(dest, src, 1);
    EXPECT_STREQ(dest, dest);
}

TEST(custom_memcpy, null2)
{
    const char *src = "hello world";
    char *dest = nullptr;
    custom_memcpy(dest, src, 1);
    EXPECT_STREQ(dest, dest);
}

TEST(custom_memcpy, null3)
{
    const char *src = nullptr;
    std::array<char, 50> dest = {"hello world"};
    custom_memcpy(dest.data(), src, 1);
    EXPECT_STREQ(dest.data(), dest.data());
}

TEST(custom_memcpy, zero)
{
    const char *src = "hello world";
    std::array<char, 50> dest = {0};
    custom_memcpy(dest.data(), src, 0);
    EXPECT_STREQ(dest.data(), "");
}

TEST(custom_memcpy, overlay)
{
    std::array<char, 50> src = {"www.baidu.com"};
    custom_memcpy(src.data() + strlen(src.data()), src.data(), strlen(src.data()) + 1);
    EXPECT_STREQ(src.data(), "www.baidu.comwww.baidu.com");
}

TEST(custom_memcpy, overlay2)
{
    std::array<char, 50> src = {"www.baidu.com"};
    custom_memcpy(src.data() + 4, src.data() + 10, 3);
    EXPECT_STREQ(src.data(), "www.comdu.com");
}

auto main(int argc, char *argv[]) -> int
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

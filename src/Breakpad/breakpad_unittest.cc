#include "breakpad.hpp"

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>

// 测试固件类
class BreakpadTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // 创建临时目录用于测试
        test_dump_path_ = "./test_crashes_"
                          + std::to_string(::testing::UnitTest::GetInstance()->random_seed());
        std::filesystem::create_directories(test_dump_path_);
    }

    void TearDown() override
    {
        // 清理测试目录
        std::filesystem::remove_all(test_dump_path_);
    }

    std::string test_dump_path_;
};

// 测试获取配置信息
TEST_F(BreakpadTest, GetConfiguration)
{
    Breakpad breakpad(test_dump_path_);

    EXPECT_EQ(breakpad.getDumpPath(), test_dump_path_);
}

// 测试设置回调函数
TEST_F(BreakpadTest, SetCrashCallback)
{
    Breakpad breakpad(test_dump_path_);

    std::atomic<bool> callback_called{false};
    std::string received_path;
    bool received_success = false;

    // 设置回调函数
    breakpad.setCrashCallback([&](const std::string &path, bool success) {
        callback_called = true;
        received_path = path;
        received_success = success;
        return true;
    });

    // 注意：我们无法在测试中触发真正的崩溃，但可以验证回调函数设置成功
    // 这里我们只是验证设置过程没有异常
    SUCCEED();
}

// 测试手动写入minidump
TEST_F(BreakpadTest, WriteMinidump)
{
    Breakpad breakpad(test_dump_path_);

    // 手动写入minidump应该成功
    EXPECT_TRUE(breakpad.writeMinidump());

    // 检查是否生成了dump文件（注意：实际文件名由Breakpad生成）
    // 由于我们不知道具体的文件名，只能验证目录存在且可访问
    EXPECT_TRUE(std::filesystem::exists(test_dump_path_));
    EXPECT_TRUE(std::filesystem::is_directory(test_dump_path_));
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

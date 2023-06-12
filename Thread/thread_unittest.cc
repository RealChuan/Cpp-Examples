#include "thread.hpp"

#include <gtest/gtest.h>

TEST(Thread, start)
{
    Thread thread([]() { Thread::sleepFor(std::chrono::milliseconds(100)); });
    thread.start();
    thread.waitForStarted();
    EXPECT_TRUE(thread.isRunning());
    thread.stop();
    EXPECT_FALSE(thread.isRunning());
}

TEST(Thread, setTask)
{
    Thread thread;
    thread.setTask([]() { Thread::sleepFor(std::chrono::milliseconds(100)); });
    thread.start();
    thread.waitForStarted();
    EXPECT_TRUE(thread.isRunning());
    thread.stop();
    EXPECT_FALSE(thread.isRunning());
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

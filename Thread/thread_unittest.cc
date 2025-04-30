#include "thread.hpp"

#include <gtest/gtest.h>

void testTaskFunction(std::stop_token stopToken)
{
    for (int i = 0; i < 5; ++i) {
        if (stopToken.stop_requested()) {
            break;
        }

        std::cout << "Test task is running: " << i << std::endl;
        Thread::msleep(200);
    }
}

TEST(ThreadTest, StartAndStop)
{
    Thread thread;
    thread.setTask(testTaskFunction);
    thread.start();
    EXPECT_TRUE(thread.isRunning());
    thread.stop();
    EXPECT_FALSE(thread.isRunning());
}

TEST(ThreadTest, Interrupt)
{
    Thread thread;
    thread.setTask(testTaskFunction);
    thread.start();
    EXPECT_TRUE(thread.isRunning());
    thread.interrupt();
    Thread::sleep(1);
    EXPECT_FALSE(thread.isRunning());
}

TEST(ThreadTest, WaitForFinished)
{
    Thread thread;
    thread.setTask(testTaskFunction);
    thread.start();
    thread.waitForFinished();
    EXPECT_FALSE(thread.isRunning());
}

TEST(ThreadTest, WaitForFinished2)
{
    Thread thread;
    thread.waitForFinished();
    EXPECT_FALSE(thread.isRunning());
}

TEST(ThreadTest, TaskExecution)
{
    Thread thread;
    bool taskExecuted = false;
    thread.setTask([&taskExecuted](std::stop_token) {
        taskExecuted = true;
        std::cout << "Task executed." << std::endl;
    });
    thread.start();
    thread.stop();
    EXPECT_TRUE(taskExecuted);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

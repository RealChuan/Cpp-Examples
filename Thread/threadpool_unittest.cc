#include "threadpool.hpp"

#include <gtest/gtest.h>

void testTaskFunction(std::stop_token stopToken, std::atomic<int> &counter)
{
    while (!stopToken.stop_requested()) {
        counter++;
        Thread::msleep(100);
    }
}

TEST(ThreadPoolTest, StartAndStop)
{
    ThreadPool pool(10, 4);
    pool.start();
    EXPECT_TRUE(pool.isRunning());
    EXPECT_EQ(pool.activeThreadCount(), 4);
    pool.stop();
    EXPECT_FALSE(pool.isRunning());
    EXPECT_EQ(pool.activeThreadCount(), 0);
}

TEST(ThreadPoolTest, AddTask)
{
    ThreadPool pool(10, 4);
    pool.start();
    std::atomic<int> counter = 0;
    for (int i = 0; i < 10; ++i) {
        pool.addTask([&counter](std::stop_token token) { testTaskFunction(token, counter); });
    }
    Thread::sleep(1);
    EXPECT_GT(counter.load(), 0);
    pool.stop();
}

TEST(ThreadPoolTest, AddTask2)
{
    ThreadPool pool(10, 4);
    for (int i = 0; i < 9999; ++i) {
        pool.addTask([](std::stop_token token) {});
    }
    EXPECT_EQ(pool.queuedTaskCount(), 0);
    pool.stop();
}

TEST(ThreadPoolTest, WaitForDone)
{
    ThreadPool pool(10, 4);
    pool.start();
    pool.waitForDone();
    EXPECT_EQ(pool.queuedTaskCount(), 0);
    pool.stop();
}

TEST(ThreadPoolTest, WaitForDone2)
{
    ThreadPool pool(10, 4);
    pool.start();
    std::atomic<int> counter = 0;
    const int taskCount = 9999;
    for (int i = 0; i < taskCount; ++i) {
        pool.addTask([&counter](std::stop_token token) { counter.fetch_add(1); });
    }
    pool.waitForDone();
    EXPECT_EQ(counter.load(), taskCount);
    EXPECT_EQ(pool.queuedTaskCount(), 0);
    pool.stop();
}

TEST(ThreadPoolTest, ClearTasks)
{
    ThreadPool pool(10, 4);
    pool.start();
    for (int i = 0; i < 10; ++i) {
        pool.addTask([](std::stop_token) {});
    }
    pool.clearTasks();
    EXPECT_EQ(pool.queuedTaskCount(), 0);
    pool.stop();
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

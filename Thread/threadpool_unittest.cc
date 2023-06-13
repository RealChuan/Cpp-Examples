#include "threadpool.hpp"

#include <gtest/gtest.h>

TEST(ThreadPool, test)
{
    ThreadPool pool(100, 4);
    pool.start();
    std::atomic_int count{0};
    for (int i = 0; i < 100; ++i) {
        pool.addTask([&count]() { ++count; });
    }
    pool.waitForDone();
    EXPECT_EQ(count, 100);
}

TEST(ThreadPool, test2)
{
    ThreadPool pool(100, 4);
    pool.start();
    std::atomic_int count{0};
    for (int i = 0; i < 100; ++i) {
        pool.addTask([&count]() { ++count; });
    }
    EXPECT_TRUE(pool.activeThreadCount() == 4);
    pool.clearTasks();
    EXPECT_TRUE(pool.queuedTaskCount() == 0);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

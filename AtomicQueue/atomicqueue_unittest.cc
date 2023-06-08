#include "atomicqueue.hpp"

#include <gtest/gtest.h>

TEST(AtomicQueue, PushPop)
{
    AtomicQueue<int> queue;
    queue.push(1);
    queue.push(2);
    queue.push(3);
    EXPECT_EQ(1, queue.front());
    EXPECT_EQ(3, queue.back());
    EXPECT_EQ(3, queue.size());
    queue.pop();
    EXPECT_EQ(2, queue.front());
    EXPECT_EQ(3, queue.back());
    EXPECT_EQ(2, queue.size());
    queue.pop();
    EXPECT_EQ(3, queue.front());
    EXPECT_EQ(3, queue.back());
    EXPECT_EQ(1, queue.size());
    queue.pop();
    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(0, queue.size());
}

TEST(AtomicQueue, PushPopMany)
{
    AtomicQueue<int> queue;
    for (int i = 0; i < 1000; ++i) {
        queue.push(i);
    }
    EXPECT_EQ(0, queue.front());
    EXPECT_EQ(999, queue.back());
    EXPECT_EQ(1000, queue.size());
    for (int i = 0; i < 1000; ++i) {
        queue.pop();
    }
    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(0, queue.size());
}

TEST(AtomicQueue, PushPopManyThreads)
{
    AtomicQueue<int> queue;
    std::vector<std::thread> threads;
    for (int i = 0; i < 1000; ++i) {
        threads.emplace_back([&queue, i] { queue.push(i); });
    }
    for (auto &thread : threads) {
        thread.join();
    }
    EXPECT_EQ(1000, queue.size());
    for (int i = 0; i < 1000; ++i) {
        queue.pop();
    }
    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(0, queue.size());
}

TEST(AtomicQueue, PushPopManyThreads2)
{
    AtomicQueue<int> queue;
    std::vector<std::thread> threads;
    for (int i = 0; i < 1000; ++i) {
        threads.emplace_back([&queue, i] {
            queue.push(i);
            queue.pop();
        });
    }
    for (auto &thread : threads) {
        thread.join();
    }
    EXPECT_TRUE(queue.empty());
    EXPECT_EQ(0, queue.size());
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

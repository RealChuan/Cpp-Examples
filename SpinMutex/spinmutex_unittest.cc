#include "spinmutex.hpp"

#include <gtest/gtest.h>

TEST(SpinMutexTest, MultiThreadIncrement)
{
    SpinMutex mutex;
    std::vector<std::thread> threads;
    int counter = 0;

    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&] {
            for (int j = 0; j < 1000; ++j) {
                SpinMutexLocker locker(&mutex);
                ++counter;
            }
        });
    }

    for (auto &thread : threads) {
        thread.join();
    }

    EXPECT_EQ(counter, 10000);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#include "singleton.hpp"

#include <thread>

#include <gtest/gtest.h>

class SimpleTest : noncopyable
{
public:
    SimpleTest() { std::cout << "SimpleTest" << std::endl; }
    ~SimpleTest() { std::cout << "~SimpleTest" << std::endl; }
};

void test_singleton_multithread(auto getInstanceFunc)
{
    std::vector<void *> addresses;
    std::mutex mutex;

    const int threadCount = 100;
    std::vector<std::thread> threads;

    for (int i = 0; i < threadCount; ++i) {
        threads.emplace_back([&]() {
            auto *instance = getInstanceFunc();
            std::lock_guard<std::mutex> lock(mutex);
            addresses.push_back(instance);
        });
    }

    for (auto &thread : threads) {
        thread.join();
    }

    for (size_t i = 1; i < addresses.size(); ++i) {
        EXPECT_EQ(addresses[0], addresses[i]);
    }
}

TEST(test_lazy_singleton, test_singleton)
{
    Lazy::Singleton &singleton1 = Lazy::Singleton::instance();
    Lazy::Singleton &singleton2 = Lazy::Singleton::instance();
    EXPECT_EQ(&singleton1, &singleton2);
}

TEST(test_lazy_singleton, test_multi_thread)
{
    test_singleton_multithread([]() { return &Lazy::Singleton::instance(); });
}

TEST(test_hungry_singleton, test_singleton)
{
    Hungry::Singleton &singleton1 = Hungry::Singleton::instance();
    Hungry::Singleton &singleton2 = Hungry::Singleton::instance();
    EXPECT_EQ(&singleton1, &singleton2);
}

TEST(test_hungry_singleton, test_multi_thread)
{
    test_singleton_multithread([]() { return &Hungry::Singleton::instance(); });
}

TEST(test_lazy_template_singleton, test_singleton)
{
    auto &singleton1 = LazyTemplate::Singleton<SimpleTest>::instance();
    auto &singleton2 = LazyTemplate::Singleton<SimpleTest>::instance();
    EXPECT_EQ(&singleton1, &singleton2);
}

TEST(test_lazy_template_singleton, test_multi_thread)
{
    test_singleton_multithread([]() { return &LazyTemplate::Singleton<SimpleTest>::instance(); });
}

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

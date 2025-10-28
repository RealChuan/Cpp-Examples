#include "singleton.hpp"

#include <gtest/gtest.h>

#include <atomic>
#include <chrono>
#include <future>
#include <string>
#include <thread>
#include <vector>

// 测试用的单例类
class TestSingleton : public Singleton<TestSingleton>
{
    friend class Singleton<TestSingleton>;

private:
    TestSingleton()
        : value(0)
    {}

public:
    void setValue(int val) { value = val; }
    int getValue() const { return value; }
    void increment() { ++value; }

private:
    std::atomic<int> value;
};

// 另一个测试用的单例类
class AnotherSingleton
{
    SINGLETON(AnotherSingleton)

public:
    std::string getName() const { return "AnotherSingleton"; }
    void setData(const std::string &data) { m_data = data; } // 重命名避免冲突
    std::string getData() const { return m_data; }

private:
    std::string m_data; // 重命名避免警告
};

// 基础功能测试
class SingletonTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // 每个测试前重置单例状态（如果需要的话）
    }

    void TearDown() override
    {
        // 清理工作
    }
};

// 测试单例实例的唯一性
TEST_F(SingletonTest, InstanceUniqueness)
{
    TestSingleton &instance1 = TestSingleton::instance();
    TestSingleton &instance2 = TestSingleton::instance();
    TestSingleton *instance3 = TestSingleton::getInstance();

    // 验证所有获取方式返回的是同一个实例
    EXPECT_EQ(&instance1, &instance2);
    EXPECT_EQ(&instance1, instance3);
    EXPECT_EQ(&instance2, instance3);
}

// 测试单例的功能方法
TEST_F(SingletonTest, Functionality)
{
    TestSingleton &instance = TestSingleton::instance();

    // 测试设置和获取值
    instance.setValue(42);
    EXPECT_EQ(instance.getValue(), 42);

    // 测试修改值
    instance.increment();
    EXPECT_EQ(instance.getValue(), 43);
}

// 测试多个单例类的独立性
TEST_F(SingletonTest, MultipleSingletonIndependence)
{
    TestSingleton &testInstance = TestSingleton::instance();
    AnotherSingleton &anotherInstance = AnotherSingleton::instance();

    testInstance.setValue(100);
    anotherInstance.setData("test_data");

    // 验证两个单例实例不冲突
    EXPECT_EQ(testInstance.getValue(), 100);
    EXPECT_EQ(anotherInstance.getData(), "test_data");
    EXPECT_NE(reinterpret_cast<void *>(&testInstance), reinterpret_cast<void *>(&anotherInstance));
}

// 测试单例的不可拷贝和不可移动特性
TEST_F(SingletonTest, NonCopyableNonMovable)
{
    // 这些代码应该编译失败，我们通过SFINAE或编译时检查来验证
    // 在实际测试中，我们可以验证这些特性是否存在

    EXPECT_TRUE(std::is_copy_constructible_v<TestSingleton> == false);
    EXPECT_TRUE(std::is_copy_assignable_v<TestSingleton> == false);
    EXPECT_TRUE(std::is_move_constructible_v<TestSingleton> == false);
    EXPECT_TRUE(std::is_move_assignable_v<TestSingleton> == false);
}

// 辅助函数用于多线程测试
void threadTask(int threadId, std::promise<TestSingleton *> &promise)
{
    (void) threadId; // 标记未使用参数，消除警告
    TestSingleton &instance = TestSingleton::instance();
    instance.increment(); // 每个线程增加一次
    promise.set_value(&instance);
}

// 多线程安全性测试
TEST_F(SingletonTest, ThreadSafety)
{
    constexpr int numThreads = 10;

    // 创建一个新的单例类用于线程测试
    class ThreadTestSingleton : public Singleton<ThreadTestSingleton>
    {
        friend class Singleton<ThreadTestSingleton>;

    private:
        ThreadTestSingleton()
            : counter(0)
        {}

    public:
        void increment() { ++counter; }
        int getCounter() const { return counter; }

    private:
        std::atomic<int> counter;
    };

    // 启动多个线程同时访问单例
    std::vector<std::thread> testThreads;
    std::vector<std::future<ThreadTestSingleton *>> testFutures;
    std::vector<std::promise<ThreadTestSingleton *>> testPromises(numThreads);

    for (auto &promise : testPromises) {
        testFutures.push_back(promise.get_future());
    }

    for (int i = 0; i < numThreads; ++i) {
        testThreads.emplace_back([i, &testPromises]() {
            ThreadTestSingleton &instance = ThreadTestSingleton::instance();
            instance.increment();
            testPromises[i].set_value(&instance);
        });
    }

    // 等待所有线程完成
    for (auto &thread : testThreads) {
        thread.join();
    }

    // 验证所有线程获取的是同一个实例
    ThreadTestSingleton *firstInstance = testFutures[0].get();
    for (size_t i = 1; i < testFutures.size(); ++i) {
        EXPECT_EQ(firstInstance, testFutures[i].get());
    }

    // 验证计数器值正确（每个线程增加了一次）
    EXPECT_EQ(firstInstance->getCounter(), numThreads);
}

// 测试单例的继承特性
class DerivedSingleton : public Singleton<DerivedSingleton>
{
    friend class Singleton<DerivedSingleton>;

private:
    DerivedSingleton()
        : specialValue(999)
    {}

public:
    int getSpecialValue() const { return specialValue; }
    virtual std::string getType() { return "DerivedSingleton"; }

private:
    int specialValue;
};

TEST_F(SingletonTest, Inheritance)
{
    DerivedSingleton &instance = DerivedSingleton::instance();
    EXPECT_EQ(instance.getSpecialValue(), 999);
    EXPECT_EQ(instance.getType(), "DerivedSingleton");
}

// 性能测试（可选）
TEST_F(SingletonTest, Performance)
{
    auto start = std::chrono::high_resolution_clock::now(); // 修正后的代码

    constexpr int iterations = 100000;
    for (int i = 0; i < iterations; ++i) {
        TestSingleton &instance = TestSingleton::instance();
        (void) instance; // 避免未使用变量警告
    }

    auto end = std::chrono::high_resolution_clock::now(); // 修正后的代码
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    // 验证获取实例的时间在合理范围内
    // 这个阈值可能需要根据具体环境调整
    EXPECT_LT(duration.count(), 1000000); // 小于1秒
}

// 测试单例宏的正确性
TEST_F(SingletonTest, MacroUsage)
{
    // 验证使用宏的单例类具有正确的特性
    EXPECT_FALSE(std::is_default_constructible_v<AnotherSingleton>);
    EXPECT_FALSE(std::is_copy_constructible_v<AnotherSingleton>);
    EXPECT_FALSE(std::is_copy_assignable_v<AnotherSingleton>);
    EXPECT_FALSE(std::is_move_constructible_v<AnotherSingleton>);
    EXPECT_FALSE(std::is_move_assignable_v<AnotherSingleton>);

    // 验证实例方法存在且工作正常
    AnotherSingleton &instance = AnotherSingleton::instance();
    EXPECT_EQ(instance.getName(), "AnotherSingleton");
}

// 主函数
int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

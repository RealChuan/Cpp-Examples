#include "queue.hpp"

#include <gtest/gtest.h>

#include <thread>

using namespace std::chrono_literals;

class QueueTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // 每个测试前的设置
    }

    void TearDown() override
    {
        // 每个测试后的清理
    }
};

// 测试基本构造和析构
TEST_F(QueueTest, ConstructionAndDestruction)
{
    {
        Queue<int> queue;
        EXPECT_FALSE(queue.isStopped());
        EXPECT_EQ(queue.size(), 0);
        EXPECT_TRUE(queue.empty());
    }

    {
        Queue<int> queue(10);
        EXPECT_EQ(queue.getMaxSize(), 10);
    }

    {
        Queue<int> queue(0);
        EXPECT_EQ(queue.getMaxSize(), 1); // 0 应该被转换为 1
    }
}

// 测试基本的 push 和 pop
TEST_F(QueueTest, BasicPushPop)
{
    Queue<int> queue;

    // 测试 push 返回成功
    EXPECT_TRUE(queue.push(1));
    EXPECT_EQ(queue.size(), 1);
    EXPECT_FALSE(queue.empty());

    // 测试 pop 返回正确值
    auto result = queue.pop();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 1);
    EXPECT_EQ(queue.size(), 0);
    EXPECT_TRUE(queue.empty());
}

// 测试容量限制
TEST_F(QueueTest, CapacityLimit)
{
    Queue<int> queue(2); // 最大容量为 2

    EXPECT_TRUE(queue.push(1));
    EXPECT_TRUE(queue.push(2));
    EXPECT_EQ(queue.size(), 2);

    // 第三个 push 应该阻塞，但我们使用 try_push 来测试
    EXPECT_FALSE(queue.try_push(3));
    EXPECT_EQ(queue.size(), 2);
}

// 测试 try_push 和 try_pop
TEST_F(QueueTest, NonBlockingOperations)
{
    Queue<int> queue;

    // 空队列的 try_pop 应该失败
    int value;
    EXPECT_FALSE(queue.try_pop(value));
    auto result = queue.try_pop();
    EXPECT_FALSE(result.has_value());

    // try_push 应该成功
    EXPECT_TRUE(queue.try_push(10));
    EXPECT_TRUE(queue.try_push(20));

    // try_pop 应该返回第一个值
    EXPECT_TRUE(queue.try_pop(value));
    EXPECT_EQ(value, 10);

    result = queue.try_pop();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 20);
}

// 测试带超时的操作
TEST_F(QueueTest, TimeoutOperations)
{
    Queue<int> queue(1); // 容量为 1

    EXPECT_TRUE(queue.push(1));

    // push_for 应该超时
    auto start = std::chrono::steady_clock::now();
    EXPECT_FALSE(queue.push_for(2, 100ms)); // 100ms 超时
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    EXPECT_GE(duration.count(), 100); // 至少等待了 100ms

    // pop_for 应该立即返回
    start = std::chrono::steady_clock::now();
    auto result = queue.pop_for(50ms);
    end = std::chrono::steady_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(result.value(), 1);
    EXPECT_LT(duration.count(), 50); // 应该立即返回，远小于 50ms
}

// 测试停止机制
TEST_F(QueueTest, StopMechanism)
{
    Queue<int> queue;

    queue.stop();
    EXPECT_TRUE(queue.isStopped());

    // 停止后 push 应该失败
    EXPECT_FALSE(queue.push(1));
    EXPECT_FALSE(queue.try_push(2));

    // 重新启动
    queue.start();
    EXPECT_FALSE(queue.isStopped());
    EXPECT_TRUE(queue.push(3));

    auto result = queue.pop();
    EXPECT_EQ(result.value(), 3);
}

// 测试清空操作
TEST_F(QueueTest, ClearAndFlush)
{
    Queue<int> queue;

    queue.push(1);
    queue.push(2);
    queue.push(3);
    EXPECT_EQ(queue.size(), 3);

    queue.clear();
    EXPECT_EQ(queue.size(), 0);
    EXPECT_TRUE(queue.empty());

    // 测试 flush
    queue.push(4);
    queue.push(5);
    auto flushed = queue.flush();

    EXPECT_EQ(flushed.size(), 2);
    EXPECT_EQ(flushed[0], 4);
    EXPECT_EQ(flushed[1], 5);
    EXPECT_EQ(queue.size(), 0);
}

// 测试多线程生产者-消费者场景
TEST_F(QueueTest, MultiProducerMultiConsumer)
{
    Queue<int> queue(100);
    const int NUM_ITEMS = 1000;
    const int NUM_PRODUCERS = 4;
    const int NUM_CONSUMERS = 4;

    std::atomic<int> produced_count{0};
    std::atomic<int> consumed_count{0};
    std::atomic<bool> stop_flag{false};
    std::vector<std::thread> producers;
    std::vector<std::thread> consumers;

    // 启动生产者线程
    for (int i = 0; i < NUM_PRODUCERS; ++i) {
        producers.emplace_back([&queue, &produced_count, i]() {
            for (int j = 0; j < NUM_ITEMS / NUM_PRODUCERS; ++j) {
                int value = i * (NUM_ITEMS / NUM_PRODUCERS) + j;
                if (queue.push(value)) {
                    produced_count++;
                }
            }
        });
    }

    // 启动消费者线程
    for (int i = 0; i < NUM_CONSUMERS; ++i) {
        consumers.emplace_back([&queue, &consumed_count, &stop_flag]() {
            while (!stop_flag) {
                auto item = queue.pop();
                if (item.has_value()) {
                    consumed_count++;
                }
                // 短暂休眠避免过度竞争
                std::this_thread::sleep_for(1ms);
            }

            // 消费剩余的元素
            while (auto item = queue.try_pop()) {
                consumed_count++;
            }
        });
    }

    // 等待生产者完成
    for (auto &producer : producers) {
        producer.join();
    }

    // 设置停止标志
    stop_flag = true;

    // 等待消费者完成
    for (auto &consumer : consumers) {
        consumer.join();
    }

    EXPECT_EQ(produced_count, NUM_ITEMS);
    EXPECT_EQ(consumed_count, NUM_ITEMS);
    EXPECT_TRUE(queue.empty());
}

// 测试停止时的优雅关闭
TEST_F(QueueTest, GracefulShutdown)
{
    Queue<int> queue;
    std::atomic<bool> consumer_running{true};
    std::atomic<int> consumed_count{0};

    // 启动消费者线程
    std::thread consumer([&queue, &consumer_running, &consumed_count]() {
        while (consumer_running) {
            auto item = queue.pop();
            if (item.has_value()) {
                consumed_count++;
            } else {
                // 队列已停止且为空
                break;
            }
        }
    });

    // 生产一些数据
    for (int i = 0; i < 10; ++i) {
        queue.push(i);
    }

    // 给消费者一些时间处理
    std::this_thread::sleep_for(10ms);

    // 停止队列
    queue.stop();

    // 等待消费者退出
    consumer_running = false;
    consumer.join();

    EXPECT_GT(consumed_count, 0);
    EXPECT_LE(consumed_count, 10);
}

// 测试动态调整容量
TEST_F(QueueTest, DynamicCapacityChange)
{
    Queue<int> queue(2); // 初始容量 2

    EXPECT_TRUE(queue.push(1));
    EXPECT_TRUE(queue.push(2));
    EXPECT_FALSE(queue.try_push(3)); // 应该失败

    // 增加容量
    queue.setMaxSize(4);
    EXPECT_TRUE(queue.try_push(3));
    EXPECT_TRUE(queue.try_push(4));
    EXPECT_FALSE(queue.try_push(5)); // 又满了

    // 减小容量
    queue.setMaxSize(1);
    // 现在有 4 个元素，但容量设为 1，新的 push 应该失败
    EXPECT_FALSE(queue.try_push(5));

    // 但可以 pop
    auto result = queue.pop();
    EXPECT_TRUE(result.has_value());

    // 现在有 3 个元素，容量为 1，仍然不能 push
    EXPECT_FALSE(queue.try_push(5));
}

// 测试移动语义
TEST_F(QueueTest, MoveSemantics)
{
    Queue<std::unique_ptr<int>> queue;

    auto ptr = std::make_unique<int>(42);
    EXPECT_TRUE(queue.push(std::move(ptr)));
    EXPECT_EQ(ptr, nullptr); // 应该已经被移动

    auto result = queue.pop();
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(*(result.value()), 42);
}

// 性能测试：高并发场景
TEST_F(QueueTest, PerformanceHighConcurrency)
{
    const int NUM_OPERATIONS = 10000;
    Queue<int> queue;
    std::atomic<int> counter{0};

    auto start = std::chrono::steady_clock::now();

    std::thread producer([&queue, &counter]() {
        for (int i = 0; i < NUM_OPERATIONS; ++i) {
            if (queue.push(i)) {
                counter++;
            }
        }
    });

    std::thread consumer([&queue, &counter]() {
        for (int i = 0; i < NUM_OPERATIONS; ++i) {
            auto item = queue.pop();
            if (item.has_value()) {
                counter--;
            }
        }
    });

    producer.join();
    consumer.join();

    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    EXPECT_EQ(counter, 0);
    EXPECT_TRUE(queue.empty());

    // 输出性能信息（可选）
    std::cout << "Performance: " << NUM_OPERATIONS << " operations took " << duration.count()
              << "ms" << std::endl;
}

// 测试异常安全性
TEST_F(QueueTest, ExceptionSafety)
{
    struct ThrowingType
    {
        int value;
        bool throw_on_copy;

        ThrowingType(int v, bool throw_copy = false)
            : value(v)
            , throw_on_copy(throw_copy)
        {}

        ThrowingType(const ThrowingType &other)
            : value(other.value)
            , throw_on_copy(other.throw_on_copy)
        {
            if (throw_on_copy) {
                throw std::runtime_error("Copy failed");
            }
        }

        ThrowingType(ThrowingType &&other) noexcept
            : value(other.value)
            , throw_on_copy(other.throw_on_copy)
        {}

        ThrowingType &operator=(const ThrowingType &) = delete;
        ThrowingType &operator=(ThrowingType &&) = delete;
    };

    Queue<ThrowingType> queue;

    // 测试正常情况
    EXPECT_TRUE(queue.push(ThrowingType(1)));

    // 测试移动构造不会抛出
    EXPECT_TRUE(queue.push(ThrowingType(2, true)));

    // pop 应该成功
    auto result = queue.pop();
    EXPECT_TRUE(result.has_value());
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

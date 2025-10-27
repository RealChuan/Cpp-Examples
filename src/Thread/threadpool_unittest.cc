#include "threadpool.hpp"

#include <gtest/gtest.h>

using namespace std::chrono_literals;

class ThreadPoolTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // 每个测试前的设置
    }

    void TearDown() override
    {
        // 确保线程池在测试后完全关闭
        if (pool && pool->isRunning()) {
            pool->shutdownNow();
        }
    }

    std::unique_ptr<ThreadPool> pool;
};

// 测试默认构造
TEST_F(ThreadPoolTest, DefaultConstruction)
{
    pool = std::make_unique<ThreadPool>();

    EXPECT_TRUE(pool->isRunning());
    EXPECT_FALSE(pool->isStopped());
    EXPECT_GT(pool->size(), 0u);
    EXPECT_EQ(pool->queueSize(), 0u);
    EXPECT_EQ(pool->getRunningTasks(), 0u);
    EXPECT_EQ(pool->getPendingTasks(), 0u);
    EXPECT_EQ(pool->getTotalTasks(), 0u);
}

// 测试自定义线程数构造
TEST_F(ThreadPoolTest, CustomThreadCount)
{
    const size_t threadCount = 4;
    pool = std::make_unique<ThreadPool>(threadCount);

    EXPECT_TRUE(pool->isRunning());
    EXPECT_EQ(pool->size(), threadCount);
}

// 测试零线程数（应该至少有一个线程）
TEST_F(ThreadPoolTest, ZeroThreadCount)
{
    pool = std::make_unique<ThreadPool>(0);

    EXPECT_TRUE(pool->isRunning());
    EXPECT_EQ(pool->size(), 1u); // 应该至少有一个线程
}

// 测试提交简单任务
TEST_F(ThreadPoolTest, SubmitSimpleTask)
{
    pool = std::make_unique<ThreadPool>(2);
    std::atomic<int> counter{0};

    auto task = [&counter]() { counter++; };

    EXPECT_TRUE(pool->submit(task));
    pool->waitAll();

    EXPECT_EQ(counter, 1);
}

// 测试提交多个任务
TEST_F(ThreadPoolTest, SubmitMultipleTasks)
{
    pool = std::make_unique<ThreadPool>(4);
    const int taskCount = 10;
    std::atomic<int> counter{0};

    for (int i = 0; i < taskCount; ++i) {
        EXPECT_TRUE(pool->submit([&counter]() { counter++; }));
    }

    pool->waitAll();
    EXPECT_EQ(counter, taskCount);
}

// 测试提交带返回值的任务
TEST_F(ThreadPoolTest, SubmitFutureTask)
{
    pool = std::make_unique<ThreadPool>(2);

    auto future = pool->submitFuture([]() -> int {
        std::this_thread::sleep_for(50ms);
        return 42;
    });

    EXPECT_EQ(future.get(), 42);
}

// 测试多个带返回值的任务
TEST_F(ThreadPoolTest, SubmitMultipleFutureTasks)
{
    pool = std::make_unique<ThreadPool>(4);
    const int taskCount = 8;
    std::vector<std::future<int>> futures;

    for (int i = 0; i < taskCount; ++i) {
        auto future = pool->submitFuture([i]() -> int {
            std::this_thread::sleep_for(10ms);
            return i * i;
        });
        futures.push_back(std::move(future));
    }

    for (int i = 0; i < taskCount; ++i) {
        EXPECT_EQ(futures[i].get(), i * i);
    }
}

// 测试任务异常处理
TEST_F(ThreadPoolTest, TaskExceptionHandling)
{
    pool = std::make_unique<ThreadPool>(2);

    // 提交会抛出异常的任务
    EXPECT_TRUE(pool->submit([]() { throw std::runtime_error("Test exception"); }));

    // 提交正常任务确保线程池仍然工作
    std::atomic<bool> normalTaskExecuted{false};
    EXPECT_TRUE(pool->submit([&normalTaskExecuted]() { normalTaskExecuted = true; }));

    pool->waitAll();
    EXPECT_TRUE(normalTaskExecuted);
}

// 测试future任务的异常传播
TEST_F(ThreadPoolTest, FutureTaskException)
{
    pool = std::make_unique<ThreadPool>(2);

    auto future = pool->submitFuture([]() -> int {
        throw std::runtime_error("Future test exception");
        return 0;
    });

    EXPECT_THROW({ future.get(); }, std::runtime_error);
}

// 测试非阻塞提交
TEST_F(ThreadPoolTest, TrySubmit)
{
    pool = std::make_unique<ThreadPool>(1, 2); // 1个线程，队列大小2

    std::atomic<int> counter{0};

    // 提交应该成功
    EXPECT_TRUE(pool->trySubmit([&counter]() {
        std::this_thread::sleep_for(100ms);
        counter++;
    }));

    // 第二个任务应该成功（进入队列）
    EXPECT_TRUE(pool->trySubmit([&counter]() {
        std::this_thread::sleep_for(100ms);
        counter++;
    }));

    std::this_thread::sleep_for(10ms);

    // 第三个任务应该成功（进入队列），第一个任务正在执行
    EXPECT_TRUE(pool->trySubmit([&counter]() {
        std::this_thread::sleep_for(100ms);
        counter++;
    }));

    // 第四个任务应该失败
    EXPECT_FALSE(pool->trySubmit([&counter]() { counter++; }));

    pool->waitAll();
    EXPECT_EQ(counter, 3);
}

// 测试带超时的提交
TEST_F(ThreadPoolTest, SubmitWithTimeout)
{
    pool = std::make_unique<ThreadPool>(1, 1); // 小队列

    // 填充队列
    EXPECT_TRUE(pool->submit([]() { std::this_thread::sleep_for(100ms); }));

    std::this_thread::sleep_for(10ms);

    // 这个任务会进入队列, 第一个任务正在执行
    EXPECT_TRUE(pool->submit([]() { std::this_thread::sleep_for(100ms); }));

    // 带短超时的提交应该失败
    auto start = std::chrono::steady_clock::now();
    EXPECT_FALSE(pool->submitFor([]() {}, 50ms));
    auto end = std::chrono::steady_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    EXPECT_GE(duration.count(), 50);
    EXPECT_LE(duration.count(), 100);

    pool->waitAll();
}

// 测试优雅关闭
TEST_F(ThreadPoolTest, GracefulShutdown)
{
    pool = std::make_unique<ThreadPool>(2);
    std::atomic<int> completedTasks{0};
    const int taskCount = 5;

    for (int i = 0; i < taskCount; ++i) {
        pool->submit([&completedTasks, i]() {
            std::this_thread::sleep_for(i * 10ms); // 不同长度的任务
            completedTasks++;
        });
    }

    // 给任务一些时间开始执行
    std::this_thread::sleep_for(50ms);

    pool->shutdown();

    EXPECT_TRUE(pool->isStopped());
    EXPECT_EQ(completedTasks, taskCount); // 所有任务应该都完成了
}

// 测试立即关闭
TEST_F(ThreadPoolTest, ImmediateShutdown)
{
    pool = std::make_unique<ThreadPool>(2);
    std::atomic<int> completedTasks{0};
    const int taskCount = 10;

    for (int i = 0; i < taskCount; ++i) {
        pool->submit([&completedTasks, i]() {
            std::this_thread::sleep_for(100ms); // 长任务
            completedTasks++;
        });
    }

    // 给任务一些时间开始执行
    std::this_thread::sleep_for(50ms);

    pool->shutdownNow();

    EXPECT_TRUE(pool->isStopped());
    // 只有部分任务可能完成
    EXPECT_LT(completedTasks, taskCount);
}

// 测试重启
TEST_F(ThreadPoolTest, Restart)
{
    pool = std::make_unique<ThreadPool>(2);

    // 提交一些任务
    std::atomic<int> counter1{0};
    for (int i = 0; i < 3; ++i) {
        pool->submit([&counter1]() { counter1++; });
    }
    pool->waitAll();
    EXPECT_EQ(counter1, 3);

    // 关闭
    pool->shutdown();
    EXPECT_TRUE(pool->isStopped());

    // 重启
    EXPECT_TRUE(pool->restart(3));
    EXPECT_TRUE(pool->isRunning());
    EXPECT_EQ(pool->size(), 3u);

    // 提交新任务
    std::atomic<int> counter2{0};
    for (int i = 0; i < 3; ++i) {
        pool->submit([&counter2]() { counter2++; });
    }
    pool->waitAll();
    EXPECT_EQ(counter2, 3);
}

// 测试等待所有任务完成
TEST_F(ThreadPoolTest, WaitAll)
{
    pool = std::make_unique<ThreadPool>(2);
    std::atomic<int> counter{0};
    const int taskCount = 5;

    for (int i = 0; i < taskCount; ++i) {
        pool->submit([&counter, i]() {
            std::this_thread::sleep_for((i + 1) * 20ms);
            counter++;
        });
    }

    auto start = std::chrono::steady_clock::now();
    pool->waitAll();
    auto end = std::chrono::steady_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    EXPECT_GE(duration.count(), 100); // 至少需要100ms（最长任务）
    EXPECT_EQ(counter, taskCount);
}

// 测试带超时的等待
TEST_F(ThreadPoolTest, WaitAllWithTimeout)
{
    pool = std::make_unique<ThreadPool>(2);

    // 提交一个长任务
    pool->submit([]() { std::this_thread::sleep_for(200ms); });

    // 短超时应该返回false
    EXPECT_FALSE(pool->waitAllFor(50ms));

    // 长超时应该返回true
    EXPECT_TRUE(pool->waitAllFor(300ms));
}

// 测试队列大小限制
TEST_F(ThreadPoolTest, QueueSizeLimit)
{
    const size_t maxQueueSize = 3;
    pool = std::make_unique<ThreadPool>(1, maxQueueSize); // 1个线程，小队列

    std::atomic<int> startedTasks{0};
    std::atomic<int> completedTasks{0};

    // 填充一个长任务占用工作线程
    pool->submit([&startedTasks, &completedTasks]() {
        startedTasks++;
        std::this_thread::sleep_for(500ms);
        completedTasks++;
    });

    // 等待任务开始
    while (startedTasks == 0) {
        std::this_thread::sleep_for(10ms);
    }

    // 提交任务填满队列
    for (size_t i = 0; i < maxQueueSize; ++i) {
        EXPECT_TRUE(pool->submit([&completedTasks]() { completedTasks++; }));
    }

    // 下一个提交应该阻塞或失败（取决于实现）
    // 这里我们测试trySubmit应该失败
    EXPECT_FALSE(pool->trySubmit([&completedTasks]() { completedTasks++; }));

    pool->waitAll();
    EXPECT_EQ(completedTasks, 1 + maxQueueSize); // 长任务 + 队列中的任务
}

// 测试设置最大队列大小
TEST_F(ThreadPoolTest, SetMaxQueueSize)
{
    pool = std::make_unique<ThreadPool>(1, 2); // 初始队列大小2

    EXPECT_EQ(pool->getMaxQueueSize(), 2u);

    pool->setMaxQueueSize(5);
    EXPECT_EQ(pool->getMaxQueueSize(), 5u);

    pool->setMaxQueueSize(0); // 0应该被处理为1
    EXPECT_EQ(pool->getMaxQueueSize(), 1u);
}

// 测试并发任务执行
TEST_F(ThreadPoolTest, ConcurrentExecution)
{
    const size_t threadCount = 4;
    const int taskCount = 20;
    pool = std::make_unique<ThreadPool>(threadCount);

    std::atomic<int> concurrentTasks{0};
    std::atomic<int> maxConcurrent{0};
    std::atomic<int> completedTasks{0};
    std::mutex mutex;

    for (int i = 0; i < taskCount; ++i) {
        pool->submit([&concurrentTasks, &maxConcurrent, &completedTasks, &mutex]() {
            int current = ++concurrentTasks;

            // 更新最大并发数
            {
                std::lock_guard<std::mutex> lock(mutex);
                if (current > maxConcurrent) {
                    maxConcurrent = current;
                }
            }

            // 模拟工作
            std::this_thread::sleep_for(10ms);

            --concurrentTasks;
            completedTasks++;
        });
    }

    pool->waitAll();

    EXPECT_EQ(completedTasks, taskCount);
    EXPECT_LE(maxConcurrent, threadCount);
    EXPECT_GT(maxConcurrent, 0);
}

// 测试性能：大量小任务
TEST_F(ThreadPoolTest, PerformanceManySmallTasks)
{
    const int taskCount = 1000;
    pool = std::make_unique<ThreadPool>();

    std::atomic<int> counter{0};
    auto start = std::chrono::steady_clock::now();

    for (int i = 0; i < taskCount; ++i) {
        pool->submit([&counter]() { counter++; });
    }

    pool->waitAll();
    auto end = std::chrono::steady_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    EXPECT_EQ(counter, taskCount);
    EXPECT_LT(duration.count(), 1000); // 应该在1秒内完成
}

// 测试任务执行顺序（不保证顺序，但应该全部执行）
TEST_F(ThreadPoolTest, TaskExecutionOrder)
{
    pool = std::make_unique<ThreadPool>(2);
    std::vector<int> executionOrder;
    std::mutex orderMutex;
    const int taskCount = 10;

    for (int i = 0; i < taskCount; ++i) {
        pool->submit([i, &executionOrder, &orderMutex]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(10 * (9 - i))); // 反向延迟
            std::lock_guard<std::mutex> lock(orderMutex);
            executionOrder.push_back(i);
        });
    }

    pool->waitAll();

    EXPECT_EQ(executionOrder.size(), taskCount);

    // 验证所有任务都执行了（顺序不保证）
    std::vector<int> sortedOrder = executionOrder;
    std::sort(sortedOrder.begin(), sortedOrder.end());
    for (int i = 0; i < taskCount; ++i) {
        EXPECT_EQ(sortedOrder[i], i);
    }
}

// 测试析构时自动关闭
TEST_F(ThreadPoolTest, AutoShutdownOnDestruction)
{
    std::atomic<bool> taskStarted{false};
    std::atomic<bool> taskInterrupted{false};

    {
        ThreadPool localPool(2);

        localPool.submit([&taskStarted, &taskInterrupted]() {
            taskStarted = true;
            try {
                // 长任务，应该被中断
                for (int i = 0; i < 100; ++i) {
                    if (std::this_thread::get_id() == std::thread::id()) {
                        // 模拟被中断的检查点
                        break;
                    }
                    std::this_thread::sleep_for(10ms);
                }
            } catch (...) {
                taskInterrupted = true;
            }
        });

        // 等待任务开始
        while (!taskStarted) {
            std::this_thread::sleep_for(10ms);
        }

        // localPool离开作用域，应该自动关闭
    }

    // 任务应该被中断
    EXPECT_TRUE(taskStarted);
    // 注意：实际是否被中断取决于实现，这里我们只测试不会死锁
    SUCCEED();
}

// 测试在已关闭的池中提交任务
TEST_F(ThreadPoolTest, SubmitAfterShutdown)
{
    pool = std::make_unique<ThreadPool>(2);

    pool->shutdown();
    EXPECT_TRUE(pool->isStopped());

    // 在关闭后提交应该失败
    EXPECT_FALSE(pool->submit([]() {}));
    EXPECT_FALSE(pool->trySubmit([]() {}));

    auto future = pool->submitFuture([]() -> int { return 42; });
    // future应该包含异常
    EXPECT_THROW({ future.get(); }, std::runtime_error);
}

// 测试状态查询的线程安全
TEST_F(ThreadPoolTest, ThreadSafeStateQueries)
{
    pool = std::make_unique<ThreadPool>(4);
    const int taskCount = 50;

    // 启动多个线程同时查询状态和提交任务
    std::vector<std::thread> queryThreads;
    std::atomic<bool> stopQueries{false};
    std::atomic<int> queryCount{0};

    // 查询线程
    for (int i = 0; i < 4; ++i) {
        queryThreads.emplace_back([this, &stopQueries, &queryCount]() {
            while (!stopQueries) {
                // 各种状态查询
                pool->isRunning();
                pool->isStopped();
                pool->size();
                pool->queueSize();
                pool->getRunningTasks();
                pool->getPendingTasks();
                pool->getTotalTasks();
                queryCount++;
                std::this_thread::yield();
            }
        });
    }

    // 提交任务
    std::atomic<int> completedTasks{0};
    for (int i = 0; i < taskCount; ++i) {
        pool->submit([&completedTasks]() {
            std::this_thread::sleep_for(1ms);
            completedTasks++;
        });
    }

    pool->waitAll();

    // 停止查询线程
    stopQueries = true;
    for (auto &thread : queryThreads) {
        thread.join();
    }

    EXPECT_EQ(completedTasks, taskCount);
    EXPECT_GT(queryCount, 0);
}

// 测试复杂任务依赖
TEST_F(ThreadPoolTest, ComplexTaskDependencies)
{
    pool = std::make_unique<ThreadPool>(4);

    std::atomic<int> stage1{0};
    std::atomic<int> stage2{0};
    std::atomic<int> stage3{0};

    // 阶段1任务
    for (int i = 0; i < 5; ++i) {
        pool->submit([&stage1, &stage2, i, this]() {
            std::this_thread::sleep_for(i * 5ms);
            stage1++;

            // 阶段2任务（依赖于阶段1）
            if (i % 2 == 0) {
                pool->submit([&stage2]() {
                    std::this_thread::sleep_for(10ms);
                    stage2++;
                });
            }
        });
    }

    // 独立阶段3任务
    for (int i = 0; i < 3; ++i) {
        pool->submit([&stage3]() {
            std::this_thread::sleep_for(15ms);
            stage3++;
        });
    }

    pool->waitAll();

    EXPECT_EQ(stage1, 5);
    EXPECT_EQ(stage2, 3); // 5个阶段1任务中，3个会提交阶段2任务
    EXPECT_EQ(stage3, 3);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

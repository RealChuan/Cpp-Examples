#include "thread.hpp"

#include <gtest/gtest.h>

#include <future>

using namespace std::chrono_literals;

class ThreadTest : public ::testing::Test
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

// 测试默认构造
TEST_F(ThreadTest, DefaultConstruction)
{
    Thread thread;
    EXPECT_FALSE(thread.isRunning());
    EXPECT_TRUE(thread.isIdle());
    EXPECT_FALSE(thread.isJoinable());
    EXPECT_EQ(thread.getState(), Thread::State::Idle);
}

// 测试带任务构造
TEST_F(ThreadTest, ConstructionWithTask)
{
    bool taskExecuted = false;
    Thread thread([&taskExecuted](std::stop_token) { taskExecuted = true; });

    EXPECT_FALSE(thread.isRunning());
    EXPECT_TRUE(thread.isIdle());
}

// 测试启动和停止
TEST_F(ThreadTest, StartAndStop)
{
    std::atomic<bool> taskExecuted{false};

    Thread thread([&taskExecuted](std::stop_token token) {
        while (!token.stop_requested()) {
            std::this_thread::sleep_for(10ms);
        }
        taskExecuted = true;
    });

    // 启动线程
    EXPECT_TRUE(thread.start());
    EXPECT_TRUE(thread.isRunning());
    EXPECT_TRUE(thread.isJoinable());

    // 等待线程真正运行
    std::this_thread::sleep_for(50ms);

    // 停止线程
    thread.stop();
    EXPECT_FALSE(thread.isRunning());
    EXPECT_TRUE(taskExecuted);
}

// 测试重复启动
TEST_F(ThreadTest, DoubleStart)
{
    Thread thread([](std::stop_token) { std::this_thread::sleep_for(100ms); });

    EXPECT_TRUE(thread.start());
    EXPECT_FALSE(thread.start()); // 第二次启动应该失败

    thread.waitForFinished();
}

// 测试无任务启动
TEST_F(ThreadTest, StartWithoutTask)
{
    Thread thread;
    EXPECT_FALSE(thread.start()); // 没有任务应该启动失败
}

// 测试运行时设置任务
TEST_F(ThreadTest, SetTaskWhileRunning)
{
    Thread thread([](std::stop_token) { std::this_thread::sleep_for(100ms); });

    EXPECT_TRUE(thread.start());

    // 尝试在运行时设置任务应该抛出异常
    EXPECT_THROW({ thread.setTask([](std::stop_token) {}); }, std::runtime_error);

    thread.stop();
}

// 测试等待完成
TEST_F(ThreadTest, WaitForFinished)
{
    std::atomic<bool> taskCompleted{false};

    Thread thread([&taskCompleted](std::stop_token) {
        std::this_thread::sleep_for(100ms);
        taskCompleted = true;
    });

    auto start = std::chrono::steady_clock::now();
    thread.start();
    thread.waitForFinished();
    auto end = std::chrono::steady_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    EXPECT_GE(duration.count(), 100);
    EXPECT_TRUE(taskCompleted);
}

// 测试带超时的等待完成
TEST_F(ThreadTest, WaitForFinishedWithTimeout)
{
    Thread thread([](std::stop_token) { std::this_thread::sleep_for(500ms); });

    thread.start();

    // 短超时应该返回false
    EXPECT_FALSE(thread.waitForFinished(100ms));

    // 长超时应该返回true
    EXPECT_TRUE(thread.waitForFinished(600ms));
}

// 测试请求停止
TEST_F(ThreadTest, RequestStop)
{
    std::atomic<bool> stopRequested{false};

    Thread thread([&stopRequested](std::stop_token token) {
        while (!token.stop_requested()) {
            std::this_thread::sleep_for(10ms);
        }
        stopRequested = true;
    });

    thread.start();
    std::this_thread::sleep_for(50ms);

    thread.requestStop();
    thread.waitForFinished(100ms);

    EXPECT_TRUE(stopRequested);
}

// 测试线程ID
TEST_F(ThreadTest, ThreadId)
{
    std::thread::id threadId;

    Thread thread([&threadId](std::stop_token) { threadId = std::this_thread::get_id(); });

    thread.start();
    thread.waitForFinished();

    EXPECT_NE(threadId, std::thread::id());
    EXPECT_EQ(threadId, thread.getThreadId());
}

// 测试停止令牌
TEST_F(ThreadTest, StopToken)
{
    std::atomic<bool> hasStopToken{false};

    Thread thread([&hasStopToken](std::stop_token token) {
        hasStopToken = token.stop_possible();
        std::this_thread::sleep_for(50ms);
    });

    thread.start();
    thread.waitForFinished();

    EXPECT_TRUE(hasStopToken);
}

// 测试异常处理
TEST_F(ThreadTest, ExceptionHandling)
{
    // 这个测试主要确保异常不会导致崩溃
    Thread thread([](std::stop_token) { throw std::runtime_error("Test exception"); });

    EXPECT_TRUE(thread.start());
    thread.waitForFinished(); // 不应该抛出异常

    EXPECT_FALSE(thread.isRunning());
}

// 测试状态转换
TEST_F(ThreadTest, StateTransitions)
{
    std::promise<void> startPromise;
    std::future<void> startFuture = startPromise.get_future();
    std::promise<void> endPromise;
    std::future<void> endFuture = endPromise.get_future();

    Thread thread([&startPromise, &endFuture](std::stop_token token) {
        startPromise.set_value(); // 通知测试线程已启动
        endFuture.get();          // 等待测试允许结束
    });

    // 初始状态
    EXPECT_EQ(thread.getState(), Thread::State::Idle);

    // 启动线程
    thread.start();

    // 等待线程内部设置promise
    startFuture.get();

    // 线程应该处于运行状态
    EXPECT_EQ(thread.getState(), Thread::State::Running);

    // 允许线程结束
    endPromise.set_value();
    thread.waitForFinished();

    // 线程应该处于停止状态
    EXPECT_EQ(thread.getState(), Thread::State::Stopped);
}

// 测试静态睡眠方法
TEST_F(ThreadTest, StaticSleepMethods)
{
    auto start = std::chrono::steady_clock::now();

    Thread::sleep(1);       // 睡眠1秒
    Thread::msleep(50);     // 睡眠50毫秒
    Thread::usleep(1000);   // 睡眠1毫秒
    Thread::sleepFor(10ms); // 睡眠10毫秒

    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // 总睡眠时间应该超过1秒
    EXPECT_GT(duration.count(), 1000);
}

// 测试静态yield方法
TEST_F(ThreadTest, StaticYield)
{
    // yield方法不应该阻塞，我们只能测试它能够正常调用
    Thread::yield();
    SUCCEED(); // 如果没有崩溃就是成功
}

// 测试硬件并发
TEST_F(ThreadTest, HardwareConcurrency)
{
    unsigned int cores = Thread::hardwareConcurrency();
    EXPECT_GT(cores, 0u);
    EXPECT_LE(cores, std::thread::hardware_concurrency());
}

// 测试析构时自动停止
TEST_F(ThreadTest, AutoStopOnDestruction)
{
    std::atomic<bool> threadRunning{false};
    std::atomic<bool> threadStopped{false};

    {
        Thread thread([&threadRunning, &threadStopped](std::stop_token token) {
            threadRunning = true;
            while (!token.stop_requested()) {
                std::this_thread::sleep_for(10ms);
            }
            threadStopped = true;
        });

        thread.start();

        // 等待线程启动
        while (!threadRunning) {
            std::this_thread::sleep_for(10ms);
        }

        // thread对象离开作用域，应该自动停止
    }

    // 给线程一些时间来处理停止
    std::this_thread::sleep_for(50ms);

    EXPECT_TRUE(threadStopped);
}

// 测试带参数的停止方法
TEST_F(ThreadTest, StopWithTimeout)
{
    Thread thread([](std::stop_token) { std::this_thread::sleep_for(200ms); });

    thread.start();

    auto start = std::chrono::steady_clock::now();
    thread.stop(100ms); // 短超时，可能不会完全等待
    auto end = std::chrono::steady_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    EXPECT_LE(duration.count(), 150); // 应该在大约100ms左右返回
}

// 测试任务返回值（通过future）
TEST_F(ThreadTest, TaskWithReturnValue)
{
    auto task = [](std::stop_token) -> int {
        std::this_thread::sleep_for(50ms);
        return 42;
    };

    // 注意：Thread类本身不支持返回值，这里测试的是lambda的调用
    Thread thread([task](std::stop_token token) {
        int result = task(token);
        EXPECT_EQ(result, 42);
    });

    thread.start();
    thread.waitForFinished();
}

// 测试多次停止调用
TEST_F(ThreadTest, MultipleStopCalls)
{
    Thread thread([](std::stop_token token) {
        while (!token.stop_requested()) {
            std::this_thread::sleep_for(10ms);
        }
    });

    thread.start();
    std::this_thread::sleep_for(50ms);

    // 多次调用stop应该是安全的
    thread.stop();
    thread.stop();
    thread.stop();

    EXPECT_FALSE(thread.isRunning());
}

// 测试在已停止的线程上操作
TEST_F(ThreadTest, OperationsOnStoppedThread)
{
    Thread thread([](std::stop_token) {
        // 快速结束的任务
    });

    thread.start();
    thread.waitForFinished();

    // 在已停止的线程上操作
    EXPECT_FALSE(thread.isRunning());
    EXPECT_TRUE(thread.isStopped());
    EXPECT_TRUE(thread.waitForFinished());     // 应该立即返回true
    EXPECT_TRUE(thread.waitForFinished(10ms)); // 应该立即返回true

    // 再次停止应该是安全的
    thread.stop();
    thread.requestStop();
}

// 性能测试：快速启动停止多个线程
TEST_F(ThreadTest, RapidStartStop)
{
    const int NUM_THREADS = 10;
    std::vector<std::unique_ptr<Thread>> threads;

    for (int i = 0; i < NUM_THREADS; ++i) {
        auto thread = std::make_unique<Thread>(
            [](std::stop_token token) { std::this_thread::sleep_for(10ms); });

        EXPECT_TRUE(thread->start());
        threads.push_back(std::move(thread));
    }

    // 等待所有线程完成
    for (auto &thread : threads) {
        thread->waitForFinished();
        EXPECT_FALSE(thread->isRunning());
    }
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

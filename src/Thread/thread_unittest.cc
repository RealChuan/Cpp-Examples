#include "thread.hpp"

#include <gtest/gtest.h>

#include <atomic>
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
    EXPECT_FALSE(taskExecuted); // 任务不应该立即执行
}

// 测试启动和停止
TEST_F(ThreadTest, StartAndStop)
{
    std::atomic<bool> taskExecuted{false};
    std::atomic<int> loopCount{0};

    Thread thread([&taskExecuted, &loopCount](std::stop_token token) {
        while (!token.stop_requested()) {
            ++loopCount;
            std::this_thread::sleep_for(5ms);
        }
        taskExecuted = true;
    });

    // 启动线程
    EXPECT_TRUE(thread.start());
    EXPECT_TRUE(thread.isRunning());
    EXPECT_TRUE(thread.isJoinable());

    // 等待线程运行一段时间
    std::this_thread::sleep_for(50ms);
    EXPECT_GT(loopCount.load(), 0);

    // 停止线程
    thread.stop();
    EXPECT_FALSE(thread.isRunning());
    EXPECT_TRUE(taskExecuted);
}

// 测试重复启动
TEST_F(ThreadTest, DoubleStart)
{
    std::atomic<int> executionCount{0};
    Thread thread([&executionCount](std::stop_token) {
        ++executionCount;
        std::this_thread::sleep_for(100ms);
    });

    EXPECT_TRUE(thread.start());
    EXPECT_FALSE(thread.start()); // 第二次启动应该失败

    // 确保只执行了一次
    thread.waitForFinished();
    EXPECT_EQ(executionCount.load(), 1);
}

// 测试无任务启动
TEST_F(ThreadTest, StartWithoutTask)
{
    Thread thread;
    EXPECT_FALSE(thread.start()); // 没有任务应该启动失败

    // 设置任务后应该能启动
    thread.setTask([](std::stop_token) {});
    EXPECT_TRUE(thread.start());
    thread.stop();
}

// 测试运行时设置任务
TEST_F(ThreadTest, SetTaskWhileRunning)
{
    Thread thread([](std::stop_token) { std::this_thread::sleep_for(100ms); });

    EXPECT_TRUE(thread.start());

    // 尝试在运行时设置任务应该抛出异常
    EXPECT_THROW(thread.setTask([](std::stop_token) {}), std::runtime_error);

    thread.stop();
}

// 测试在停止状态设置任务
TEST_F(ThreadTest, SetTaskAfterStopped)
{
    Thread thread([](std::stop_token) {});

    // 启动并等待完成
    thread.start();
    thread.waitForFinished();

    // 停止后应该可以重新设置任务
    EXPECT_NO_THROW(thread.setTask([](std::stop_token) {}));
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
    auto start = std::chrono::steady_clock::now();
    EXPECT_FALSE(thread.waitForFinished(50ms));
    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    EXPECT_LE(duration.count(), 100); // 应该在超时后很快返回

    // 长超时应该返回true
    EXPECT_TRUE(thread.waitForFinished(600ms));
}

// 测试零超时等待
TEST_F(ThreadTest, WaitForFinishedWithZeroTimeout)
{
    Thread thread([](std::stop_token) { std::this_thread::sleep_for(100ms); });

    thread.start();

    // 零超时应该立即返回false
    EXPECT_FALSE(thread.waitForFinished(0ms));

    thread.waitForFinished(); // 正常等待完成
}

// 测试请求停止
TEST_F(ThreadTest, RequestStop)
{
    std::atomic<bool> stopRequested{false};
    std::atomic<int> iterations{0};

    Thread thread([&stopRequested, &iterations](std::stop_token token) {
        while (!token.stop_requested()) {
            ++iterations;
            std::this_thread::sleep_for(5ms);
        }
        stopRequested = true;
    });

    thread.start();

    // 确保线程运行了一些迭代
    std::this_thread::sleep_for(50ms);
    EXPECT_GT(iterations.load(), 0);

    thread.requestStop();
    bool finished = thread.waitForFinished(100ms);

    EXPECT_TRUE(stopRequested);
    EXPECT_TRUE(finished);
}

// 测试线程ID
TEST_F(ThreadTest, ThreadId)
{
    std::thread::id threadId;
    std::promise<void> started;

    Thread thread([&threadId, &started](std::stop_token) {
        threadId = std::this_thread::get_id();
        started.set_value();
        std::this_thread::sleep_for(50ms);
    });

    EXPECT_EQ(thread.getThreadId(), std::thread::id()); // 启动前应该为空

    thread.start();
    started.get_future().wait(); // 等待线程真正启动

    EXPECT_NE(threadId, std::thread::id());
    EXPECT_EQ(threadId, thread.getThreadId());

    thread.waitForFinished();
}

// 测试停止令牌
TEST_F(ThreadTest, StopToken)
{
    std::atomic<bool> hasStopToken{false};
    std::atomic<bool> canStop{false};

    Thread thread([&hasStopToken, &canStop](std::stop_token token) {
        hasStopToken = token.stop_possible();
        while (!token.stop_requested()) {
            std::this_thread::sleep_for(5ms);
        }
        canStop = true;
    });

    thread.start();

    // 等待线程设置hasStopToken
    std::this_thread::sleep_for(50ms);
    EXPECT_TRUE(hasStopToken);

    // 测试停止令牌功能
    thread.requestStop();
    thread.waitForFinished(100ms);

    EXPECT_TRUE(canStop);
}

// 测试异常处理
TEST_F(ThreadTest, ExceptionHandling)
{
    std::atomic<bool> exceptionThrown{false};

    // 这个测试主要确保异常不会导致崩溃
    Thread thread([&exceptionThrown](std::stop_token) {
        try {
            throw std::runtime_error("Test exception");
        } catch (...) {
            exceptionThrown = true;
            throw; // 重新抛出，测试线程类的异常处理
        }
    });

    EXPECT_TRUE(thread.start());
    EXPECT_NO_THROW(thread.waitForFinished()); // 不应该抛出异常

    EXPECT_FALSE(thread.isRunning());
    EXPECT_TRUE(exceptionThrown);
}

// 测试多个异常
TEST_F(ThreadTest, MultipleExceptions)
{
    // 测试多次启动停止，确保异常处理稳定
    for (int i = 0; i < 3; ++i) {
        Thread thread([](std::stop_token) { throw std::logic_error("Iteration exception"); });

        EXPECT_TRUE(thread.start());
        EXPECT_NO_THROW(thread.waitForFinished());
        EXPECT_TRUE(thread.isStopped());
    }
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
    EXPECT_EQ(thread.getState(), Thread::State::Starting);

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

// 测试快速状态转换
TEST_F(ThreadTest, RapidStateTransitions)
{
    Thread thread([](std::stop_token) {
        // 快速完成的任务
    });

    // 快速连续调用状态检查
    EXPECT_TRUE(thread.isIdle());
    EXPECT_FALSE(thread.isRunning());
    EXPECT_FALSE(thread.isStopped());

    thread.start();

    // 可能的状态：Starting 或 Running
    EXPECT_TRUE(thread.isRunning() || thread.getState() == Thread::State::Starting);

    thread.waitForFinished();

    EXPECT_TRUE(thread.isStopped());
    EXPECT_FALSE(thread.isRunning());
    EXPECT_FALSE(thread.isIdle());
}

// 测试静态睡眠方法
TEST_F(ThreadTest, StaticSleepMethods)
{
    auto start = std::chrono::steady_clock::now();

    Thread::sleep(0);      // 边界测试：零秒睡眠
    Thread::msleep(10);    // 短时间睡眠
    Thread::usleep(100);   // 微秒级睡眠
    Thread::sleepFor(5ms); // 毫秒级睡眠

    auto end = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    // 总睡眠时间应该合理
    EXPECT_GE(duration.count(), 15);
}

// 测试静态yield方法
TEST_F(ThreadTest, StaticYield)
{
    // yield方法不应该阻塞，我们只能测试它能够正常调用
    for (int i = 0; i < 10; ++i) {
        Thread::yield();
    }
    SUCCEED(); // 如果没有崩溃就是成功
}

// 测试硬件并发
TEST_F(ThreadTest, HardwareConcurrency)
{
    unsigned int cores = Thread::hardwareConcurrency();
    EXPECT_GT(cores, 0u);
    EXPECT_LE(cores, std::thread::hardware_concurrency());

    // 多次调用应该返回相同结果
    EXPECT_EQ(cores, Thread::hardwareConcurrency());
}

// 测试析构时自动停止
TEST_F(ThreadTest, AutoStopOnDestruction)
{
    std::atomic<bool> threadRunning{false};
    std::atomic<bool> threadStopped{false};
    std::promise<void> threadStarted;

    {
        Thread thread([&threadRunning, &threadStopped, &threadStarted](std::stop_token token) {
            threadRunning = true;
            threadStarted.set_value();
            while (!token.stop_requested()) {
                std::this_thread::sleep_for(5ms);
            }
            threadStopped = true;
        });

        thread.start();

        // 等待线程启动
        threadStarted.get_future().wait();
        EXPECT_TRUE(threadRunning);

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
    thread.stop(50ms); // 短超时，可能不会完全等待
    auto end = std::chrono::steady_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    EXPECT_LE(duration.count(), 100); // 应该在大约50ms左右返回

    // 线程可能还在运行，需要最终清理
    if (thread.isJoinable()) {
        thread.waitForFinished();
    }
}

// 测试任务返回值（通过future）
TEST_F(ThreadTest, TaskWithReturnValue)
{
    std::atomic<int> result{0};

    auto task = [&result](std::stop_token) {
        std::this_thread::sleep_for(50ms);
        result = 42;
    };

    Thread thread(task);

    thread.start();
    thread.waitForFinished();

    EXPECT_EQ(result.load(), 42);
}

// 测试多次停止调用
TEST_F(ThreadTest, MultipleStopCalls)
{
    std::atomic<int> stopCount{0};

    Thread thread([&stopCount](std::stop_token token) {
        while (!token.stop_requested()) {
            std::this_thread::sleep_for(5ms);
        }
        ++stopCount;
    });

    thread.start();
    std::this_thread::sleep_for(50ms);

    // 多次调用stop应该是安全的
    thread.stop();
    thread.stop();
    thread.stop();
    thread.requestStop();
    thread.requestStop();

    thread.waitForFinished();

    // 确保任务只停止了一次
    EXPECT_EQ(stopCount.load(), 1);
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
    EXPECT_FALSE(thread.isIdle());
    EXPECT_TRUE(thread.waitForFinished());     // 应该立即返回true
    EXPECT_TRUE(thread.waitForFinished(10ms)); // 应该立即返回true

    // 再次停止应该是安全的
    thread.stop();
    thread.requestStop();

    // 状态应该保持不变
    EXPECT_TRUE(thread.isStopped());
}

// 测试在空闲状态的操作
TEST_F(ThreadTest, OperationsOnIdleThread)
{
    Thread thread;

    EXPECT_TRUE(thread.isIdle());
    EXPECT_FALSE(thread.isRunning());
    EXPECT_FALSE(thread.isStopped());
    EXPECT_TRUE(thread.waitForFinished());     // 应该立即返回true
    EXPECT_TRUE(thread.waitForFinished(10ms)); // 应该立即返回true

    // 停止操作在空闲状态应该是安全的
    thread.stop();
    thread.requestStop();

    // 状态应该保持不变
    EXPECT_TRUE(thread.isIdle());
}

// 性能测试：快速启动停止多个线程
TEST_F(ThreadTest, RapidStartStop)
{
    const int NUM_THREADS = 10;
    std::vector<std::unique_ptr<Thread>> threads;
    std::atomic<int> completedCount{0};

    for (int i = 0; i < NUM_THREADS; ++i) {
        auto thread = std::make_unique<Thread>([&completedCount](std::stop_token token) {
            std::this_thread::sleep_for(10ms);
            ++completedCount;
        });

        EXPECT_TRUE(thread->start());
        threads.push_back(std::move(thread));
    }

    // 等待所有线程完成
    for (auto &thread : threads) {
        thread->waitForFinished();
        EXPECT_FALSE(thread->isRunning());
    }

    EXPECT_EQ(completedCount.load(), NUM_THREADS);
}

// 测试长时间运行的任务
TEST_F(ThreadTest, LongRunningTask)
{
    std::atomic<bool> running{false};
    std::promise<void> started;

    Thread thread([&running, &started](std::stop_token token) {
        running = true;
        started.set_value();

        // 长时间运行，但会响应停止请求
        int counter = 0;
        while (!token.stop_requested() && counter < 1000) {
            std::this_thread::sleep_for(1ms);
            ++counter;
        }
        running = false;
    });

    thread.start();
    started.get_future().wait(); // 等待线程启动

    EXPECT_TRUE(running);

    // 让线程运行一段时间
    std::this_thread::sleep_for(100ms);
    EXPECT_TRUE(running);

    // 然后停止
    thread.stop();
    thread.waitForFinished();

    EXPECT_FALSE(running);
}

// 测试任务在停止请求前自然结束
TEST_F(ThreadTest, NaturalTaskCompletion)
{
    std::atomic<bool> completed{false};

    Thread thread([&completed](std::stop_token) {
        std::this_thread::sleep_for(50ms);
        completed = true;
    });

    thread.start();

    // 不调用stop，等待任务自然完成
    thread.waitForFinished();

    EXPECT_TRUE(completed);
    EXPECT_TRUE(thread.isStopped());
}

// 测试线程局部存储交互
TEST_F(ThreadTest, ThreadLocalStorage)
{
    static thread_local int tls_value = 0;
    std::atomic<int> thread_tls_value{0};

    Thread thread([&thread_tls_value](std::stop_token) {
        tls_value = 42;
        thread_tls_value = tls_value;
    });

    tls_value = 100; // 主线程的值

    thread.start();
    thread.waitForFinished();

    EXPECT_EQ(thread_tls_value.load(), 42);
    EXPECT_EQ(tls_value, 100); // 主线程的TLS不应该被影响
}

// 测试内存使用（基本验证）
TEST_F(ThreadTest, MemoryUsage)
{
    // 创建多个线程确保没有内存泄漏
    const int NUM_THREADS = 5;
    std::vector<std::unique_ptr<Thread>> threads;

    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back(std::make_unique<Thread>([](std::stop_token) {}));
    }

    // 启动所有线程
    for (auto &thread : threads) {
        EXPECT_TRUE(thread->start());
    }

    // 等待所有线程完成
    for (auto &thread : threads) {
        thread->waitForFinished();
    }

    // 没有崩溃就是成功
    SUCCEED();
}

// 测试极端超时值
TEST_F(ThreadTest, ExtremeTimeoutValues)
{
    Thread thread([](std::stop_token) { std::this_thread::sleep_for(100ms); });

    thread.start();

    // 测试各种边界超时值
    EXPECT_FALSE(thread.waitForFinished(0ms));  // 零超时
    EXPECT_FALSE(thread.waitForFinished(-1ms)); // 负超时（应该视为零）
    EXPECT_TRUE(thread.waitForFinished(200ms)); // 足够长的超时

    // 确保线程已停止
    EXPECT_TRUE(thread.isStopped());
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
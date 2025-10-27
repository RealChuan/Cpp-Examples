#pragma once

#include <utils/object.hpp>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <thread>
#include <type_traits>

class Thread : noncopyable
{
public:
    using Task = std::function<void(std::stop_token)>;

    enum class State {
        Idle,     // 初始状态，未启动
        Starting, // 正在启动
        Running,  // 正在运行
        Stopping, // 正在停止
        Stopped   // 已停止
    };

    Thread() = default;

    explicit Thread(Task task)
        : m_task(std::move(task))
    {}

    ~Thread() { stop(); }

    void setTask(Task task)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        if (getStateUnsafe() != State::Idle) {
            throw std::runtime_error("Cannot set task while thread is not idle");
        }
        m_task = std::move(task);
    }

    bool start()
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        // 检查状态
        if (getStateUnsafe() != State::Idle) {
            return false;
        }

        // 检查任务
        if (!m_task) {
            return false;
        }

        // 设置状态为启动中
        m_state.store(State::Starting, std::memory_order_release);

        try {
            // 创建线程
            m_jthread = std::jthread([this](std::stop_token token) { threadMain(token); });

            // 等待线程真正启动 - 修复：使用正确的条件变量等待方式
            m_condState.wait(lock, [this] {
                return getStateUnsafe() == State::Running || getStateUnsafe() == State::Stopped;
            });

            return getStateUnsafe() == State::Running;
        } catch (...) {
            m_state.store(State::Stopped, std::memory_order_release);
            m_condState.notify_all();
            return false;
        }
    }

    void stop() { stopInternal(false); }

    void stop(std::chrono::milliseconds timeout) { stopInternal(true, timeout); }

    void requestStop()
    {
        if (m_jthread.joinable()) {
            m_jthread.request_stop();
        }
    }

    bool waitForFinished()
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        // 如果已经是停止状态，直接返回
        if (getStateUnsafe() == State::Stopped) {
            return true;
        }

        // 如果还没有启动，也直接返回
        if (getStateUnsafe() == State::Idle) {
            return true;
        }

        // 等待状态变为 Stopped - 修复：使用正确的条件变量等待方式
        m_condState.wait(lock, [this] { return getStateUnsafe() == State::Stopped; });

        return true;
    }

    bool waitForFinished(std::chrono::milliseconds timeout)
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        if (getStateUnsafe() == State::Stopped || getStateUnsafe() == State::Idle) {
            return true;
        }

        return m_condState.wait_for(lock, timeout, [this] {
            return getStateUnsafe() == State::Stopped;
        });
    }

    [[nodiscard]] bool isRunning() const
    {
        auto state = m_state.load(std::memory_order_acquire);
        return state == State::Running || state == State::Starting;
    }

    [[nodiscard]] bool isStopped() const
    {
        return m_state.load(std::memory_order_acquire) == State::Stopped;
    }

    [[nodiscard]] bool isIdle() const
    {
        return m_state.load(std::memory_order_acquire) == State::Idle;
    }

    [[nodiscard]] State getState() const { return m_state.load(std::memory_order_acquire); }

    [[nodiscard]] bool isJoinable() const { return m_jthread.joinable(); }

    [[nodiscard]] std::thread::id getThreadId() const { return m_jthread.get_id(); }

    [[nodiscard]] std::stop_token getStopToken() const { return m_jthread.get_stop_token(); }

    // 静态工具方法
    static void yield() { std::this_thread::yield(); }

    static void sleep(unsigned int seconds)
    {
        std::this_thread::sleep_for(std::chrono::seconds(seconds));
    }

    static void msleep(unsigned int milliseconds)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
    }

    static void usleep(unsigned int microseconds)
    {
        std::this_thread::sleep_for(std::chrono::microseconds(microseconds));
    }

    static void sleepFor(std::chrono::milliseconds duration)
    {
        std::this_thread::sleep_for(duration);
    }

    static void sleepUntil(const std::chrono::system_clock::time_point &wake_time)
    {
        std::this_thread::sleep_until(wake_time);
    }

    static auto hardwareConcurrency() -> unsigned int
    {
        return std::thread::hardware_concurrency();
    }

private:
    void threadMain(std::stop_token token)
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);

            // 检查是否在启动过程中被停止
            if (token.stop_requested()) {
                m_state.store(State::Stopped, std::memory_order_release);
                m_condState.notify_all();
                return;
            }

            // 标记为运行状态
            m_state.store(State::Running, std::memory_order_release);
            m_condState.notify_all();
        }

        // 执行用户任务
        try {
            if (m_task) {
                m_task(token);
            }
        } catch (const std::exception &e) {
            std::cerr << "Thread exception: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Unknown thread exception" << std::endl;
        }

        // 标记线程结束
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_state.store(State::Stopped, std::memory_order_release);
            m_condState.notify_all();
        }
    }

    void stopInternal(bool useTimeout,
                      std::chrono::milliseconds timeout = std::chrono::milliseconds(0))
    {
        State currentState = m_state.load(std::memory_order_acquire);

        // 如果已经是停止状态或空闲状态，直接返回
        if (currentState == State::Stopped || currentState == State::Idle) {
            return;
        }

        // 请求停止
        if (m_jthread.joinable()) {
            m_jthread.request_stop();
        }

        // 等待线程停止
        if (useTimeout) {
            waitForFinished(timeout);
        } else {
            waitForFinished();
        }

        // 如果线程仍然可连接，强制join
        if (!useTimeout && m_jthread.joinable()) {
            m_jthread.join();
        }
    }

    State getStateUnsafe() const { return m_state.load(std::memory_order_relaxed); }

private:
    std::jthread m_jthread;
    mutable std::mutex m_mutex;
    std::condition_variable m_condState;
    std::atomic<State> m_state{State::Idle};
    Task m_task;
};

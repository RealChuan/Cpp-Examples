#pragma once

#include <utils/object.hpp>

#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>
#include <type_traits>

class Thread : noncopyable
{
public:
    using Task = std::function<void(std::stop_token)>;

    enum class State : int {
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
        if (m_state != State::Idle && m_state != State::Stopped) {
            throw std::runtime_error("Cannot set task while thread is not idle");
        }
        m_task = std::move(task);
    }

    bool start()
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        if (m_state != State::Idle || !m_task) {
            return false;
        }

        m_state = State::Starting;

        try {
            m_jthread = std::jthread([this](std::stop_token token) { threadMain(token); });
            return true;
        } catch (...) {
            m_state = State::Stopped;
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

    bool waitForFinished(std::chrono::milliseconds timeout = std::chrono::milliseconds::max())
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        // 如果已经是停止状态或空闲状态，直接返回
        if (m_state == State::Stopped || m_state == State::Idle) {
            return true;
        }

        // 等待状态变为 Stopped
        if (timeout == std::chrono::milliseconds::max()) {
            m_condState.wait(lock, [this] { return m_state == State::Stopped; });
            return true;
        } else {
            return m_condState.wait_for(lock, timeout, [this] { return m_state == State::Stopped; });
        }
    }

    [[nodiscard]] bool isRunning() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_state == State::Running || m_state == State::Starting;
    }

    [[nodiscard]] bool isStopped() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_state == State::Stopped;
    }

    [[nodiscard]] bool isIdle() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_state == State::Idle;
    }

    [[nodiscard]] State getState() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_state;
    }

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
            m_state = State::Running;
        }
        m_condState.notify_all();

        try {
            if (m_task) {
                m_task(token);
            }
        } catch (const std::exception &e) {
            std::cerr << "Thread exception: " << e.what() << std::endl;
        } catch (...) {
            std::cerr << "Unknown thread exception" << std::endl;
        }

        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_state = State::Stopped;
        }
        m_condState.notify_all();
    }

    void stopInternal(bool useTimeout,
                      std::chrono::milliseconds timeout = std::chrono::milliseconds(0))
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_state == State::Stopped || m_state == State::Idle) {
                return;
            }
        }

        requestStop();

        if (useTimeout) {
            waitForFinished(timeout);
        } else {
            waitForFinished();
        }

        if (!useTimeout && m_jthread.joinable()) {
            m_jthread.join();
        }
    }

private:
    std::jthread m_jthread;
    mutable std::mutex m_mutex;
    std::condition_variable m_condState;
    State m_state{State::Idle};
    Task m_task;
};

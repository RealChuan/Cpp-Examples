#pragma once

#include <utils/object.hpp>

#include <atomic>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>

class Thread : noncopyable
{
public:
    using Task = std::function<void()>;

    Thread() = default;
    explicit Thread(Task task) { setTask(task); }
    ~Thread() { stop(); }

    void setTask(Task task) { m_task = std::move(task); }

    void start()
    {
        m_running.store(true);
        m_thread = std::thread([this]() {
            m_condition.notify_one();
            if (m_task) {
                m_task();
            }
        });
    }

    void stop()
    {
        if (m_thread.joinable()) {
            m_thread.join();
        }
        m_running.store(false);
    }

    void waitForStarted()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condition.wait(lock, [this]() { return m_running.load(); });
    }

    auto isRunning() const -> bool { return m_running; }

    auto getThreadId() const -> std::thread::id { return m_thread.get_id(); }

    static void yield() { std::this_thread::yield(); }

    static void sleepFor(std::chrono::milliseconds duration)
    {
        std::this_thread::sleep_for(duration);
    }

    static void sleepUntil(std::chrono::system_clock::time_point timePoint)
    {
        std::this_thread::sleep_until(timePoint);
    }

    static auto hardwareConcurrency() -> unsigned int
    {
        unsigned int n = std::thread::hardware_concurrency(); // 如果不支持，返回0
        assert(n > 0);
        return n;
    }

private:
    std::thread m_thread;
    std::atomic_bool m_running{false};
    std::mutex m_mutex;
    std::condition_variable m_condition;
    Task m_task;
};

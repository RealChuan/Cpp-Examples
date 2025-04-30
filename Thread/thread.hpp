#pragma once

#include <utils/object.hpp>

#include <atomic>
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <thread>

class Thread : noncopyable
{
public:
    using Task = std::function<void(std::stop_token)>;

    Thread() = default;

    explicit Thread(Task task)
        : m_task(std::move(task))
    {}

    ~Thread() { stop(); }

    void setTask(Task task)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_task = std::move(task);
    }

    bool start()
    {
        if (m_jthread.joinable()) {
            return false;
        }

        m_running.store(true);
        m_jthread = std::jthread([this](std::stop_token token) {
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                if (m_task) {
                    try {
                        m_task(token);
                    } catch (const std::exception &e) {
                        std::cerr << "Thread exception: " << e.what() << std::endl;
                    }
                }
            }

            m_running.store(false);
            m_running.notify_all();
        });

        return true;
    }

    void stop()
    {
        if (m_jthread.joinable()) {
            m_jthread.request_stop();
            m_jthread.join();
        }
    }

    void interrupt()
    {
        if (m_jthread.joinable()) {
            m_jthread.request_stop();
        }
    }

    void waitForFinished()
    {
        while (m_running.load()) {
            m_running.wait(true);
        }
    }

    [[nodiscard]] auto isRunning() const -> bool { return m_running.load(); }

    [[nodiscard]] auto getThreadId() const -> std::thread::id { return m_jthread.get_id(); }

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

    static void nsleep(unsigned int nanoseconds)
    {
        std::this_thread::sleep_for(std::chrono::nanoseconds(nanoseconds));
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
        auto availableCores = std::thread::hardware_concurrency();
        assert(availableCores > 0);
        return availableCores;
    }

private:
    std::jthread m_jthread;
    std::atomic_bool m_running{false};
    std::mutex m_mutex;
    Task m_task;
};

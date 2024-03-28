#pragma once

#include <utils/object.hpp>

#include <atomic>
#include <cassert>
#include <condition_variable>
#include <mutex>

class CountDownLatch : noncopyable
{
public:
    explicit CountDownLatch(int count)
        : m_count(count)
    {
        assert(m_count >= 0);
    }

    void wait()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condition.wait(lock, [&]() { return m_count == 0; });
    }
    void countDown()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (--m_count == 0) {
            m_condition.notify_all();
        }
    }
    [[nodiscard]] auto getCount() const -> int
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_count;
    }

private:
    mutable std::mutex m_mutex;
    std::condition_variable m_condition;
    std::atomic<int> m_count;
};

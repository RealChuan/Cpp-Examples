#pragma once

#include <utils/object.hpp>

#include <condition_variable>
#include <functional>
#include <mutex>
#include <utility>
#include <queue>

template<typename T>
    requires std::is_move_assignable_v<T>
class Queue : noncopyable
{
    std::queue<T> m_queue;
    bool m_stop = false;
    mutable std::mutex m_mutex;
    std::condition_variable m_condEmpty;
    std::condition_variable m_condFull;
    std::size_t m_maxSize = 0;

public:
    using PushSuccessCallback = std::function<void(void)>;

    Queue() = default;
    explicit Queue(std::size_t maxSize)
        : m_maxSize(maxSize == 0 ? 1 : maxSize)
    {}
    ~Queue() { stop(); }

    [[nodiscard]] auto push(T &&item, PushSuccessCallback callback = nullptr) -> bool
    {
        {
            std::unique_lock lock(m_mutex);
            m_condFull.wait(lock, [&]() { return !m_stop && (m_queue.size() < m_maxSize); });
            if (m_stop) {
                return false;
            }
            m_queue.push(std::move(item));
            if (callback) {
                callback();
            }
        }
        m_condEmpty.notify_one();
        return true;
    }

    [[nodiscard]] auto pop(T &item) -> bool
    {
        std::unique_lock lock(m_mutex);
        m_condEmpty.wait(lock, [&]() { return !m_queue.empty() || m_stop; });
        if (m_queue.empty()) {
            return false;
        }
        item = std::move(m_queue.front());
        m_queue.pop();
        m_condFull.notify_one();
        return true;
    }

    void setMaxSize(std::size_t maxSize)
    {
        std::scoped_lock guard(m_mutex);
        m_maxSize = maxSize == 0 ? 1 : maxSize;
        m_condFull.notify_all();
    }

    [[nodiscard]] auto getMaxSize() const -> std::size_t
    {
        std::scoped_lock guard(m_mutex);
        return m_maxSize;
    }

    [[nodiscard]] auto size() const -> std::size_t
    {
        std::scoped_lock guard(m_mutex);
        return m_queue.size();
    }

    [[nodiscard]] auto empty() const -> bool
    {
        std::scoped_lock guard(m_mutex);
        return m_queue.empty();
    }

    void clear()
    {
        std::scoped_lock guard(m_mutex);
        std::queue<T>().swap(m_queue);
        m_condFull.notify_all();
        m_condEmpty.notify_all();
    }

    [[nodiscard]] auto isStopped() const -> bool
    {
        std::scoped_lock guard(m_mutex);
        return m_stop;
    }

    void start()
    {
        std::scoped_lock guard(m_mutex);
        m_stop = false;
    }

    void stop()
    {
        {
            std::scoped_lock guard(m_mutex);
            m_stop = true;
        }
        m_condEmpty.notify_all();
        m_condFull.notify_all();
    }
};

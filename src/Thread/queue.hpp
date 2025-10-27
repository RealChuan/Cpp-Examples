#pragma once

#include <utils/object.hpp>

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <optional>
#include <utility>
#include <queue>

template<typename T>
class Queue : noncopyable
{
    std::queue<T> m_queue;
    std::atomic<bool> m_stop = false; // 改为 atomic
    mutable std::mutex m_mutex;
    std::condition_variable m_condEmpty;
    std::condition_variable m_condFull;
    std::size_t m_maxSize = 0;

public:
    Queue() = default;

    explicit Queue(std::size_t maxSize)
        : m_maxSize(maxSize == 0 ? 1 : maxSize)
    {}

    ~Queue()
    {
        stop();
        clear();
    }

    // 阻塞push，直到有空间可用或队列停止
    [[nodiscard]] auto push(T &&item) -> bool
    {
        bool success = false;
        {
            std::unique_lock lock(m_mutex);
            m_condFull.wait(lock, [this]() {
                return m_stop.load() || m_maxSize == 0 || m_queue.size() < m_maxSize;
            });

            if (m_stop.load()) {
                return false;
            }

            m_queue.push(std::move(item));
            success = true;
        }

        if (success) {
            m_condEmpty.notify_one();
        }
        return success;
    }

    // 阻塞push，复制版本
    [[nodiscard]] auto push(const T &item) -> bool
    {
        T temp = item;
        return push(std::move(temp));
    }

    // 非阻塞push，立即返回结果
    [[nodiscard]] auto try_push(T &&item) -> bool
    {
        std::unique_lock lock(m_mutex, std::try_to_lock);
        if (!lock.owns_lock() || m_stop.load()) {
            return false;
        }

        if (m_maxSize > 0 && m_queue.size() >= m_maxSize) {
            return false;
        }

        m_queue.push(std::move(item));
        lock.unlock();
        m_condEmpty.notify_one();
        return true;
    }

    // 带超时的push
    template<typename Rep, typename Period>
    [[nodiscard]] auto push_for(T &&item, const std::chrono::duration<Rep, Period> &timeout) -> bool
    {
        bool success = false;
        {
            std::unique_lock lock(m_mutex);
            if (!m_condFull.wait_for(lock, timeout, [this]() {
                    return m_stop.load() || m_maxSize == 0 || m_queue.size() < m_maxSize;
                })) {
                return false; // 超时
            }

            if (m_stop.load()) {
                return false;
            }

            m_queue.push(std::move(item));
            success = true;
        }

        if (success) {
            m_condEmpty.notify_one();
        }
        return success;
    }

    // 阻塞pop，使用输出参数
    [[nodiscard]] auto pop(T &item) -> bool
    {
        std::unique_lock lock(m_mutex);
        m_condEmpty.wait(lock, [this]() { return !m_queue.empty() || m_stop.load(); });

        if (m_queue.empty() || m_stop.load()) {
            return false;
        }

        item = std::move(m_queue.front());
        m_queue.pop();

        lock.unlock();
        m_condFull.notify_one();
        return true;
    }

    // 阻塞pop，返回optional（推荐使用）
    [[nodiscard]] auto pop() -> std::optional<T>
    {
        std::unique_lock lock(m_mutex);
        m_condEmpty.wait(lock, [this]() { return !m_queue.empty() || m_stop.load(); });

        if (m_queue.empty() || m_stop.load()) {
            return std::nullopt;
        }

        T item = std::move(m_queue.front());
        m_queue.pop();

        lock.unlock();
        m_condFull.notify_one();
        return std::move(item);
    }

    // 非阻塞pop，使用输出参数
    [[nodiscard]] auto try_pop(T &item) -> bool
    {
        std::unique_lock lock(m_mutex, std::try_to_lock);
        if (!lock.owns_lock() || m_queue.empty() || m_stop.load()) {
            return false;
        }

        item = std::move(m_queue.front());
        m_queue.pop();

        lock.unlock();
        m_condFull.notify_one();
        return true;
    }

    // 非阻塞pop，返回optional
    [[nodiscard]] auto try_pop() -> std::optional<T>
    {
        std::unique_lock lock(m_mutex, std::try_to_lock);
        if (!lock.owns_lock() || m_queue.empty() || m_stop.load()) {
            return std::nullopt;
        }

        T item = std::move(m_queue.front());
        m_queue.pop();

        lock.unlock();
        m_condFull.notify_one();
        return std::move(item);
    }

    // 带超时的pop，使用输出参数
    template<typename Rep, typename Period>
    [[nodiscard]] auto pop_for(T &item, const std::chrono::duration<Rep, Period> &timeout) -> bool
    {
        std::unique_lock lock(m_mutex);
        if (!m_condEmpty.wait_for(lock, timeout, [this]() {
                return !m_queue.empty() || m_stop.load();
            })) {
            return false; // 超时
        }

        if (m_queue.empty() || m_stop.load()) {
            return false;
        }

        item = std::move(m_queue.front());
        m_queue.pop();

        lock.unlock();
        m_condFull.notify_one();
        return true;
    }

    // 带超时的pop，返回optional
    template<typename Rep, typename Period>
    [[nodiscard]] auto pop_for(const std::chrono::duration<Rep, Period> &timeout)
        -> std::optional<T>
    {
        std::unique_lock lock(m_mutex);
        if (!m_condEmpty.wait_for(lock, timeout, [this]() {
                return !m_queue.empty() || m_stop.load();
            })) {
            return std::nullopt; // 超时
        }

        if (m_queue.empty() || m_stop.load()) {
            return std::nullopt;
        }

        T item = std::move(m_queue.front());
        m_queue.pop();

        lock.unlock();
        m_condFull.notify_one();
        return std::move(item);
    }

    // 设置最大容量
    void setMaxSize(std::size_t maxSize)
    {
        {
            std::scoped_lock guard(m_mutex);
            m_maxSize = maxSize == 0 ? 1 : maxSize;
        }
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

    // 清空队列
    void clear()
    {
        std::scoped_lock guard(m_mutex);
        std::queue<T>().swap(m_queue);
        m_condFull.notify_all();
    }

    // 清空并返回所有剩余元素
    [[nodiscard]] auto flush() -> std::vector<T>
    {
        std::vector<T> result;
        {
            std::scoped_lock guard(m_mutex);
            while (!m_queue.empty()) {
                result.push_back(std::move(m_queue.front()));
                m_queue.pop();
            }
        }
        m_condFull.notify_all();
        return result;
    }

    [[nodiscard]] auto isStopped() const -> bool { return m_stop.load(); }

    void start() { m_stop.store(false); }

    void stop()
    {
        bool expected = false;
        if (m_stop.compare_exchange_strong(expected, true)) {
            m_condEmpty.notify_all();
            m_condFull.notify_all();
        }
    }
};

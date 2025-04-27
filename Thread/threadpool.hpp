#pragma once

#include "thread.hpp"

#include <iostream>
#include <semaphore>
#include <queue>

class ThreadPool : noncopyable
{
public:
    using Task = Thread::Task;

    explicit ThreadPool(uint64_t maxTasks = static_cast<uint64_t>(Thread::hardwareConcurrency()) * 4,
                        uint32_t maxThreads = Thread::hardwareConcurrency())
        : m_maxTasks(maxTasks)
        , m_maxThreads(maxThreads)
    {
        assert(m_maxTasks > 0);
        assert(m_maxThreads > 0);
    }
    ~ThreadPool()
    {
        if (m_running.load()) {
            stop();
        }
    }

    void start()
    {
        assert(!m_running.load());
        m_running.store(true);
        for (int i = 0; i < m_maxThreads; ++i) {
            auto *thread = new Thread([this] { runInThread(); });
            m_threads.emplace_back(thread);
            thread->start();
        }
    }

    void stop()
    {
        assert(m_running.load());
        m_running.store(false);
        m_emptyCondition.notify_all();
        m_fullCondition.notify_all();
        for (auto &thread : m_threads) {
            thread->stop();
        }
        m_threads.clear();
    }

    void waitForDone()
    {
        assert(m_running.load());
        std::binary_semaphore semaphore(0);
        addTask([&]() { semaphore.release(); });
        semaphore.acquire();
    }

    void addTask(Task task)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        while (m_tasks.size() >= m_maxTasks && m_running.load()) {
            m_fullCondition.wait(lock);
        }
        if (!m_running.load()) {
            return;
        }
        m_tasks.push(std::move(task));
        m_emptyCondition.notify_one();
    }

    void clearTasks()
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::queue<Task>().swap(m_tasks);
    }

    [[nodiscard]] auto activeThreadCount() const -> std::size_t { return m_threads.size(); }
    [[nodiscard]] auto queuedTaskCount() const -> std::size_t
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_tasks.size();
    }

    [[nodiscard]] auto isRunning() const -> bool { return m_running.load(); }

private:
    void runInThread()
    {
        while (m_running.load()) {
            Task task;
            {
                std::unique_lock<std::mutex> lock(m_mutex);
                while (m_tasks.empty() && m_running.load()) {
                    m_emptyCondition.wait(lock);
                }
                if (!m_running.load()) {
                    return;
                }
                task = std::move(m_tasks.front());
                m_tasks.pop();
                m_fullCondition.notify_one();
            }
            task();
        }
    }

    uint32_t m_maxThreads = 0;
    uint64_t m_maxTasks = 0;
    std::atomic_bool m_running = false;

    mutable std::mutex m_mutex;
    std::condition_variable m_emptyCondition;
    std::condition_variable m_fullCondition;

    std::queue<Task> m_tasks;
    std::vector<std::unique_ptr<Thread>> m_threads;
};

#pragma once

#include "queue.hpp"
#include "thread.hpp"

class ThreadPool : noncopyable
{
public:
    using Task = Thread::Task;

    explicit ThreadPool(uint64_t maxTasks = static_cast<uint64_t>(Thread::hardwareConcurrency()) * 4,
                        uint32_t maxThreads = Thread::hardwareConcurrency())
        : m_maxThreads(maxThreads)
        , m_queue(maxTasks)
    {
        assert(maxTasks > 0);
        assert(maxThreads > 0);
    }

    ~ThreadPool()
    {
        if (isRunning()) {
            stop();
        }
    }

    void start()
    {
        assert(!isRunning());
        m_queue.start();

        for (uint32_t i = 0; i < m_maxThreads; ++i) {
            auto *thread = new Thread([this](std::stop_token token) { runInThread(token); });
            m_threads.emplace_back(thread);
            thread->start();
        }
    }

    void stop()
    {
        m_queue.stop();
        m_threads.clear();
    }

    void waitForDone()
    {
        if (!isRunning()) {
            return;
        }

        while (!m_allDone.load()) {
            m_allDone.wait(false);
        }
    }

    bool addTask(Task task)
    {
        if (!isRunning()) {
            return false;
        }

        return m_queue.push(std::move(task), [&]() {
            m_allDone.store(false);
            m_taskCount.fetch_add(1);
        });
    }

    void clearTasks()
    {
        m_queue.clear();
        m_taskCount.store(0);
        notifyAllDone();
    }

    [[nodiscard]] auto activeThreadCount() const -> std::size_t { return m_threads.size(); }

    [[nodiscard]] auto queuedTaskCount() const -> std::size_t { return m_queue.size(); }

    [[nodiscard]] auto isRunning() const -> bool
    {
        return m_threads.empty() ? false : m_threads[0]->isRunning();
    }

private:
    void runInThread(std::stop_token token)
    {
        while (!token.stop_requested()) {
            Task task;
            if (m_queue.pop(task)) {
                task(token);
                m_taskCount.fetch_sub(1);
                notifyAllDone();
            }
        }
    }

    void notifyAllDone()
    {
        if (m_taskCount.load() == 0) {
            m_allDone.store(true);
            m_allDone.notify_all();
        }
    }

    uint32_t m_maxThreads;
    Queue<Task> m_queue;
    std::atomic_uint64_t m_taskCount{0};
    std::atomic_bool m_allDone{true};
    std::vector<std::unique_ptr<Thread>> m_threads;
};

#pragma once

#include "thread.hpp"

#include <CountDownLatch/countdownlatch.hpp>

#include <iostream>
#include <queue>

class ThreadPool : noncopyable
{
public:
    using Task = Thread::Task;

    explicit ThreadPool(int maxTasks = Thread::hardwareConcurrency() * 4,
                        int maxThreads = Thread::hardwareConcurrency())
        : m_maxTasks(maxTasks)
        , m_maxThreads(maxThreads)
    {
        assert(m_maxTasks > 0);
        assert(m_maxThreads > 0);
    }
    ~ThreadPool()
    {
        if (m_running) {
            stop();
        }
    }

    void start()
    {
        assert(!m_running);
        m_running.store(true);
        for (int i = 0; i < m_maxThreads; ++i) {
            auto thread = new Thread(std::bind(&ThreadPool::runInThread, this));
            m_threads.emplace_back(thread);
            thread->start();
        }
    }
    void stop()
    {
        assert(m_running);
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
        assert(m_running);
        CountDownLatch latch(1);
        addTask([&]() { latch.countDown(); });
        latch.wait();
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

    int activeThreadCount() const { return m_threads.size(); }
    int queuedTaskCount() const
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_tasks.size();
    }

    bool isRunning() const { return m_running; }

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

    int m_maxThreads = 0;
    int m_maxTasks = 0;
    std::atomic_bool m_running = false;

    mutable std::mutex m_mutex;
    std::condition_variable m_emptyCondition;
    std::condition_variable m_fullCondition;

    std::queue<Task> m_tasks;
    std::vector<std::unique_ptr<Thread>> m_threads;
};

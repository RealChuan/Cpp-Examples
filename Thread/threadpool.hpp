#pragma once

#include "thread.hpp"

#include <iostream>
#include <queue>

class TaskQueue : noncopyable
{
public:
    using Task = Thread::Task;

    TaskQueue(int maxTaskCount = 1000) { setMaxSize(maxTaskCount); }
    ~TaskQueue() { clearTasks(); }

    void setMaxSize(int maxTaskCount)
    {
        assert(maxTaskCount > 0);
        m_maxTaskCount = maxTaskCount;
    }

    void setRunning(bool running) { m_running.store(running); }

    void addTask(Task task)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condition.wait(lock,
                         [this]() { return m_running.load() || m_tasks.size() < m_maxTaskCount; });
        if (m_running.load()) {
            m_tasks.push(std::move(task));
            m_condition.notify_one();
        }
    }

    Task takeTask()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condition.wait(lock, [this]() { return !m_tasks.empty() || !m_running.load(); });
        Task task;
        if (!m_tasks.empty()) {
            task = std::move(m_tasks.front());
            m_tasks.pop();
            m_condition.notify_one();
        }
        return task;
    }

    void notifyAll() { m_condition.notify_all(); }

    void clearTasks()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        std::queue<Task> empty;
        std::swap(m_tasks, empty);
    }

    unsigned int queuedTaskCount() const
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        return static_cast<unsigned int>(m_tasks.size());
    }

private:
    int m_maxTaskCount = 0;
    std::atomic_bool m_running{true};
    std::queue<Task> m_tasks;
    mutable std::mutex m_mutex;
    std::condition_variable m_condition;
};

class ThreadPool : noncopyable
{
public:
    using Task = Thread::Task;

    explicit ThreadPool(int threadCount = Thread::hardwareConcurrency())
    {
        assert(threadCount > 0);
        m_threads.reserve(threadCount);
        for (int i = 0; i < threadCount; ++i) {
            m_threads.emplace_back([this]() {
                while (m_running.load()) {
                    Task task = m_taskQueue.takeTask();
                    if (task) {
                        task();
                    }
                }
            });
        }
    }

    ~ThreadPool()
    {
        m_running.store(false);
        m_taskQueue.setRunning(false);
        m_taskQueue.notifyAll();
        for (auto &thread : m_threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    void addTask(Task task) { m_taskQueue.addTask(std::move(task)); }

    void clearTasks() { m_taskQueue.clearTasks(); }

    void waitForDone()
    {
        while (m_taskQueue.queuedTaskCount() > 0) {
            Thread::yield();
        }
    }

    unsigned int activeThreadCount() const { return m_threads.size(); }

    unsigned int queuedTaskCount() const { return m_taskQueue.queuedTaskCount(); }

private:
    std::atomic_bool m_running{true};
    std::vector<std::thread> m_threads;
    TaskQueue m_taskQueue;
};

#pragma once

#include "thread.hpp"

#include <future>
#include <queue>

class ThreadPool : noncopyable
{
public:
    using Task = Thread::Task; // 统一使用 Thread::Task 类型

    explicit ThreadPool(size_t threadCount = std::thread::hardware_concurrency(),
                        size_t maxQueueSize = 1000)
        : m_maxQueueSize(maxQueueSize == 0 ? 1 : maxQueueSize)
    {
        if (threadCount == 0) {
            threadCount = 1;
        }

        initializeWorkers(threadCount);
    }

    ~ThreadPool() { shutdownNow(); }

    // 提交任务（无返回值）
    template<typename F>
    auto submit(F &&task) -> bool
    {
        return submitInternal(std::forward<F>(task), false, std::chrono::milliseconds(0));
    }

    // 尝试提交任务（非阻塞）
    template<typename F>
    auto trySubmit(F &&task) -> bool
    {
        return submitInternal(std::forward<F>(task), true, std::chrono::milliseconds(0));
    }

    // 带超时的任务提交
    template<typename F, typename Rep, typename Period>
    auto submitFor(F &&task, const std::chrono::duration<Rep, Period> &timeout) -> bool
    {
        return submitInternal(std::forward<F>(task),
                              false,
                              std::chrono::duration_cast<std::chrono::milliseconds>(timeout));
    }

    // 提交接受 stop_token 的任务并返回 future
    template<typename F, typename... Args>
    auto submitFuture(F &&f, Args &&...args)
        -> std::future<std::invoke_result_t<F, std::stop_token, Args...>>
    {
        using return_type = std::invoke_result_t<F, std::stop_token, Args...>;

        auto task = std::make_shared<std::packaged_task<return_type(std::stop_token)>>(
            [func = std::forward<F>(f),
             args = std::make_tuple(std::forward<Args>(args)...)](std::stop_token token) mutable {
                return std::apply(func, std::tuple_cat(std::make_tuple(token), std::move(args)));
            });

        std::future<return_type> result = task->get_future();

        bool success = submit([task](std::stop_token token) { (*task)(token); });
        if (!success) {
            std::promise<return_type> p;
            p.set_exception(std::make_exception_ptr(
                std::runtime_error("ThreadPool is stopped or queue is full")));
            return p.get_future();
        }

        return result;
    }

    // 等待所有任务完成
    void waitAll()
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condAllDone.wait(lock, [this]() {
            return m_totalTasks == 0 && m_taskQueue.empty() && m_runningTasks == 0;
        });
    }

    // 带超时的等待
    bool waitAllFor(std::chrono::milliseconds timeout = std::chrono::milliseconds::max())
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        return m_condAllDone.wait_for(lock, timeout, [this]() {
            return m_totalTasks == 0 && m_taskQueue.empty() && m_runningTasks == 0;
        });
    }

    // 优雅关闭 - 等待所有任务完成
    void shutdown()
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_stop) {
                return; // 已经在关闭过程中
            }
            m_stop = true;
        }

        // 通知所有条件变量（不需要在锁内通知）
        m_condEmpty.notify_all();
        m_condFull.notify_all();

        // 停止所有工作线程
        for (auto &worker : m_workers) {
            if (worker->isJoinable()) {
                worker->requestStop();
            }
        }

        // 等待所有工作线程结束
        for (auto &worker : m_workers) {
            if (worker->isJoinable()) {
                worker->waitForFinished(std::chrono::seconds(5));
            }
        }

        // 清空任务队列
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            std::queue<Task>().swap(m_taskQueue);
            m_totalTasks = 0;
            m_runningTasks = 0;
        }

        m_condAllDone.notify_all();
    }

    // 立即关闭 - 丢弃未执行的任务
    void shutdownNow()
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_stop) {
                return;
            }
            m_stop = true;

            // 清空任务队列
            std::queue<Task>().swap(m_taskQueue);
            m_totalTasks = 0;
        }

        // 通知所有条件变量
        m_condEmpty.notify_all();
        m_condFull.notify_all();
        m_condAllDone.notify_all();

        // 停止所有工作线程
        for (auto &worker : m_workers) {
            if (worker->isJoinable()) {
                worker->requestStop();
            }
        }

        // 等待工作线程结束（带超时）
        for (auto &worker : m_workers) {
            if (worker->isJoinable()) {
                if (!worker->waitForFinished(std::chrono::seconds(3))) {
                    // 超时，强制停止
                    worker->stop();
                }
            }
        }

        m_workers.clear();
    }

    // 重启线程池
    bool restart(size_t threadCount = std::thread::hardware_concurrency())
    {
        // 先关闭
        shutdownNow();

        // 重置状态
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_stop = false;
            m_runningTasks = 0;
            m_totalTasks = 0;
        }

        // 创建新的工作线程
        return initializeWorkers(threadCount);
    }

    // 获取线程池状态
    [[nodiscard]] auto isRunning() const -> bool
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return !m_stop;
    }

    [[nodiscard]] auto isStopped() const -> bool
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_stop;
    }

    [[nodiscard]] auto size() const -> size_t { return m_workers.size(); }

    [[nodiscard]] auto queueSize() const -> size_t
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_taskQueue.size();
    }

    [[nodiscard]] auto getMaxQueueSize() const -> size_t
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_maxQueueSize;
    }

    [[nodiscard]] auto getRunningTasks() const -> size_t
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_runningTasks;
    }

    [[nodiscard]] auto getPendingTasks() const -> size_t
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_taskQueue.size();
    }

    [[nodiscard]] auto getTotalTasks() const -> size_t
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        return m_totalTasks;
    }

    // 设置最大队列大小
    void setMaxQueueSize(size_t maxSize)
    {
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            m_maxQueueSize = maxSize == 0 ? 1 : maxSize;
        }
        m_condFull.notify_all();
    }

private:
    bool initializeWorkers(size_t threadCount)
    {
        try {
            m_workers.clear();
            for (size_t i = 0; i < threadCount; ++i) {
                auto worker = std::make_unique<Thread>(
                    [this](std::stop_token token) { workerThread(token); });

                if (!worker->start()) {
                    shutdownNow();
                    return false;
                }

                m_workers.push_back(std::move(worker));
            }
            return true;
        } catch (...) {
            shutdownNow();
            return false;
        }
    }

    template<typename F>
    bool submitInternal(F &&task, bool nonBlocking, std::chrono::milliseconds timeout)
    {
        // 快速检查是否已停止
        {
            std::lock_guard<std::mutex> lock(m_mutex);
            if (m_stop) {
                return false;
            }
        }
        {
            std::unique_lock<std::mutex> lock(m_mutex);

            // 检查队列是否已满
            if (m_maxQueueSize > 0 && m_taskQueue.size() >= m_maxQueueSize) {
                if (nonBlocking) {
                    return false;
                }

                if (timeout.count() > 0) {
                    // 带超时等待
                    if (!m_condFull.wait_for(lock, timeout, [this]() {
                            return m_stop || m_taskQueue.size() < m_maxQueueSize;
                        })) {
                        return false; // 超时
                    }
                } else {
                    // 无限等待
                    m_condFull.wait(lock, [this]() {
                        return m_stop || m_taskQueue.size() < m_maxQueueSize;
                    });
                }
            }

            if (m_stop) {
                return false;
            }

            m_taskQueue.push(std::forward<F>(task));
            m_totalTasks++;
        }
        m_condEmpty.notify_one();
        return true;
    }

    void workerThread(std::stop_token token)
    {
        while (true) {
            Task task;

            {
                std::unique_lock<std::mutex> lock(m_mutex);

                // 等待任务或停止信号
                m_condEmpty.wait(lock, [this, &token]() {
                    return !m_taskQueue.empty() || m_stop || token.stop_requested();
                });

                // 检查停止条件
                if (m_stop || token.stop_requested()) {
                    break;
                }

                if (m_taskQueue.empty()) {
                    continue;
                }

                // 获取任务
                task = std::move(m_taskQueue.front());
                m_taskQueue.pop();
                if (m_maxQueueSize > 0 && m_taskQueue.size() < m_maxQueueSize) {
                    m_condFull.notify_one();
                }
                m_runningTasks++;
            }

            // 执行任务，传递 stop_token
            try {
                if (task) {
                    task(token); // 传递 stop_token 给任务
                }
            } catch (const std::exception &e) {
                std::cerr << "ThreadPool task exception: " << e.what() << std::endl;
            } catch (...) {
                std::cerr << "ThreadPool unknown task exception" << std::endl;
            }

            // 更新计数
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_runningTasks--;
                m_totalTasks--;

                // 通知等待的线程
                if (m_totalTasks == 0 && m_taskQueue.empty() && m_runningTasks == 0) {
                    m_condAllDone.notify_all();
                }
            }
        }
    }

private:
    std::vector<std::unique_ptr<Thread>> m_workers;
    std::queue<Task> m_taskQueue;

    // 所有状态变量都用 mutex 保护
    mutable std::mutex m_mutex;
    bool m_stop{false};
    size_t m_maxQueueSize;
    size_t m_runningTasks{0};
    size_t m_totalTasks{0};

    std::condition_variable m_condEmpty;
    std::condition_variable m_condFull;
    std::condition_variable m_condAllDone;
};

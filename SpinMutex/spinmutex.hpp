#pragma once

#include <utils/object.hpp>

#include <atomic>
#include <cassert>
#include <iostream>
#include <thread>

class SpinMutex : noncopyable
{
public:
    SpinMutex() { m_atomic_flag.clear(std::memory_order_release); }

    ~SpinMutex()
    {
        if (m_atomic_flag.test_and_set(std::memory_order_acquire)) {
            assert(false && "SpinLock destroyed while locked!");
            std::cerr << "Error: SpinLock destroyed while locked!" << std::endl;
        }
        m_atomic_flag.clear(std::memory_order_release);
    }

    void lock()
    {
        while (m_atomic_flag.test_and_set(std::memory_order_acquire)) {
            std::this_thread::yield(); // 有这一行，就是自旋锁；没有这一行，就是互斥锁（忙等待）
        }
    }

    void unlock() { m_atomic_flag.clear(std::memory_order_release); }

private:
    std::atomic_flag m_atomic_flag;
};

class SpinMutexLocker : noncopyable
{
public:
    explicit SpinMutexLocker(SpinMutex *mutex)
        : m_mutex(mutex)
    {
        assert(m_mutex);
        m_mutex->lock();
    }

    ~SpinMutexLocker()
    {
        assert(m_mutex);
        m_mutex->unlock();
    }

private:
    SpinMutex *m_mutex = nullptr;
};

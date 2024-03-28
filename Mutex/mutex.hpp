#pragma once

#include <utils/object.hpp>

#include <atomic>
#include <cassert>
#include <thread>

class Mutex : noncopyable
{
public:
    Mutex() {}

    ~Mutex() { assert(!m_atomic_flag.test_and_set()); }

    void lock()
    {
        while (m_atomic_flag.test_and_set(std::memory_order_acquire)) {
            std::this_thread::yield(); // 有这一行，就是自旋锁；没有这一行，就是忙等待
        }
    }

    void unlock() { m_atomic_flag.clear(std::memory_order_release); }

private:
    std::atomic_flag m_atomic_flag;
};

class MutexLocker : noncopyable
{
    Mutex *m_mutex = nullptr;

public:
    MutexLocker(Mutex *mutex)
        : m_mutex(mutex)
    {
        assert(m_mutex);
        m_mutex->lock();
    }

    ~MutexLocker()
    {
        assert(m_mutex);
        m_mutex->unlock();
    }
};

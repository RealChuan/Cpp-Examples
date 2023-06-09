#pragma once

#include <atomic>
#include <cassert>
#include <thread>

#include <object.hpp>

class Mutex
{
    DISABLE_COPY(Mutex)
public:
    Mutex() {}

    ~Mutex() { assert(!m_atomic_flag.test_and_set()); }

    void lock()
    {
        while (m_atomic_flag.test_and_set(std::memory_order_acquire)) {
            std::this_thread::yield();
        }
    }

    void unlock() { m_atomic_flag.clear(std::memory_order_release); }

private:
    std::atomic_flag m_atomic_flag;
};

class MutexLocker
{
    Mutex *m_mutex;

    DISABLE_COPY(MutexLocker)
public:
    MutexLocker(Mutex *mutex)
        : m_mutex(mutex)
    {
        m_mutex->lock();
    }

    ~MutexLocker() { m_mutex->unlock(); }
};

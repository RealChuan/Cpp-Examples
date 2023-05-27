#ifndef MUTEX_HPP
#define MUTEX_HPP

#include <atomic>
#include <thread>

#include <object.hpp>

class Mutex
{
    DISABLE_COPY(Mutex)
public:
    Mutex() {}
    ~Mutex() {}

    void lock()
    {
        while (m_atomic_flag.test_and_set(std::memory_order_acquire)) {
            // 有这一行 ？ 互斥锁：自旋锁；
            // std::this_thread::yield();
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

#endif // MUTEX_HPP

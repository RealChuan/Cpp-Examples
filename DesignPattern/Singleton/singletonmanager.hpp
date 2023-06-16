#ifndef SINGLETONMANAGER_HPP
#define SINGLETONMANAGER_HPP

#include "singleton.hpp"

class Singleton1
{
public:
    Singleton1() { std::cout << "Singleton1" << std::endl; }
    ~Singleton1() { std::cout << "~Singleton1" << std::endl; }
};

class Singleton2
{
public:
    Singleton2() { std::cout << "Singleton2" << std::endl; }
    ~Singleton2() { std::cout << "~Singleton2" << std::endl; }
};

class SingletonManager
{
    SingletonManager()
        : m_s1_ptr(new Singleton1)
        , m_s2_ptr(new Singleton2)
    {}
    ~SingletonManager() {}

    DISABLE_COPY_MOVE(SingletonManager)

    std::mutex m_mutex;

    std::unique_ptr<Singleton1> m_s1_ptr;
    std::unique_ptr<Singleton2> m_s2_ptr;

public:
    static auto instance() -> SingletonManager &
    {
        static SingletonManager s; // C++11 thread safe
        return s;
    }

    Singleton1 &singleton1()
    {
        std::unique_lock locker(m_mutex);
        return *m_s1_ptr;
    }

    auto singleton2() -> Singleton2 &
    {
        std::unique_lock locker(m_mutex);
        return *m_s2_ptr;
    }
};

#endif // SINGLETONMANAGER_HPP

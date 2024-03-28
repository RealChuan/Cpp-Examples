#ifndef SINGLETON_HPP
#define SINGLETON_HPP

#include <utils/object.hpp>

#include <iostream>
#include <memory>
#include <mutex>

namespace Lazy {

class Singleton
{
    Singleton() { std::cout << "Lazy Singleton" << std::endl; }
    ~Singleton() { std::cout << "Lazy ~Singleton" << std::endl; }

    DISABLE_COPY_MOVE(Singleton)

public:
    static Singleton &instance()
    {
        static Singleton s; // C++11 thread safe
        return s;
    }
};

} // namespace Lazy

namespace Hungry {

class Singleton
{
    Singleton() { std::cout << "Hungry Singleton" << std::endl; }
    //~Singleton() { std::cout << "Hungry ~Singleton" << std::endl; }

    DISABLE_COPY_MOVE(Singleton)

    static std::unique_ptr<Singleton> s_singleton_ptr;
    static std::mutex s_mutex;

public:
    ~Singleton() { std::cout << "Hungry ~Singleton" << std::endl; }

    // ① This the first way to implement Singleton
    // static Singleton &instance()
    // {
    //     std::unique_lock locker(s_mutex);
    //     if (!s_singleton_ptr) {
    //         s_singleton_ptr.reset(new Singleton);
    //     }
    //     return *s_singleton_ptr;
    // }

    // ② This the second way to implement Singleton
    static Singleton &instance()
    {
        static std::once_flag flag;
        std::call_once(flag, []() {
            if (!s_singleton_ptr) {
                s_singleton_ptr.reset(new Singleton);
            }
        });
        return *s_singleton_ptr;
    }
};

} // namespace Hungry

namespace LazyTemplate {

template<typename T>
class Singleton
{
    Singleton() = delete;
    ~Singleton() = delete;

    DISABLE_COPY_MOVE(Singleton)

public:
    static T &instance()
    {
        static T t; // C++11 thread safe
        return t;
    }
};

} // namespace LazyTemplate

#endif // SINGLETON_HPP

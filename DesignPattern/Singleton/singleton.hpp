#pragma once

#include <utils/object.hpp>

#include <iostream>
#include <memory>
#include <mutex>

namespace Lazy {

class Singleton : noncopyable
{
    Singleton() { std::cout << "Lazy Singleton" << std::endl; }
    ~Singleton() { std::cout << "Lazy ~Singleton" << std::endl; }

public:
    static Singleton &instance()
    {
        static Singleton singleton; // C++11 thread safe
        return singleton;
    }
};

} // namespace Lazy

namespace Hungry {

class Singleton : noncopyable
{
    Singleton() { std::cout << "Hungry Singleton" << std::endl; }
    //~Singleton() { std::cout << "Hungry ~Singleton" << std::endl; }

    static std::unique_ptr<Singleton> singletonPtr;
    static std::mutex mutex;

public:
    ~Singleton() { std::cout << "Hungry ~Singleton" << std::endl; }

    // ① This the first way to implement Singleton
    // static Singleton &instance()
    // {
    //     std::unique_lock locker(mutex);
    //     if (!singletonPtr) {
    //         singletonPtr.reset(new Singleton);
    //     }
    //     return *singletonPtr;
    // }

    // ② This the second way to implement Singleton
    static Singleton &instance()
    {
        static std::once_flag flag;
        std::call_once(flag, []() {
            if (!singletonPtr) {
                singletonPtr.reset(new Singleton);
            }
        });
        return *singletonPtr;
    }
};

} // namespace Hungry

namespace LazyTemplate {

template<typename T>
class Singleton : noncopyable
{
public:
    static T &instance()
    {
        static T t; // C++11 thread safe
        return t;
    }
};

} // namespace LazyTemplate

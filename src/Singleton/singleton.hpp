#pragma once

#include <utils/object.hpp>

template<typename T>
class Singleton
{
    DISABLE_COPY_MOVE(Singleton)
public:
    static T &instance()
    {
        static T instance;
        return instance;
    }

    static T *getInstance() { return &instance(); }

protected:
    Singleton() = default;
    virtual ~Singleton() = default;
};

#define SINGLETON(Class) \
private: \
    Class(const Class &) = delete; \
    Class &operator=(const Class &) = delete; \
    Class(Class &&) = delete; \
    Class &operator=(Class &&) = delete; \
    friend class Singleton<Class>; \
\
protected: \
    Class() = default; \
\
public: \
    static Class &instance() \
    { \
        return Singleton<Class>::instance(); \
    }\

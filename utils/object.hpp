#pragma once

#define DISABLE_COPY(Class) \
    Class(const Class &) = delete; \
    Class &operator=(const Class &) = delete;

#define DISABLE_MOVE(Class) \
    Class(Class &&) = delete; \
    Class &operator=(Class &&) = delete;

#define DISABLE_COPY_MOVE(Class) \
    DISABLE_COPY(Class) \
    DISABLE_MOVE(Class)

class noncopyable
{
public:
    noncopyable(const noncopyable &) = delete;
    void operator=(const noncopyable &) = delete;

protected:
    noncopyable() = default;
    ~noncopyable() = default;
};

class copyable
{
public:
    copyable(const copyable &) = default;
    auto operator=(const copyable &) -> copyable & = default;

protected:
    copyable() = default;
    ~copyable() = default;
};

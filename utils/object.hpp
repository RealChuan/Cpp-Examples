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

#define DEFAULT_COPY(Class) \
    Class(const Class &) = default; \
    Class &operator=(const Class &) = default;

#define DEFAULT_MOVE(Class) \
    Class(Class &&) = default; \
    Class &operator=(Class &&) = default;

#define DEFAULT_COPY_MOVE(Class) \
    DEFAULT_COPY(Class) \
    DEFAULT_MOVE(Class)

#define DEFAULT_CTORS(Class) \
    Class() = default; \
    ~Class() = default;

class noncopyable
{
public:
    DEFAULT_CTORS(noncopyable)
    DISABLE_COPY_MOVE(noncopyable)
};

class copyable
{
public:
    DEFAULT_CTORS(copyable)
    DEFAULT_COPY_MOVE(copyable)
};

class nonmovable
{
public:
    DEFAULT_CTORS(nonmovable)
    DEFAULT_COPY(nonmovable)
    DISABLE_MOVE(nonmovable)
};

class movable
{
public:
    DEFAULT_CTORS(movable)
    DISABLE_COPY(movable)
    DEFAULT_MOVE(movable)
};

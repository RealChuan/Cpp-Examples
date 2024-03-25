#pragma once

#include "object.hpp"

#include <type_traits>
#include <utility>

template<typename F>
class ScopeGuard : noncopyable
{
public:
    explicit ScopeGuard(F &&f) noexcept
        : m_func(std::move(f))
    {}

    explicit ScopeGuard(const F &f) noexcept
        : m_func(f)
    {}

    ScopeGuard(ScopeGuard &&other) noexcept
        : m_func(std::move(other.m_func))
        , m_invoke(std::exchange(other.m_invoke, false))
    {}

    ~ScopeGuard() noexcept
    {
        if (m_invoke) {
            m_func();
        }
    }

    void dismiss() noexcept { m_invoke = false; }

private:
    F m_func;
    bool m_invoke = true;
};

template<typename F>
ScopeGuard(F (&)()) -> ScopeGuard<F (*)()>;

template<typename F>
[[nodiscard]] auto scopeGuard(F &&f) -> ScopeGuard<std::decay_t<F>>
{
    return ScopeGuard<std::decay_t<F>>(std::forward<F>(f));
}
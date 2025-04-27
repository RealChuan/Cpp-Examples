#pragma once

#include "object.hpp"

#include <type_traits>
#include <utility>

template<typename F>
class ScopeGuard
{
    DISABLE_COPY(ScopeGuard)
public:
    ScopeGuard() = delete;

    explicit ScopeGuard(F &&func) noexcept
        : m_func(std::move(func))
    {}

    explicit ScopeGuard(const F &func) noexcept
        : m_func(func)
    {}

    ScopeGuard(ScopeGuard &&other) noexcept
        : m_func(std::move(other.m_func))
        , m_invoke(std::exchange(other.m_invoke, false))
    {}

    ScopeGuard &operator=(ScopeGuard &&other) noexcept
    {
        if (this != &other) {
            m_func = std::move(other.m_func);
            m_invoke = std::exchange(other.m_invoke, false);
        }
    }

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
[[nodiscard]] auto scopeGuard(F &&func) -> ScopeGuard<std::decay_t<F>>
{
    return ScopeGuard<std::decay_t<F>>(std::forward<F>(func));
}
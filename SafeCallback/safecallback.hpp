#pragma once

#include <functional>
#include <iostream>
#include <memory>
#include <utility>

template<typename CLASS, typename... ARGS>
class SafeCallback
{
public:
    SafeCallback(const std::weak_ptr<CLASS> &object,
                 const std::function<void(CLASS *, ARGS...)> &function)
        : m_object(object)
        , m_function(function)
    {}

    template<typename... CallARGS>
    void operator()(CallARGS &&...args) const
    {
        if (auto ptr = m_object.lock()) {
            m_function(ptr.get(), std::forward<CallARGS>(args)...);
        } else {
            std::cout << "Callback object has been destroyed." << std::endl;
        }
    }

private:
    std::weak_ptr<CLASS> m_object;
    std::function<void(CLASS *, ARGS...)> m_function;
};

template<typename CLASS, typename... ARGS>
SafeCallback<CLASS, ARGS...> makeSafeCallback(const std::shared_ptr<CLASS> &object,
                                              void (CLASS::*function)(ARGS...))
{
    return SafeCallback<CLASS, ARGS...>(object, function);
}

template<typename CLASS, typename... ARGS>
SafeCallback<CLASS, ARGS...> makeSafeCallback(const std::shared_ptr<CLASS> &object,
                                              void (CLASS::*function)(ARGS...) const)
{
    return SafeCallback<CLASS, ARGS...>(object, function);
}

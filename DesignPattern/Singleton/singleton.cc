#include "singleton.hpp"

namespace Hungry {

// ② This the second way to implement Singleton
// std::unique_ptr<Singleton> Singleton::s_singleton_ptr(new Singleton);
std::unique_ptr<Singleton> Singleton::s_singleton_ptr;
std::mutex Singleton::s_mutex;

} // namespace Hungry

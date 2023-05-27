#include "singleton.hpp"
#include "singletonmanager.hpp"

class Test
{
public:
    Test() { std::cout << "Test" << std::endl; }
    ~Test() { std::cout << "~Test" << std::endl; }

    void doSomeThing() { std::cout << "doSomeThing" << std::endl; }
};

int main(int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    Lazy::Singleton::instance();
    Lazy::Singleton::instance();

    Hungry::Singleton::instance();
    Hungry::Singleton::instance();

    LazyTemplate::Singleton<Test>::instance().doSomeThing();
    LazyTemplate::Singleton<Test>::instance().doSomeThing();

    SingletonManager::instance();
    SingletonManager::instance();

    return 0;
}

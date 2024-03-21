#include <functional>
#include <iostream>

#include "mutex.hpp"

class TestMutex
{
    Mutex m_mutex;
    int count = 0;

public:
    void run()
    {
        while (count < 30) {
            MutexLocker locker(&m_mutex);
            std::cout << std::this_thread::get_id() << ": " << ++count << '\n';
        }
    }
};

auto main(int argc, char *argv[]) -> int
{
    (void) argc;
    (void) argv;

    TestMutex test;
    std::thread t1([ObjectPtr = &test] { ObjectPtr->run(); });
    std::thread t2([ObjectPtr = &test] { ObjectPtr->run(); });
    t1.join();
    t2.join();

    return 0;
}

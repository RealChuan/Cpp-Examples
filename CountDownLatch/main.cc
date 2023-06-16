#include "countdownlatch.hpp"

#include <iostream>
#include <thread>

auto main() -> int
{
    CountDownLatch latch(1);
    std::thread thread([&]() {
        std::cout << "thread start" << std::endl;
        latch.wait();
        std::cout << "thread end" << std::endl;
    });
    std::cout << "main start" << std::endl;
    latch.countDown();
    thread.join();
    std::cout << "main end" << std::endl;
}

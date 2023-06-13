#include "countdownlatch.hpp"

#include <iostream>
#include <thread>

int main()
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

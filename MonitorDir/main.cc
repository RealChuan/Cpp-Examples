#include "monitordir.hpp"

#include <iostream>
#include <thread>

auto main(int argc, char *argv[]) -> int
{
    MonitorDir monitorDir(".");
    monitorDir.start();

    std::this_thread::sleep_for(std::chrono::seconds(60));

    return 0;
}

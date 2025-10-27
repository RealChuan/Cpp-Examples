#include "monitordir.hpp"

#include <filesystem>
#include <iostream>
#include <thread>

auto main(int argc, char *argv[]) -> int
{
    auto filepath = std::filesystem::current_path();
    std::cout << filepath << std::endl;

    MonitorDir monitorDir(filepath);
    monitorDir.start();

    std::this_thread::sleep_for(std::chrono::seconds(60));

    return 0;
}

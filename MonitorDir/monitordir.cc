#include "monitordir.hpp"

void wakeWatch(const std::string &dir)
{
    auto path = std::filesystem::path(dir) / "monitor_dir_wake";
    std::filesystem::create_directory(path);
    std::filesystem::remove(path);
}

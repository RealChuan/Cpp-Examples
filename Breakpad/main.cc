#include "breakpad.hpp"

#include <filesystem>

void crash()
{
    int *p = nullptr;
    *p = 1;
}

auto main(int argc, char *argv[]) -> int
{
    auto dumpPath = std::filesystem::current_path() / "breakpad";
    if (!std::filesystem::exists(dumpPath)) {
        std::filesystem::create_directory(dumpPath);
    }

    Breakpad breakpad(dumpPath.string());

    crash();

    return 0;
}

#include "crashpad.hpp"

#include <filesystem>

void crash()
{
    int *p = nullptr;
    *p = 1;
}

auto main() -> int
{
    auto libexecPath = std::filesystem::current_path();
    auto dumpPath = libexecPath / "crashpad";
    if (!std::filesystem::exists(dumpPath)) {
        std::filesystem::create_directory(dumpPath);
    }

    Crashpad crashpad(dumpPath.string(), libexecPath.string(), "", true);

    crash();

    return 0;
}

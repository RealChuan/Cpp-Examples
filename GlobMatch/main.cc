#include "globmatcher.hpp"

#include <filesystem>
#include <iostream>

auto main() -> int
{
    auto path = std::filesystem::current_path().parent_path().parent_path();
    std::cout << path << std::endl;
    auto filePath = path / ".gitignore";

    GlobMatcher globMatcher(filePath.string());
    for (const auto &entry : std::filesystem::directory_iterator(path)) {
        auto currentPath = entry.path();
        if (globMatcher.match(currentPath.filename().string())) {
            std::cout << currentPath << " is ignored by .gitignore" << std::endl;
        } else {
            std::cout << currentPath << " is not ignored by .gitignore" << std::endl;
        }
    }

    return 0;
}
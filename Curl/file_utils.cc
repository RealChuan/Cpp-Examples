#include "file_utils.hpp"

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <iostream>

void createFile(const std::string &filename, const std::string &data)
{
    auto filepath = std::filesystem::current_path() / filename;
    std::filesystem::create_directories(filepath.parent_path());
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Cannot open the file: " << filepath << std::endl;
        return;
    }
    file << data;
    file.close();
}

void removeFile(const std::string &filename)
{
    auto filepath = std::filesystem::current_path() / filename;
    if (std::filesystem::exists(filepath)) {
        std::filesystem::remove(filepath);
    }
}

void assertFileData(const std::string &filename, const std::string &data)
{
    auto filepath = std::filesystem::current_path() / filename;
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Cannot open the file: " << filepath << std::endl;
        return;
    }
    std::string fileData((std::istreambuf_iterator<char>(file)), (std::istreambuf_iterator<char>()));
    file.close();
    EXPECT_EQ(fileData, data);
}

auto formatBytes(double value, int precision) -> std::string
{
    std::vector<std::string> units = {"B", "KB", "MB", "GB", "TB", "PB", "EB", "ZB", "YB"};

    int i = 0;
    while (value > 1024) {
        value /= 1024;
        i++;
    }

    std::ostringstream out;
    out.precision(precision);
    out << std::fixed << value;

    return out.str() + " " + units[i];
}

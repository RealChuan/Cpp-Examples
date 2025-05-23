#pragma once

#include <string>

void createFile(const std::string &filename, const std::string &data);

void removeFile(const std::string &filename);

void assertFileData(const std::string &filename, const std::string &data);

auto formatBytes(int64_t value, int precision = 2) -> std::string;

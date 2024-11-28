#include "globmatcher.hpp"

#include <fstream>
#include <iostream>

std::vector<std::string> readGlobPatternsFromFile(const std::string &filename)
{
    std::vector<std::string> patterns;
    std::ifstream file(filename);
    std::string pattern;
    if (file.is_open()) {
        while (std::getline(file, pattern)) {
            patterns.push_back(pattern);
        }
        file.close();
    } else {
        std::cerr << "Unable to open file: " << filename << std::endl;
    }
    return patterns;
}

bool globMatch(const std::string &text, const std::string &pattern)
{
    size_t n = text.size(), m = pattern.size();
    size_t i = 0, j = 0;
    while (i < n && j < m) {
        if (pattern[j] == '*') {
            while (j < m && pattern[j] == '*')
                ++j;
            if (j == m)
                return true;
            while (i < n && !globMatch(text.substr(i), pattern.substr(j))) {
                ++i;
            }
            if (i > 0)
                --i;
        } else if (pattern[j] == '?' || pattern[j] == text[i]) {
            ++i;
            ++j;
        } else {
            return false;
        }
    }
    while (j < m && pattern[j] == '*') {
        ++j;
    }
    return j == m && i == n;
}

GlobMatcher::GlobMatcher(const std::string &patternFile)
{
    m_patterns = readGlobPatternsFromFile(patternFile);
}

bool GlobMatcher::match(const std::string &text)
{
    for (const auto &pattern : m_patterns) {
        if (globMatch(text, pattern)) {
            return true;
        }
    }
    return false;
}

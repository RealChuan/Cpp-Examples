#pragma once

#include <utils/object.hpp>

#include <string>
#include <vector>

class GlobMatcher : noncopyable
{
public:
    explicit GlobMatcher(const std::string &patternFile);

    auto match(const std::string &text) -> bool;

private:
    std::vector<std::string> m_patterns;
};

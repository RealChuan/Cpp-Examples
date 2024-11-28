#pragma once

#include <utils/object.hpp>

#include <string>
#include <vector>

class GlobMatcher : noncopyable
{
public:
    explicit GlobMatcher(const std::string &patternFile);

    bool match(const std::string &text);

private:
    std::vector<std::string> m_patterns;
};

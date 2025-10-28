#pragma once

#include <string>

#define UNUSED(x) (void) (x);

namespace Utils {

#ifdef _WIN32
std::wstring toWide(const std::string &str);
#endif

} // namespace Utils

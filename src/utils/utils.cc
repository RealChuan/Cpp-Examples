#include "utils.hpp"

// clang-format off
#ifdef _WIN32
#include <windows.h>
#include <stringapiset.h>
#endif
// clang-format on

namespace Utils {

#ifdef _WIN32

std::wstring toWide(const std::string &str)
{
    if (str.empty())
        return {};

    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int) str.size(), nullptr, 0);
    if (size_needed == 0)
        return {};

    std::wstring wstr(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), (int) str.size(), &wstr[0], size_needed);
    return wstr;
}

#endif

} // namespace Utils

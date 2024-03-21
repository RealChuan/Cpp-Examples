#pragma once

#include <algorithm>
#include <iostream>

auto custom_memcpy(void *dest, const void *src, size_t n) -> void *
{
    if (dest == nullptr || src == nullptr || n == 0) {
        return dest;
    }

    char *d = static_cast<char *>(dest);
    const char *s = static_cast<const char *>(src);

    try {
        if (d <= s) {
            // No overlap, copy from start to end
            for (size_t i = 0; i < n; ++i) {
                d[i] = s[i];
            }
        } else {
            // Overlap, copy from end to start
            for (size_t i = n; i > 0; --i) {
                d[i - 1] = s[i - 1];
            }
        }
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << '\n';
        return nullptr;
    } catch (...) {
        std::cerr << "Unknown error" << '\n';
        return nullptr;
    }
    return dest;
}

#include "mymemcpy.hpp"

void *my_memcpy(void *dest, const void *src, size_t n)
{
    // 参数检查
    if (dest == NULL || src == NULL || n == 0) {
        return dest;
    }

    // 类型转换用于逐字节操作
    char *dest_ptr = (char *) dest;
    const char *src_ptr = (const char *) src;

    // 逐字节复制
    for (size_t i = 0; i < n; i++) {
        dest_ptr[i] = src_ptr[i];
    }

    return dest;
}

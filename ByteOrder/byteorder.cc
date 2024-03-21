#include <iostream>

// 大端：高位字节存放在内存的低地址，低位字节存放在内存的高地址
// 0x12345678, 大端为 0x12 0x34 0x56 0x78, 小端为 0x78 0x56 0x34 0x12

auto main() -> int
{
    int i = 1;
    char c = *reinterpret_cast<char *>(&i);
    if (c == 1) {
        std::cout << "Little Endian" << '\n';
    } else {
        std::cout << "Big Endian" << '\n';
    }

    return 0;
}
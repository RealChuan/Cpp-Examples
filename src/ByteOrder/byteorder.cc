#include <cstdint>
#include <cstring>
#include <iostream>

#ifdef __cpp_lib_endian
#include <bit>
#endif

// 方法1: 联合体方式 - 使用联合体进行类型双关
bool is_little_endian_union()
{
    union {
        uint32_t i;
        uint8_t c[4];
    } test = {0x01020304};

    return test.c[0] == 0x04; // 小端: 低地址存低位字节
}

// 方法2: 位运算方式 - 通过位掩码检查最低有效字节
bool is_little_endian_bitwise()
{
    uint32_t value = 0x00000001;
    return (value & 0xFF) == 0x01;
}

// 方法3: C++20 标准库方式 - 使用标准库提供的字节序检测
#ifdef __cpp_lib_endian
bool is_little_endian_std()
{
    return std::endian::native == std::endian::little;
}
#endif

// 方法4: 指针方式 - 通过指针访问内存字节
bool is_little_endian_pointer()
{
    uint32_t i = 0x01020304;
    auto *ptr = reinterpret_cast<const uint8_t *>(&i);
    return *ptr == 0x04;
}

// 方法5: 内存拷贝方式 - 使用memcpy避免严格别名问题
bool is_little_endian_memcpy()
{
    uint32_t original = 0x01020304;
    uint8_t bytes[4];
    std::memcpy(bytes, &original, sizeof(original));
    return bytes[0] == 0x04;
}

// 方法6: 网络字节序对比方式 - 对比主机字节序和网络字节序
bool is_little_endian_network()
{
    uint32_t host = 0x01020304;
    // 假设网络字节序是大端
    // 如果主机字节序与网络字节序不同，则是小端
    uint32_t network = (host >> 24) | ((host >> 8) & 0x0000FF00) | ((host << 8) & 0x00FF0000)
                       | (host << 24);

    return host != network;
}

// 方法7: 类型双关方式 - 使用char*访问内存
bool is_little_endian_type_punning()
{
    uint32_t value = 0x01020304;
    const char *as_chars = reinterpret_cast<const char *>(&value);
    return static_cast<uint8_t>(as_chars[0]) == 0x04;
}

// 测试所有方法
void test_all_methods()
{
    std::cout << "Endianness Detection Methods Comparison:\n";
    std::cout << "========================================\n";

    std::cout << "1. Union method: " << (is_little_endian_union() ? "Little Endian" : "Big Endian")
              << '\n';
    std::cout << "2. Bitwise method: "
              << (is_little_endian_bitwise() ? "Little Endian" : "Big Endian") << '\n';

#ifdef __cpp_lib_endian
    std::cout << "3. C++20 Standard Library: "
              << (is_little_endian_std() ? "Little Endian" : "Big Endian") << '\n';
#endif

    std::cout << "4. Pointer method: "
              << (is_little_endian_pointer() ? "Little Endian" : "Big Endian") << '\n';
    std::cout << "5. Memory copy method: "
              << (is_little_endian_memcpy() ? "Little Endian" : "Big Endian") << '\n';
    std::cout << "6. Network byte order comparison: "
              << (is_little_endian_network() ? "Little Endian" : "Big Endian") << '\n';
    std::cout << "7. Type punning method: "
              << (is_little_endian_type_punning() ? "Little Endian" : "Big Endian") << '\n';
}

// 推荐的生产环境使用方法
bool detect_endianness()
{
    // 优先使用C++20标准库方法（如果可用）
#ifdef __cpp_lib_endian
    return std::endian::native == std::endian::little;
#else
    // 回退到联合体方法
    return is_little_endian_union();
#endif
}

auto main() -> int
{
    test_all_methods();

    std::cout << "\nRecommended Method Result:\n";
    std::cout << "==========================\n";
    std::cout << "System is: " << (detect_endianness() ? "Little Endian" : "Big Endian") << '\n';

    return 0;
}

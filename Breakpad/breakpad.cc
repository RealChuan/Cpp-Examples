#include "breakpad.hpp"

#ifdef _WIN32
#include <client/windows/handler/exception_handler.h>
#elif __APPLE__
#include <client/mac/handler/exception_handler.h>
#elif __linux__
#include <client/linux/handler/exception_handler.h>
#endif

#include <codecvt>
#include <iostream>
#include <locale>

#ifdef _WIN32

auto convertWideStringToUTF8(const wchar_t *wstr) -> std::string
{
    if (wstr == nullptr) {
        return {};
    }

    // 首先，获取转换后的字符串长度（不包括空终止符）
    int len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, nullptr, 0, nullptr, nullptr);

    // 如果转换失败，返回空字符串
    if (len == 0) {
        return {};
    }

    // 分配足够的空间来存储转换后的字符串
    std::string utf8String(len, 0);

    // 执行转换
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, &utf8String[0], len, nullptr, nullptr);

    // 去除末尾的空字符
    utf8String.resize(len - 1);
    return utf8String;
}

auto callback(const wchar_t *dump_path,
              const wchar_t *id,
              void *context,
              EXCEPTION_POINTERS *exinfo,
              MDRawAssertionInfo *assertion,
              bool succeeded) -> bool
{
    const auto *succeeded_str = succeeded ? "succeeded" : "fialed";
    auto dump_path_str = convertWideStringToUTF8(dump_path) + convertWideStringToUTF8(id);
    std::cout << "Create dump file " << succeeded_str << " Dump path: " << dump_path_str << '\n';
    return succeeded;
}
#elif __APPLE__
bool callback(const char *dump_path, const char *id, void *context, bool succeeded)
{
    auto succeeded_str = succeeded ? "succeeded" : "fialed";
    std::cout << "Create dump file " << succeeded_str << " Dump path: " << dump_path << std::endl;
    return succeeded;
}
#elif __linux__
bool callback(const google_breakpad::MinidumpDescriptor &descriptor, void *context, bool succeeded)
{
    auto succeeded_str = succeeded ? "succeeded" : "fialed";
    std::cout << "Create dump file " << succeeded_str << " Dump path: " << descriptor.path()
              << std::endl;
    return succeeded;
}
#endif

Breakpad::Breakpad(const std::string &dump_path)
{
#ifdef _WIN32
    auto dump_path_w = std::wstring(dump_path.begin(), dump_path.end());
    handler_ptr = std::make_unique<google_breakpad::ExceptionHandler>(
        dump_path_w, nullptr, callback, nullptr, google_breakpad::ExceptionHandler::HANDLER_ALL);
#elif __APPLE__
    handler_ptr = std::make_unique<google_breakpad::ExceptionHandler>(dump_path,
                                                                      nullptr,
                                                                      callback,
                                                                      nullptr,
                                                                      true,
                                                                      nullptr);
#elif __linux__
    handler_ptr = std::make_unique<google_breakpad::ExceptionHandler>(
        google_breakpad::MinidumpDescriptor(dump_path), nullptr, callback, nullptr, true, -1);
#endif
}

Breakpad::~Breakpad() = default;

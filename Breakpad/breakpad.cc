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
bool callback(const wchar_t *dump_path,
              const wchar_t *id,
              void *context,
              EXCEPTION_POINTERS *exinfo,
              MDRawAssertionInfo *assertion,
              bool succeeded)
{
    auto succeeded_str = succeeded ? "succeeded" : "fialed";
    auto convert_str = [](const wchar_t *wstr) {
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        return converter.to_bytes(wstr);
    };
    auto dump_path_str = convert_str(dump_path) + convert_str(id);
    std::cout << "Create dump file " << succeeded_str << " Dump path: " << dump_path_str
              << std::endl;
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

Breakpad::~Breakpad() {}
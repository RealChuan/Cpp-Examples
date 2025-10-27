#include "crashpad.hpp"

#include <crashpad/client/crash_report_database.h>
#include <crashpad/client/crashpad_client.h>
#include <crashpad/client/settings.h>

#ifdef _WIN32
auto convertStringToWideString(const std::string &str) -> std::wstring
{
    if (str.empty()) {
        return {};
    }

    // 首先，获取转换后的字符串长度（不包括空终止符）
    int len = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, nullptr, 0);

    // 如果转换失败，返回空字符串
    if (len == 0) {
        return {};
    }

    // 分配足够的空间来存储转换后的字符串
    std::wstring wstr(len, 0);

    // 执行转换
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], len);

    // 去除末尾的空字符
    wstr.resize(len - 1);
    return wstr;
}
#endif

Crashpad::Crashpad(const std::string &dumpPath,
                   const std::string &libexecPath,
                   const std::string &reportUrl,
                   bool crashReportingEnabled)
{
    auto handlerPath = libexecPath + "/crashpad_handler";
#ifdef _WIN32
    handlerPath += ".exe";
    base::FilePath database(convertStringToWideString(dumpPath));
    base::FilePath handler(convertStringToWideString(handlerPath));
#else
    base::FilePath database(dumpPath);
    base::FilePath handler(handlerPath);
#endif
#ifdef __linux__
    bool asynchronous_start = false;
#else
    bool asynchronous_start = true;
#endif

    auto dbPtr = crashpad::CrashReportDatabase::Initialize(database);
    if (dbPtr && (dbPtr->GetSettings() != nullptr)) {
        dbPtr->GetSettings()->SetUploadsEnabled(crashReportingEnabled);
    }

    m_crashpadClientPtr = std::make_unique<crashpad::CrashpadClient>();
    m_crashpadClientPtr->StartHandler(handler,
                                      database,
                                      database,
                                      reportUrl,
                                      {},
                                      {"--no-rate-limit"},
                                      true,
                                      asynchronous_start);
}

Crashpad::~Crashpad() = default;

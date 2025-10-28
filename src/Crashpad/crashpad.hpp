#pragma once

#include <utils/object.hpp>

#include <memory>
#include <string>

class Crashpad : noncopyable
{
public:
    explicit Crashpad(const std::string &dumpPath,
                      const std::string &libexecPath,
                      const std::string &reportUrl,
                      bool crashReportingEnabled);

    ~Crashpad();

    std::string getDumpPath() const;
    std::string getReportUrl() const;
    bool isReportingEnabled() const;

private:
    class CrashpadPrivate;
    std::unique_ptr<CrashpadPrivate> d_ptr;
};

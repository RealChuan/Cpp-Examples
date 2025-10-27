#pragma once

#include <utils/object.hpp>

#include <memory>
#include <string>

namespace crashpad {
class CrashpadClient;
} // namespace crashpad

class Crashpad : noncopyable
{
public:
    explicit Crashpad(const std::string &dumpPath,
                      const std::string &libexecPath,
                      const std::string &reportUrl,
                      bool crashReportingEnabled);
    ~Crashpad();

private:
    std::unique_ptr<crashpad::CrashpadClient> m_crashpadClientPtr;
};

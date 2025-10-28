#pragma once

#include <utils/object.hpp>

#include <functional>
#include <memory>
#include <string>

class BreakpadPrivate;
class Breakpad : noncopyable
{
public:
    using CrashCallback = std::function<bool(const std::string &dump_path, bool succeeded)>;

    explicit Breakpad(const std::string &dump_path, int timeout_ms = 5000);
    ~Breakpad();

    void setCrashCallback(CrashCallback callback);

    bool writeMinidump();

    std::string getDumpPath() const;
    int getTimeoutMs() const;

private:
    std::unique_ptr<BreakpadPrivate> d_ptr;
};

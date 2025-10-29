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

    explicit Breakpad(const std::string &dump_path);
    ~Breakpad();

    void setCrashCallback(CrashCallback callback);

    bool writeMinidump();

    std::string getDumpPath() const;

private:
    std::unique_ptr<BreakpadPrivate> d_ptr;
};

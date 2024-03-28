#pragma once

#include <utils/object.hpp>

#include <memory>
#include <string>

namespace google_breakpad {
class ExceptionHandler;
}

class Breakpad : noncopyable
{
public:
    explicit Breakpad(const std::string &dump_path);
    ~Breakpad();

private:
    std::unique_ptr<google_breakpad::ExceptionHandler> handler_ptr;
};

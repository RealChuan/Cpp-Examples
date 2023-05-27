#pragma once

#include <memory>
#include <string>

namespace google_breakpad {
class ExceptionHandler;
}

class Breakpad
{
public:
    Breakpad(const std::string &dump_path);
    ~Breakpad();

private:
    std::unique_ptr<google_breakpad::ExceptionHandler> handler_ptr;
};

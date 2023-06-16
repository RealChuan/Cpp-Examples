#include "breakpad.hpp"

auto main(int argc, char *argv[]) -> int
{
    Breakpad breakpad("./");
    int *p = nullptr;
    *p = 1;

    return 0;
}

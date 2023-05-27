#include "breakpad.hpp"

int main(int argc, char *argv[])
{
    Breakpad breakpad("./");
    int *p = nullptr;
    *p = 1;

    return 0;
}

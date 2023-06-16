#include "monitoringdirectory.hpp"

auto main(int argc, char *argv[]) -> int
{
    MonitoringDirectory monitoringDirectory(".");
    monitoringDirectory.start();

    return 0;
}

#include "monitoringdirectory.hpp"

int main(int argc, char *argv[])
{
    MonitoringDirectory monitoringDirectory(".");
    monitoringDirectory.start();

    return 0;
}

#include "crashpad.hpp"

#include <chrono>
#include <filesystem>
#include <iostream>
#include <thread>

void crash()
{
    std::cout << "About to cause a segmentation fault..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    int *p = nullptr;
    *p = 1; // 这会导致真正的段错误
}

// 另一种崩溃方式：除零错误
void crash_divide_by_zero()
{
    std::cout << "About to cause a divide by zero..." << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    int a = 1;
    int b = 0;
    int c = a / b;                             // 除零错误
    std::cout << "Result: " << c << std::endl; // 这行不会执行
}

// 模拟工作负载
void normal_work(int iteration)
{
    std::cout << "Working on iteration: " << iteration << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
}

int main(int argc, char *argv[])
{
    std::cout << "Crashpad Crash Demonstration" << std::endl;
    std::cout << "============================" << std::endl;

    try {
        auto dumpPath = std::filesystem::current_path() / "crash_dumps";
        auto libexecPath = std::filesystem::current_path(); // crashpad_handler 所在目录
        std::string reportUrl = "http://127.0.0.1:8000/upload";
        bool enableReporting = true;

        // 初始化 Crashpad
        Crashpad crashpad(dumpPath.string(), libexecPath.string(), reportUrl, enableReporting);

        std::cout << "Crashpad initialized successfully" << std::endl;
        std::cout << "Report URL: " << crashpad.getReportUrl() << std::endl;

        // 解析命令行参数
        bool should_crash = false;
        int crash_type = 1; // 1: null pointer, 2: divide by zero

        if (argc > 1) {
            std::string arg = argv[1];
            if (arg == "--crash" || arg == "-c") {
                should_crash = true;
                if (argc > 2) {
                    crash_type = std::atoi(argv[2]);
                }
            } else if (arg == "--help" || arg == "-h") {
                std::cout << "\nUsage:" << std::endl;
                std::cout << "  " << argv[0] << "                    # Run without crashing"
                          << std::endl;
                std::cout << "  " << argv[0]
                          << " --crash           # Crash with null pointer (default)" << std::endl;
                std::cout << "  " << argv[0] << " --crash 1        # Crash with null pointer"
                          << std::endl;
                std::cout << "  " << argv[0] << " --crash 2        # Crash with divide by zero"
                          << std::endl;
                std::cout << "  " << argv[0] << " --help           # Show this help" << std::endl;
                return 0;
            }
        }

        // 模拟正常工作
        std::cout << "\nStarting normal work..." << std::endl;
        for (int i = 1; i <= 5; ++i) {
            normal_work(i);
        }

        // 根据命令行参数决定是否崩溃
        if (should_crash) {
            std::cout << "\n*** INTENTIONALLY CAUSING CRASH ***" << std::endl;
            std::cout << "Crash type: " << crash_type << std::endl;

            if (crash_type == 2) {
                crash_divide_by_zero();
            } else {
                crash(); // 默认崩溃方式
            }

            // 这行代码不会执行
            std::cout << "This message will not be printed due to crash" << std::endl;
        } else {
            std::cout << "\nWork completed successfully - no crash was triggered" << std::endl;
            std::cout << "To test crash handling, run with: " << argv[0] << " --crash" << std::endl;
        }

    } catch (const std::exception &e) {
        std::cerr << "Failed to initialize Crashpad: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

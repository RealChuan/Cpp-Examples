#include "breakpad.hpp"

#include <chrono>
#include <cstdlib>
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

    // 在第3次迭代时手动保存状态
    if (iteration == 3) {
        std::cout << "Saving checkpoint minidump..." << std::endl;
    }
}

int main(int argc, char *argv[])
{
    std::cout << "Breakpad Crash Demonstration" << std::endl;
    std::cout << "============================" << std::endl;

    try {
        auto dumpPath = std::filesystem::current_path() / "crash_dumps";
        Breakpad breakpad(dumpPath.string());

        // 设置详细的崩溃回调
        breakpad.setCrashCallback([](const std::string &dump_path, bool success) {
            std::cout << "\n*** CRASH DETECTED ***" << std::endl;
            std::cout << "Minidump file: " << dump_path << std::endl;
            std::cout << "Generation: " << (success ? "SUCCESSFUL" : "FAILED") << std::endl;
            std::cout << "*** END CRASH REPORT ***" << std::endl;

            // 在实际应用中，这里可以：
            // 1. 上传崩溃报告到服务器
            // 2. 记录额外的诊断信息
            // 3. 发送通知
            // 4. 尝试优雅关闭资源

            return true;
        });

        std::cout << "Breakpad initialized successfully" << std::endl;
        std::cout << "Dump path: " << breakpad.getDumpPath() << std::endl;

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

        // 在程序启动时保存初始状态
        std::cout << "\nSaving initial minidump..." << std::endl;
        if (breakpad.writeMinidump()) {
            std::cout << "Initial minidump saved successfully" << std::endl;
        }

        // 模拟正常工作
        std::cout << "\nStarting normal work..." << std::endl;
        for (int i = 1; i <= 5; ++i) {
            normal_work(i);

            // 保存检查点
            if (i == 3) {
                std::cout << "Saving checkpoint minidump..." << std::endl;
                breakpad.writeMinidump();
            }
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
        std::cerr << "Failed to initialize Breakpad: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}

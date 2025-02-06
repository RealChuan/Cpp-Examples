#include <glog/logging.h>

#include <filesystem>
#include <format>

auto main(int argc, char **argv) -> int
{
    (void) argc;

    const auto log_dir = std::filesystem::current_path() / "glog";

    // Initialize Google’s logging library.
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();
    google::SetLogFilenameExtension(".log");
    google::EnableLogCleaner(std::chrono::hours(24 * 7));
    //google::DisableLogCleaner();
    FLAGS_alsologtostderr = true;  // 是否将日志输出到文件和stderr
    FLAGS_colorlogtostderr = true; // 是否启用不同颜色显示
    FLAGS_max_log_size = 1000;     // 最大日志文件大小
    fLS::FLAGS_log_dir = log_dir.string();

    std::filesystem::create_directories(log_dir);

    std::string message("Hello World");

    LOG(INFO) << std::format("{}, This is a info message", message);
    LOG(WARNING) << std::format("{}, This is a warning message", message);
    LOG(ERROR) << std::format("{}, This is a error message", message);
    // LOG(FATAL) << std::format("{}, This is a fatal message", message);

    DLOG(INFO) << std::format("{}, This is a debug info message", message);
    DLOG(WARNING) << std::format("{}, This is a debug warning message", message);
    DLOG(ERROR) << std::format("{}, This is a debug error message", message);
    // DLOG(FATAL) << std::format("{}, This is a debug fatal message", message);

    google::ShutdownGoogleLogging();
    return 0;
}

#include <glog/logging.h>

#include <filesystem>

auto main(int argc, char **argv) -> int
{
    (void) argc;

    const auto *log_dir = "./glog_demo";

    // Initialize Google’s logging library.
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();
    google::SetLogFilenameExtension(".log");
    google::EnableLogCleaner(7);
    //google::DisableLogCleaner();
    FLAGS_alsologtostderr = true;  // 是否将日志输出到文件和stderr
    FLAGS_colorlogtostderr = true; // 是否启用不同颜色显示
    FLAGS_max_log_size = 1000;     // 最大日志文件大小
    fLS::FLAGS_log_dir = log_dir;

    std::filesystem::create_directories(log_dir);

    std::string message("Hello World");

    LOG(INFO) << message;
    LOG(WARNING) << message;
    LOG(ERROR) << message;
    // LOG(FATAL) << message;

    DLOG(INFO) << message;
    DLOG(WARNING) << message;
    DLOG(ERROR) << message;
    // DLOG(FATAL) << message;

    google::ShutdownGoogleLogging();
    return 0;
}

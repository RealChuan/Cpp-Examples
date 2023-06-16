#include <glog/logging.h>

#include <filesystem>

auto main(int argc, char **argv) -> int
{
    (void) argc;
    (void) argv;

    // Initialize Google’s logging library.
    google::InitGoogleLogging(argv[0]);
    google::InstallFailureSignalHandler();
    google::SetLogFilenameExtension(".log");
    google::EnableLogCleaner(3);
    //google::DisableLogCleaner();
    FLAGS_alsologtostderr = true;  //是否将日志输出到文件和stderr
    FLAGS_colorlogtostderr = true; //是否启用不同颜色显示
    fLS::FLAGS_log_dir = "./Log";

    std::filesystem::create_directories("./Log");

    LOG(INFO) << "INFO_LOG Hello, world!";
    LOG(WARNING) << "WARNING_LOG Hello, world!";
    LOG(ERROR) << "ERROR_LOG Hello, world!";
    //LOG(FATAL) << "FATAL_LOG Hello, world!";

    DLOG(INFO) << "DINFO_LOG Hello, world!";
    DLOG(WARNING) << "DWARNING_LOG Hello, world!";
    DLOG(ERROR) << "DERROR_LOG Hello, world!";
    //DLOG(FATAL) << "DFATAL_LOG Hello, world!";

    google::ShutdownGoogleLogging();
    return 0;
}

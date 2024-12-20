#include "monitordir.hpp"

#include <cassert>
#include <climits>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <string>
#include <sys/fanotify.h>
#include <sys/stat.h>
#include <thread>
#include <unistd.h>

#define BUF_SIZE 256

std::string getPathFromFd(int fd)
{
    char path[PATH_MAX];
    char procfd_path[PATH_MAX];
    snprintf(procfd_path, sizeof(procfd_path), "/proc/self/fd/%d", fd);
    ssize_t len = readlink(procfd_path, path, sizeof(path) - 1);
    if (len >= 0) {
        path[len] = '\0'; // 确保字符串以空字符结尾
        return std::string(path);
    }
    return "Unknown"; // 读取失败时的处理
}

std::string maskToString(unsigned long long mask)
{
    if ((mask & FAN_ACCESS) != 0U) {
        return "FAN_ACCESS: ";
    }
    if ((mask & FAN_MODIFY) != 0U) {
        return "FAN_MODIFY: ";
    }
    if ((mask & FAN_ATTRIB) != 0U) {
        return "FAN_ATTRIB: ";
    }
    if ((mask & FAN_CLOSE_WRITE) != 0U) {
        return "FAN_CLOSE_WRITE: ";
    }
    if ((mask & FAN_CLOSE_NOWRITE) != 0U) {
        return "FAN_CLOSE_NOWRITE: ";
    }
    if ((mask & FAN_OPEN) != 0U) {
        return "FAN_OPEN: ";
    }
    if ((mask & FAN_MOVED_FROM) != 0U) {
        return "FAN_MOVED_FROM: ";
    }
    if ((mask & FAN_MOVED_TO) != 0U) {
        return "FAN_MOVED_TO: ";
    }
    if ((mask & FAN_CREATE) != 0U) {
        return "FAN_CREATE: ";
    }
    if ((mask & FAN_DELETE) != 0U) {
        return "FAN_DELETE: ";
    }
    if ((mask & FAN_DELETE_SELF) != 0U) {
        return "FAN_DELETE_SELF: ";
    }
    if ((mask & FAN_MOVE_SELF) != 0U) {
        return "FAN_MOVE_SELF: ";
    }
    if ((mask & FAN_OPEN_EXEC) != 0U) {
        return "FAN_OPEN_EXEC: ";
    }
    if ((mask & FAN_Q_OVERFLOW) != 0U) {
        return "FAN_Q_OVERFLOW: ";
    }
    if ((mask & FAN_FS_ERROR) != 0U) {
        return "FAN_FS_ERROR: ";
    }
    if ((mask & FAN_OPEN_PERM) != 0U) {
        return "FAN_OPEN_PERM: ";
    }
    if ((mask & FAN_ACCESS_PERM) != 0U) {
        return "FAN_ACCESS_PERM: ";
    }
    if ((mask & FAN_OPEN_EXEC_PERM) != 0U) {
        return "FAN_OPEN_EXEC_PERM: ";
    }
    if ((mask & FAN_EVENT_ON_CHILD) != 0U) {
        return "FAN_EVENT_ON_CHILD: ";
    }
    if ((mask & FAN_RENAME) != 0U) {
        return "FAN_RENAME: ";
    }
    if ((mask & FAN_ONDIR) != 0U) {
        return "FAN_ONDIR: ";
    }
    if ((mask & FAN_CLOSE) != 0U) {
        return "FAN_CLOSE: ";
    }
    if ((mask & FAN_MOVE) != 0U) {
        return "FAN_MOVE: ";
    }
    return "Unknown mask " + std::to_string(mask) + ": ";
}

class MonitorDir::MonitorDirPrivate
{
public:
    explicit MonitorDirPrivate(MonitorDir *q)
        : q_ptr(q)
    {}

    ~MonitorDirPrivate() = default;

    auto createFd() -> bool
    {
        mountFd = open(dir.c_str(), O_DIRECTORY | O_RDONLY);
        if (mountFd == -1) {
            perror("open");
            exit(EXIT_FAILURE);
        }
        std::cout << "mountFd: " << mountFd << std::endl;

        unsigned int flags = FAN_CLASS_NOTIF | FAN_CLOEXEC | FAN_UNLIMITED_QUEUE
                             | FAN_UNLIMITED_MARKS | FAN_ENABLE_AUDIT | FAN_REPORT_FID
                             | FAN_REPORT_DFID_NAME;
        unsigned int event_f_flags = O_RDONLY | O_LARGEFILE | O_CLOEXEC;
        // 普通用户级别flags
        // flags = FAN_CLASS_NOTIF | FAN_CLOEXEC | FAN_REPORT_FID | FAN_REPORT_DFID_NAME;
        fanotifyFd = fanotify_init(flags, event_f_flags);
        if (fanotifyFd == -1) {
            perror("fanotify_init");
            exit(EXIT_FAILURE);
        }
        std::cout << "fanotifyFd: " << fanotifyFd << std::endl;
        return true;
    }

    void closeFd()
    {
        if (mountFd != -1) {
            close(mountFd);
            mountFd = -1;
        }
        if (fanotifyFd != -1) {
            close(fanotifyFd);
            fanotifyFd = -1;
        }
    }

    auto addWatch() -> bool
    {
        // FAN_MARK_FILESYSTEM 全局监控，需要 CAP_SYS_ADMIN 能力，即 root 权限
        unsigned int flags = FAN_MARK_ADD | FAN_MARK_FILESYSTEM;
        unsigned int mask = FAN_ACCESS | FAN_MODIFY | FAN_ATTRIB | FAN_CLOSE_WRITE | FAN_MOVED_FROM
                            | FAN_MOVED_TO | FAN_CREATE | FAN_DELETE | FAN_DELETE_SELF
                            | FAN_MOVE_SELF | FAN_OPEN_EXEC | FAN_EVENT_ON_CHILD | FAN_RENAME
                            | FAN_ONDIR | FAN_MOVE;
        if (fanotify_mark(fanotifyFd, flags, mask, AT_FDCWD, dir.c_str()) == -1) {
            perror("fanotify_mark");
            exit(EXIT_FAILURE);
        }
        std::cout << "add watch: " << dir << std::endl;

        return true;
    }

    void monitor()
    {
        char buf[BUF_SIZE];
        auto len = read(fanotifyFd, buf, sizeof(buf));
        if (!isRunning.load()) {
            return;
        }
        if (len <= 0) {
            std::cerr << "read failed" << std::endl;
            return;
        }

        fanotify_event_metadata *event = nullptr;
        for (event = reinterpret_cast<struct fanotify_event_metadata *>(buf);
             FAN_EVENT_OK(event, len);
             event = FAN_EVENT_NEXT(event, len)) {
            if (!isRunning.load()) {
                return; // 全局监控，会产生大量事件，需要及时处理退出事件
            }

            const auto *fid = reinterpret_cast<const struct fanotify_event_info_fid *>(event + 1);
            auto *file_handle = (struct file_handle *) fid->handle;

            std::string path;
            auto event_fd = open_by_handle_at(mountFd, file_handle, O_RDONLY);
            if (event_fd >= 0) {
                path = getPathFromFd(event_fd);
                close(event_fd);
                if (path.find(dir.string()) == std::string::npos) {
                    continue; // 全局监控，不在目标目录下的文件不处理
                }
            } else {
                if (errno == ESTALE) {
                    std::cerr << "File handle is no longer valid. File has been deleted"
                              << std::endl;
                    continue;
                }
                perror("open_by_handle_at");
                // exit(EXIT_FAILURE);
            }

            std::string filename;
            if (fid->hdr.info_type == FAN_EVENT_INFO_TYPE_DFID_NAME) {
                auto *file_name = file_handle->f_handle + file_handle->handle_bytes;
                filename = std::string(file_name,
                                       file_name
                                           + strlen(reinterpret_cast<const char *>(file_name)));
            } else {
                std::cerr << "Received unexpected event info type: "
                          << std::to_string(fid->hdr.info_type) << std::endl;
            }

            auto fileEvent = maskToString(event->mask) + path;
            if (!filename.empty()) {
                fileEvent += "/" + filename;
            }
            std::cout << fileEvent << std::endl;
        }
    }

    void run()
    {
        if (!createFd()) {
            return;
        }

        if (!addWatch()) {
            closeFd();
            return;
        }
        isRunning.store(true);
        while (isRunning.load()) {
            monitor();
        }

        closeFd();
    }

    MonitorDir *q_ptr;

    int mountFd = -1;
    int fanotifyFd = -1;

    std::filesystem::path dir;
    std::atomic_bool isRunning;
    std::thread monitorThread;
};

MonitorDir::MonitorDir(const std::filesystem::path &dir)
    : d_ptr(std::make_unique<MonitorDirPrivate>(this))
    , m_dir(dir)
    , m_isRunning(false)
{
    assert(std::filesystem::is_directory(dir) && std::filesystem::exists(dir));
    d_ptr->dir = dir;
}

MonitorDir::~MonitorDir()
{
    stop();
}

auto MonitorDir::start() -> bool
{
    if (m_isRunning) {
        std::cerr << "MonitorDir is already running" << std::endl;
        return false;
    }

    m_isRunning.store(true);
    d_ptr->monitorThread = std::thread([this] {
        d_ptr->run();
        m_isRunning.store(false);
    });

    return true;
}

void MonitorDir::stop()
{
    if (!m_isRunning.load()) {
        std::cerr << "MonitorDir is not running" << std::endl;
        return;
    }
    d_ptr->isRunning.store(false);
    if (d_ptr->monitorThread.joinable()) {
        d_ptr->monitorThread.join();
    }
}

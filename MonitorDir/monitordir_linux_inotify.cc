#include "monitordir.hpp"

#include <cassert>
#include <fcntl.h>
#include <iostream>
#include <sys/inotify.h>
#include <thread>
#include <unistd.h>

std::string maskToString(uint32_t mask)
{
    if ((mask & IN_ACCESS) != 0U) {
        return "IN_ACCESS: ";
    }
    if ((mask & IN_MODIFY) != 0U) {
        return "IN_MODIFY: ";
    }
    if ((mask & IN_ATTRIB) != 0U) {
        return "IN_ATTRIB: ";
    }
    if ((mask & IN_CLOSE_WRITE) != 0U) {
        return "IN_CLOSE_WRITE: ";
    }
    if ((mask & IN_CLOSE_NOWRITE) != 0U) {
        return "IN_CLOSE_NOWRITE: ";
    }
    if ((mask & IN_CLOSE) != 0U) {
        return "IN_CLOSE: ";
    }
    if ((mask & IN_OPEN) != 0U) {
        return "IN_OPEN: ";
    }
    if ((mask & IN_MOVED_FROM) != 0U) {
        return "IN_MOVED_FROM: ";
    }
    if ((mask & IN_MOVED_TO) != 0U) {
        return "IN_MOVED_TO: ";
    }
    if ((mask & IN_MOVE) != 0U) {
        return "IN_MOVE: ";
    }
    if ((mask & IN_CREATE) != 0U) {
        return "IN_CREATE: ";
    }
    if ((mask & IN_DELETE) != 0U) {
        return "IN_DELETE: ";
    }
    if ((mask & IN_DELETE_SELF) != 0U) {
        return "IN_DELETE_SELF: ";
    }
    if ((mask & IN_MOVE_SELF) != 0U) {
        return "IN_MOVE_SELF: ";
    }
    if ((mask & IN_UNMOUNT) != 0U) {
        return "IN_UNMOUNT: ";
    }
    if ((mask & IN_Q_OVERFLOW) != 0U) {
        return "IN_Q_OVERFLOW: ";
    }
    if ((mask & IN_IGNORED) != 0U) {
        return "IN_IGNORED: ";
    }
    if ((mask & IN_ISDIR) != 0U) {
        return "IN_ISDIR: ";
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
        // 创建inotify实例
        inotifyFd = inotify_init();
        if (inotifyFd == -1) {
            perror("inotify_init");
            exit(EXIT_FAILURE);
        }

        return true;
    }

    void closeFd()
    {
        if (inotifyFd != -1) {
            close(inotifyFd);
            inotifyFd = -1;
        }
    }

    auto addWatch() -> bool
    {
        // 添加监控目录
        watchFd = inotify_add_watch(inotifyFd, dir.c_str(), IN_ALL_EVENTS);
        if (watchFd == -1) {
            perror("inotify_add_watch");
            exit(EXIT_FAILURE);
        }
        std::cout << "add watch: " << dir << std::endl;
        return true;
    }

    void removeWatch()
    {
        if (watchFd != -1) {
            inotify_rm_watch(inotifyFd, watchFd);
            watchFd = -1;
        }
    }

    void monitor()
    {
        constexpr size_t eventSize = sizeof(struct inotify_event);
        constexpr size_t bufLen = 1024 * (eventSize + 16); // Buffer to hold multiple events
        char buf[bufLen];

        auto len = read(inotifyFd, buf, bufLen);
        if (!isRunning.load()) {
            return;
        }
        if (len == -1) {
            std::cerr << "read failed" << std::endl;
            return;
        }

        for (char *ptr = buf; ptr < buf + len;
             ptr += eventSize + reinterpret_cast<struct inotify_event *>(ptr)->len) {
            auto *event = reinterpret_cast<struct inotify_event *>(ptr);
            std::string fileEvent = maskToString(event->mask);
            if (event->len > 0) {
                fileEvent += event->name;
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

        removeWatch();
        closeFd();
    }

    MonitorDir *q_ptr;

    int inotifyFd = -1;
    int watchFd = -1;

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
    wakeWatch(m_dir.string());
    if (d_ptr->monitorThread.joinable()) {
        d_ptr->monitorThread.join();
    }
}

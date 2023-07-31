#include "monitordir.hpp"

#include <fcntl.h>
#include <iostream>
#include <sys/inotify.h>
#include <thread>
#include <unistd.h>

class MonitorDir::MonitorDirPrivate
{
public:
    explicit MonitorDirPrivate(MonitorDir *q)
        : q_ptr(q)
    {}

    ~MonitorDirPrivate() = default;

    bool createFd()
    {
        // 创建inotify实例
        inotifyFd = inotify_init();
        if (inotifyFd == -1) {
            std::cerr << "inotify_init failed" << std::endl;
            return false;
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

    auto addWatch(const std::string &dir) -> bool
    {
        // 添加监控目录
        watchFd = inotify_add_watch(inotifyFd, dir.c_str(), IN_ALL_EVENTS);
        if (watchFd == -1) {
            std::cerr << "inotify_add_watch failed" << std::endl;
            return false;
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
        char buf[1024];
        struct inotify_event *event;
        auto len = read(inotifyFd, buf, sizeof(buf));
        if (!isRunning.load()) {
            return;
        }
        if (len == -1) {
            std::cerr << "read failed" << std::endl;
            return;
        }

        for (char *ptr = buf; ptr < buf + len; ptr += sizeof(struct inotify_event) + event->len) {
            event = reinterpret_cast<struct inotify_event *>(ptr);
            if ((event->mask & IN_CREATE) != 0U) {
                std::cout << "IN_CREATE: " << event->name << std::endl;
            } else if ((event->mask & IN_DELETE) != 0U) {
                std::cout << "IN_DELETE: " << event->name << std::endl;
            } else if ((event->mask & IN_MODIFY) != 0U) {
                std::cout << "IN_MODIFY: " << event->name << std::endl;
            } else if ((event->mask & IN_MOVED_FROM) != 0U) {
                std::cout << "IN_MOVED_FROM: " << event->name << std::endl;
            } else if ((event->mask & IN_MOVED_TO) != 0U) {
                std::cout << "IN_MOVED_TO: " << event->name << std::endl;
            } else if ((event->mask & IN_CLOSE_WRITE) != 0U) {
                std::cout << "IN_CLOSE_WRITE: " << event->name << std::endl;
            } else if ((event->mask & IN_CLOSE_NOWRITE) != 0U) {
                std::cout << "IN_CLOSE_NOWRITE: " << event->name << std::endl;
            } else if ((event->mask & IN_ACCESS) != 0U) {
                std::cout << "IN_ACCESS: " << event->name << std::endl;
            } else if ((event->mask & IN_ATTRIB) != 0U) {
                std::cout << "IN_ATTRIB: " << event->name << std::endl;
            } else if ((event->mask & IN_OPEN) != 0U) {
                std::cout << "IN_OPEN: " << event->name << std::endl;
            } else if ((event->mask & IN_DELETE_SELF) != 0U) {
                std::cout << "IN_DELETE_SELF: " << event->name << std::endl;
            } else if ((event->mask & IN_MOVE_SELF) != 0U) {
                std::cout << "IN_MOVE_SELF: " << event->name << std::endl;
            } else if ((event->mask & IN_UNMOUNT) != 0U) {
                std::cout << "IN_UNMOUNT: " << event->name << std::endl;
            } else if ((event->mask & IN_Q_OVERFLOW) != 0U) {
                std::cout << "IN_Q_OVERFLOW: " << event->name << std::endl;
            } else if ((event->mask & IN_IGNORED) != 0U) {
                std::cout << "IN_IGNORED: " << event->name << std::endl;
            } else if ((event->mask & IN_ISDIR) != 0U) {
                std::cout << "IN_ISDIR: " << event->name << std::endl;
            } else {
                std::cout << "UNKNOWN: " << event->name << std::endl;
            }
        }
    }

    void run(const std::string &dir)
    {
        if (!createFd()) {
            return;
        }

        if (!addWatch(dir)) {
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

    std::atomic_bool isRunning;
    std::thread monitorThread;
};

MonitorDir::MonitorDir(const std::filesystem::path &dir)
    : d_ptr(std::make_unique<MonitorDirPrivate>(this))
    , m_dir(dir)
    , m_isRunning(false)
{}

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
        d_ptr->run(m_dir.string());
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

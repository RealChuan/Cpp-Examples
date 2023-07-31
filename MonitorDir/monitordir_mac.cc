#include "monitordir.hpp"

#include <CoreServices/CoreServices.h>

#include <iostream>
#include <thread>

class MonitorDir::MonitorDirPrivate
{
public:
    explicit MonitorDirPrivate(MonitorDir *q)
        : q_ptr(q)
    {}

    ~MonitorDirPrivate() = default;

    static void monitorCallback(ConstFSEventStreamRef streamRef,
                                void *clientCallBackInfo,
                                size_t numEvents,
                                void *eventPaths,
                                const FSEventStreamEventFlags eventFlags[],
                                const FSEventStreamEventId eventIds[])
    {
        //auto *monitorDir = static_cast<MonitorDirPrivate *>(clientCallBackInfo);
        char **paths = static_cast<char **>(eventPaths);
        for (size_t i = 0; i < numEvents; ++i) {
            std::string path = paths[i];
            if ((eventFlags[i] & kFSEventStreamEventFlagItemCreated) != 0U) {
                std::cout << "Created: " << path << std::endl;
            } else if ((eventFlags[i] & kFSEventStreamEventFlagItemRemoved) != 0U) {
                std::cout << "Removed: " << path << std::endl;
            } else if ((eventFlags[i] & kFSEventStreamEventFlagItemRenamed) != 0U) {
                std::cout << "Renamed: " << path << std::endl;
            } else if ((eventFlags[i] & kFSEventStreamEventFlagItemModified) != 0U) {
                std::cout << "Modified: " << path << std::endl;
            } else if ((eventFlags[i] & kFSEventStreamEventFlagItemInodeMetaMod) != 0U) {
                std::cout << "InodeMetaMod: " << path << std::endl;
            } else if ((eventFlags[i] & kFSEventStreamEventFlagItemChangeOwner) != 0U) {
                std::cout << "ChangeOwner: " << path << std::endl;
            } else if ((eventFlags[i] & kFSEventStreamEventFlagItemXattrMod) != 0U) {
                std::cout << "XattrMod: " << path << std::endl;
            } else if ((eventFlags[i] & kFSEventStreamEventFlagItemIsFile) != 0U) {
                std::cout << "IsFile: " << path << std::endl;
            } else if ((eventFlags[i] & kFSEventStreamEventFlagItemIsDir) != 0U) {
                std::cout << "IsDir: " << path << std::endl;
            } else if ((eventFlags[i] & kFSEventStreamEventFlagItemIsSymlink) != 0U) {
                std::cout << "IsSymlink: " << path << std::endl;
            } else if ((eventFlags[i] & kFSEventStreamEventFlagOwnEvent) != 0U) {
                std::cout << "OwnEvent: " << path << std::endl;
            } else if ((eventFlags[i] & kFSEventStreamEventFlagItemIsHardlink) != 0U) {
                std::cout << "IsHardlink: " << path << std::endl;
            } else if ((eventFlags[i] & kFSEventStreamEventFlagItemIsLastHardlink) != 0U) {
                std::cout << "IsLastHardlink: " << path << std::endl;
            } else if ((eventFlags[i] & kFSEventStreamEventFlagItemCloned) != 0U) {
                std::cout << "Cloned: " << path << std::endl;
            } else {
                std::cout << "Unknown: " << path << std::endl;
            }
        }
    }

    void monitor(const std::string &dir)
    {
        std::cout << "addWatch: " << dir << std::endl;
        CFStringRef pathToWatch = CFStringCreateWithCString(kCFAllocatorDefault,
                                                            dir.c_str(),
                                                            kCFStringEncodingUTF8);
        CFArrayRef pathsToWatch = CFArrayCreate(kCFAllocatorDefault,
                                                reinterpret_cast<const void **>(&pathToWatch),
                                                1,
                                                nullptr);
        FSEventStreamContext context{0, this, nullptr, nullptr, nullptr};
        FSEventStreamRef stream = FSEventStreamCreate(kCFAllocatorDefault,
                                                      monitorCallback,
                                                      &context,
                                                      pathsToWatch,
                                                      kFSEventStreamEventIdSinceNow,
                                                      0.1,
                                                      kFSEventStreamCreateFlagFileEvents);
        runLoop = CFRunLoopGetCurrent();
        FSEventStreamScheduleWithRunLoop(stream, runLoop, kCFRunLoopDefaultMode);
        FSEventStreamStart(stream);
        CFRunLoopRun(); // This will block until the stream is stopped.
        FSEventStreamStop(stream);
        FSEventStreamInvalidate(stream);
        FSEventStreamRelease(stream);
        CFRelease(pathsToWatch);
        CFRelease(pathToWatch);
    }

    void stop()
    {
        if (CFRunLoopIsWaiting(runLoop) == 0U) {
            CFRunLoopWakeUp(runLoop);
        }
        CFRunLoopStop(runLoop);
    }

    void run(const std::string &dir) { monitor(dir); }

    MonitorDir *q_ptr;

    CFRunLoopRef runLoop;

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

bool MonitorDir::start()
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
    if (!m_isRunning) {
        std::cerr << "MonitorDir is not running" << std::endl;
        return;
    }

    d_ptr->stop();
    if (d_ptr->monitorThread.joinable()) {
        d_ptr->monitorThread.join();
    }
}

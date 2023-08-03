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
        std::string fileEvent;
        for (size_t i = 0; i < numEvents; ++i) {
            if ((eventFlags[i] & kFSEventStreamEventFlagItemCreated) != 0U) {
                fileEvent = "Created: ";
            } else if ((eventFlags[i] & kFSEventStreamEventFlagItemRemoved) != 0U) {
                fileEvent = "Removed: ";
            } else if ((eventFlags[i] & kFSEventStreamEventFlagItemRenamed) != 0U) {
                fileEvent = "Renamed: ";
            } else if ((eventFlags[i] & kFSEventStreamEventFlagItemModified) != 0U) {
                fileEvent = "Modified: ";
            } else if ((eventFlags[i] & kFSEventStreamEventFlagItemInodeMetaMod) != 0U) {
                fileEvent = "InodeMetaMod: ";
            } else if ((eventFlags[i] & kFSEventStreamEventFlagItemChangeOwner) != 0U) {
                fileEvent = "ChangeOwner: ";
            } else if ((eventFlags[i] & kFSEventStreamEventFlagItemXattrMod) != 0U) {
                fileEvent = "XattrMod: ";
            } else if ((eventFlags[i] & kFSEventStreamEventFlagItemIsFile) != 0U) {
                fileEvent = "IsFile: ";
            } else if ((eventFlags[i] & kFSEventStreamEventFlagItemIsDir) != 0U) {
                fileEvent = "IsDir: ";
            } else if ((eventFlags[i] & kFSEventStreamEventFlagItemIsSymlink) != 0U) {
                fileEvent = "IsSymlink: ";
            } else if ((eventFlags[i] & kFSEventStreamEventFlagOwnEvent) != 0U) {
                fileEvent = "OwnEvent: ";
            } else if ((eventFlags[i] & kFSEventStreamEventFlagItemIsHardlink) != 0U) {
                fileEvent = "IsHardlink: ";
            } else if ((eventFlags[i] & kFSEventStreamEventFlagItemIsLastHardlink) != 0U) {
                fileEvent = "IsLastHardlink: ";
            } else if ((eventFlags[i] & kFSEventStreamEventFlagItemCloned) != 0U) {
                fileEvent = "Cloned: ";
            } else {
                fileEvent = "Unknown: ";
            }
            fileEvent += paths[i];
            std::cout << fileEvent << std::endl;
        }
    }

    void monitor()
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
        if (nullptr == runLoop) {
            return;
        }
        if (CFRunLoopIsWaiting(runLoop) == 0U) {
            CFRunLoopWakeUp(runLoop);
        }
        CFRunLoopStop(runLoop);
    }

    void run() { monitor(); }

    MonitorDir *q_ptr;

    CFRunLoopRef runLoop = nullptr;

    std::filesystem::path dir;
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

bool MonitorDir::start()
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
    if (!m_isRunning) {
        std::cerr << "MonitorDir is not running" << std::endl;
        return;
    }

    d_ptr->stop();
    if (d_ptr->monitorThread.joinable()) {
        d_ptr->monitorThread.join();
    }
}

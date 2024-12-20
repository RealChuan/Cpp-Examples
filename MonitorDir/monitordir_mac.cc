#include "monitordir.hpp"

#include <CoreServices/CoreServices.h>

#include <iostream>
#include <thread>

std::string eventFlagsToString(FSEventStreamEventFlags eventFlags)
{
    if ((eventFlags & kFSEventStreamEventFlagNone) != 0U) {
        return "None: ";
    }
    if ((eventFlags & kFSEventStreamEventFlagMustScanSubDirs) != 0U) {
        return "MustScanSubDirs: ";
    }
    if ((eventFlags & kFSEventStreamEventFlagUserDropped) != 0U) {
        return "UserDropped: ";
    }
    if ((eventFlags & kFSEventStreamEventFlagKernelDropped) != 0U) {
        return "KernelDropped: ";
    }
    if ((eventFlags & kFSEventStreamEventFlagEventIdsWrapped) != 0U) {
        return "EventIdsWrapped: ";
    }
    if ((eventFlags & kFSEventStreamEventFlagHistoryDone) != 0U) {
        return "HistoryDone: ";
    }
    if ((eventFlags & kFSEventStreamEventFlagRootChanged) != 0U) {
        return "RootChanged: ";
    }
    if ((eventFlags & kFSEventStreamEventFlagMount) != 0U) {
        return "Mount: ";
    }
    if ((eventFlags & kFSEventStreamEventFlagUnmount) != 0U) {
        return "Unmount: ";
    }
    if ((eventFlags & kFSEventStreamEventFlagItemCreated) != 0U) {
        return "Created: ";
    }
    if ((eventFlags & kFSEventStreamEventFlagItemRemoved) != 0U) {
        return "Removed: ";
    }
    if ((eventFlags & kFSEventStreamEventFlagItemInodeMetaMod) != 0U) {
        return "InodeMetaMod: ";
    }
    if ((eventFlags & kFSEventStreamEventFlagItemRenamed) != 0U) {
        return "Renamed: ";
    }
    if ((eventFlags & kFSEventStreamEventFlagItemModified) != 0U) {
        return "Modified: ";
    }
    if ((eventFlags & kFSEventStreamEventFlagItemFinderInfoMod) != 0U) {
        return "FinderInfoMod: ";
    }
    if ((eventFlags & kFSEventStreamEventFlagItemChangeOwner) != 0U) {
        return "ChangeOwner: ";
    }
    if ((eventFlags & kFSEventStreamEventFlagItemXattrMod) != 0U) {
        return "XattrMod: ";
    }
    if ((eventFlags & kFSEventStreamEventFlagItemIsFile) != 0U) {
        return "IsFile: ";
    }
    if ((eventFlags & kFSEventStreamEventFlagItemIsDir) != 0U) {
        return "IsDir: ";
    }
    if ((eventFlags & kFSEventStreamEventFlagItemIsSymlink) != 0U) {
        return "IsSymlink: ";
    }
    if ((eventFlags & kFSEventStreamEventFlagOwnEvent) != 0U) {
        return "OwnEvent: ";
    }
    if ((eventFlags & kFSEventStreamEventFlagItemIsHardlink) != 0U) {
        return "IsHardlink: ";
    }
    if ((eventFlags & kFSEventStreamEventFlagItemIsLastHardlink) != 0U) {
        return "IsLastHardlink: ";
    }
    if ((eventFlags & kFSEventStreamEventFlagItemCloned) != 0U) {
        return "Cloned: ";
    }
    return "Unknown " + std::to_string(eventFlags) + ": ";
}

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
        // auto *monitorDir = static_cast<MonitorDirPrivate *>(clientCallBackInfo);
        char **paths = static_cast<char **>(eventPaths);
        if (paths == nullptr) {
            std::cerr << "Error: paths is null." << std::endl;
            return;
        }
        for (size_t i = 0; i < numEvents; ++i) {
            auto fileEvent = eventFlagsToString(eventFlags[i]);
            if (paths[i] != nullptr) {
                fileEvent += paths[i];
            }
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
                                                      3,
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

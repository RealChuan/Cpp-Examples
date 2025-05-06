#include "monitordir.hpp"

#include <CoreServices/CoreServices.h>

#include <iostream>

std::string eventFlagsToString(FSEventStreamEventFlags eventFlags)
{
    std::cout << "FSEventStreamEventFlags: " << std::to_string(eventFlags) << std::endl;

    std::string text;
    if ((eventFlags & kFSEventStreamEventFlagNone) != 0U) {
        text += "None";
    }
    if ((eventFlags & kFSEventStreamEventFlagMustScanSubDirs) != 0U) {
        text += "MustScanSubDirs ";
    }
    if ((eventFlags & kFSEventStreamEventFlagUserDropped) != 0U) {
        text += "UserDropped ";
    }
    if ((eventFlags & kFSEventStreamEventFlagKernelDropped) != 0U) {
        text += "KernelDropped ";
    }
    if ((eventFlags & kFSEventStreamEventFlagEventIdsWrapped) != 0U) {
        text += "EventIdsWrapped ";
    }
    if ((eventFlags & kFSEventStreamEventFlagHistoryDone) != 0U) {
        text += "HistoryDone ";
    }
    if ((eventFlags & kFSEventStreamEventFlagRootChanged) != 0U) {
        text += "RootChanged ";
    }
    if ((eventFlags & kFSEventStreamEventFlagMount) != 0U) {
        text += "Mount ";
    }
    if ((eventFlags & kFSEventStreamEventFlagUnmount) != 0U) {
        text += "Unmount ";
    }
    if ((eventFlags & kFSEventStreamEventFlagItemCreated) != 0U) {
        text += "Created ";
    }
    if ((eventFlags & kFSEventStreamEventFlagItemRemoved) != 0U) {
        text += "Removed ";
    }
    if ((eventFlags & kFSEventStreamEventFlagItemInodeMetaMod) != 0U) {
        text += "InodeMetaMod ";
    }
    if ((eventFlags & kFSEventStreamEventFlagItemRenamed) != 0U) {
        text += "Renamed ";
    }
    if ((eventFlags & kFSEventStreamEventFlagItemModified) != 0U) {
        text += "Modified ";
    }
    if ((eventFlags & kFSEventStreamEventFlagItemFinderInfoMod) != 0U) {
        text += "FinderInfoMod ";
    }
    if ((eventFlags & kFSEventStreamEventFlagItemChangeOwner) != 0U) {
        text += "ChangeOwner ";
    }
    if ((eventFlags & kFSEventStreamEventFlagItemXattrMod) != 0U) {
        text += "XattrMod ";
    }
    if ((eventFlags & kFSEventStreamEventFlagItemIsFile) != 0U) {
        text += "IsFile ";
    }
    if ((eventFlags & kFSEventStreamEventFlagItemIsDir) != 0U) {
        text += "IsDir ";
    }
    if ((eventFlags & kFSEventStreamEventFlagItemIsSymlink) != 0U) {
        text += "IsSymlink ";
    }
    if ((eventFlags & kFSEventStreamEventFlagOwnEvent) != 0U) {
        text += "OwnEvent ";
    }
    if ((eventFlags & kFSEventStreamEventFlagItemIsHardlink) != 0U) {
        text += "IsHardlink ";
    }
    if ((eventFlags & kFSEventStreamEventFlagItemIsLastHardlink) != 0U) {
        text += "IsLastHardlink ";
    }
    if ((eventFlags & kFSEventStreamEventFlagItemCloned) != 0U) {
        text += "Cloned ";
    }
    if (text.empty()) {
        text = "Unknown " + std::to_string(eventFlags) + "";
    } else {
        text += ": ";
    }
    return text;
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
            std::cerr << "Errorpaths is null." << std::endl;
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

    bool monitor()
    {
        std::cout << "addWatch" << dir << std::endl;
        auto pathToWatch = CFStringCreateWithCString(kCFAllocatorDefault,
                                                     dir.c_str(),
                                                     kCFStringEncodingUTF8);
        auto pathsToWatch = CFArrayCreate(kCFAllocatorDefault,
                                          reinterpret_cast<const void **>(&pathToWatch),
                                          1,
                                          nullptr);
        FSEventStreamContext context{0, this, nullptr, nullptr, nullptr};
        stream = FSEventStreamCreate(kCFAllocatorDefault,
                                     monitorCallback,
                                     &context,
                                     pathsToWatch,
                                     kFSEventStreamEventIdSinceNow,
                                     0.,
                                     kFSEventStreamCreateFlagFileEvents);
        if (stream == nullptr) {
            std::cerr << "Failed to create FSEventStream" << std::endl;
            CFRelease(pathsToWatch);
            CFRelease(pathToWatch);
            return false;
        }
        auto queue = dispatch_queue_create(nullptr, nullptr);
        FSEventStreamSetDispatchQueue(stream, queue);
        FSEventStreamStart(stream);
        CFRelease(pathsToWatch);
        CFRelease(pathToWatch);
        return true;
    }

    void stop()
    {
        if (nullptr == stream) {
            return;
        }
        FSEventStreamStop(stream);
        FSEventStreamInvalidate(stream);
        FSEventStreamRelease(stream);
    }

    bool run() { return monitor(); }

    MonitorDir *q_ptr;

    FSEventStreamRef stream = nullptr;

    std::filesystem::path dir;
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
    if (m_isRunning.load()) {
        std::cerr << "MonitorDir is already running" << std::endl;
        return false;
    }
    m_isRunning.store(d_ptr->run());
    return m_isRunning.load();
}

void MonitorDir::stop()
{
    if (!m_isRunning.load()) {
        std::cerr << "MonitorDir is not running" << std::endl;
        return;
    }

    d_ptr->stop();
}

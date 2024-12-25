#include <efsw/efsw.hpp>

#include <filesystem>
#include <iostream>
#include <signal.h>
#include <thread>

bool STOP = false;

void sigend(int)
{
    std::cout << std::endl << "Bye bye" << std::endl;
    STOP = true;
}

std::string getActionName(efsw::Action action)
{
    switch (action) {
    case efsw::Actions::Add: return "Add";
    case efsw::Actions::Modified: return "Modified";
    case efsw::Actions::Delete: return "Delete";
    case efsw::Actions::Moved: return "Moved";
    default: break;
    }
    return "Bad Action";
}

efsw::WatchID handleWatchID(efsw::WatchID watchid)
{
    switch (watchid) {
    case efsw::Errors::FileNotFound:
    case efsw::Errors::FileRepeated:
    case efsw::Errors::FileOutOfScope:
    case efsw::Errors::FileRemote:
    case efsw::Errors::WatcherFailed:
    case efsw::Errors::Unspecified: {
        std::cout << efsw::Errors::Log::getLastErrorLog().c_str() << std::endl;
        break;
    }
    default: {
        std::cout << "Added WatchID: " << watchid << std::endl;
    }
    }
    return watchid;
}

/// Processes a file action
class UpdateListener : public efsw::FileWatchListener
{
public:
    UpdateListener() {}

    void handleFileAction(efsw::WatchID watchid,
                          const std::string &dir,
                          const std::string &filename,
                          efsw::Action action,
                          std::string oldFilename = "") override
    {
        std::cout << "Watch ID " << watchid << " DIR ("
                  << dir + ") FILE ("
                         + (oldFilename.empty() ? "" : "from file " + oldFilename + " to ")
                         + filename + ") has event "
                  << getActionName(action) << std::endl;
    }
};

int main(int argc, char **argv)
{
    signal(SIGABRT, sigend);
    signal(SIGINT, sigend);
    signal(SIGTERM, sigend);

    std::cout << "Press ^C to exit demo" << std::endl;

    bool useGeneric = false;
    auto filepath = std::filesystem::current_path().parent_path().parent_path();
    std::cout << filepath << std::endl;

    auto *ul = new UpdateListener;

    /// create the file watcher object
    efsw::FileWatcher fileWatcher(useGeneric);

    fileWatcher.followSymlinks(false);
    fileWatcher.allowOutOfScopeLinks(false);

    /// starts watching
    fileWatcher.watch();

    /// add a watch to the system
    handleWatchID(fileWatcher.addWatch((filepath / "bin-64").string(), ul, true));

    /// adds another watch after started watching...
    std::this_thread::sleep_for(std::chrono::seconds(1));

    efsw::WatchID watchID = handleWatchID(
        fileWatcher.addWatch((filepath / "build").string(), ul, true));

    /// delete the watch
    if (watchID > 0) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        fileWatcher.removeWatch(watchID);
    }

    while (!STOP) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}

#include "monitordir.hpp"

#include <cassert>
#include <condition_variable>
#include <iostream>
#include <thread>
#include <windows.h>

std::string wstringToString(const std::wstring &wstr)
{
    if (wstr.empty()) {
        return std::string();
    }

    int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    if (size <= 0) {
        return std::string();
    }

    std::string str(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], size, NULL, NULL);
    return str;
}

std::string actionToString(DWORD Action)
{
    switch (Action) {
    case FILE_ACTION_ADDED: return "FILE_ACTION_ADDED: ";
    case FILE_ACTION_REMOVED: return "FILE_ACTION_REMOVED: ";
    case FILE_ACTION_MODIFIED: return "FILE_ACTION_MODIFIED: ";
    case FILE_ACTION_RENAMED_OLD_NAME: return "FILE_ACTION_RENAMED_OLD_NAME: ";
    case FILE_ACTION_RENAMED_NEW_NAME: return "FILE_ACTION_RENAMED_NEW_NAME: ";
    default: return "Unknown " + std::to_string(Action) + ": ";
    }
    return "";
}

class MonitorDir::MonitorDirPrivate
{
public:
    explicit MonitorDirPrivate(MonitorDir *q)
        : q_ptr(q)
    {}

    ~MonitorDirPrivate() = default;

    auto createHandle() -> bool
    {
        // 创建目录句柄
        directoryHandle = CreateFile(dir.string().c_str(),
                                     FILE_LIST_DIRECTORY,
                                     FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                     NULL,
                                     OPEN_EXISTING,
                                     FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
                                     NULL);
        if (directoryHandle == INVALID_HANDLE_VALUE) {
            std::cerr << "CreateFile failed: " << GetLastError() << std::endl;
            return false;
        }

        std::cout << "addWatch: " << dir << std::endl;

        return true;
    }

    auto createEvent() -> bool
    {
        // 创建事件句柄，用于通知目录变化
        overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
        if (overlapped.hEvent == NULL) {
            std::cerr << "CreateEvent failed: " << GetLastError() << std::endl;
            return false;
        }

        return true;
    }

    void closeHandle()
    {
        if (directoryHandle != INVALID_HANDLE_VALUE) {
            CloseHandle(directoryHandle);
            directoryHandle = INVALID_HANDLE_VALUE;
        }
    }

    void closeEvent()
    {
        if (overlapped.hEvent != NULL) {
            CloseHandle(overlapped.hEvent);
            overlapped.hEvent = NULL;
        }
    }

    void monitor()
    {
        std::vector<char> notifyBuffer(BUFSIZ);
        DWORD bytesReturned = 0;

        // 监视目录变化
        auto result = ReadDirectoryChangesW(directoryHandle,
                                            notifyBuffer.data(),
                                            static_cast<DWORD>(notifyBuffer.size()),
                                            TRUE,
                                            filters,
                                            &bytesReturned,
                                            &overlapped,
                                            NULL);
        if (!result) {
            std::cerr << "ReadDirectoryChangesW failed: " << GetLastError() << std::endl;
            return;
        }

        // 等待目录变化
        auto waitResult = WaitForSingleObject(overlapped.hEvent, INFINITE);
        if (!isRunning.load()) {
            return;
        }
        if (waitResult != WAIT_OBJECT_0) {
            std::cerr << "WaitForSingleObject failed: " << GetLastError() << std::endl;
            return;
        }

        // 获取目录变化结果
        result = GetOverlappedResult(directoryHandle, &overlapped, &bytesReturned, TRUE);
        if (!result) {
            std::cerr << "GetOverlappedResult failed: " << GetLastError() << std::endl;
            return;
        }
        if (bytesReturned == 0) {
            std::cerr << "GetOverlappedResult failed: bytesReturned is 0" << std::endl;
            return;
        }

        // 通知目录变化
        auto notifyInfo = reinterpret_cast<PFILE_NOTIFY_INFORMATION>(notifyBuffer.data());
        while (isRunning.load()) {
            std::wstring fileName(notifyInfo->FileName,
                                  notifyInfo->FileNameLength / sizeof(wchar_t));
            std::string fileEvent = actionToString(notifyInfo->Action) + wstringToString(fileName);
            std::cout << fileEvent << std::endl;

            if (notifyInfo->NextEntryOffset == 0) {
                break;
            }
            notifyInfo = reinterpret_cast<PFILE_NOTIFY_INFORMATION>(
                reinterpret_cast<LPBYTE>(notifyInfo) + notifyInfo->NextEntryOffset);
        }
    }

    void run()
    {
        if (!createHandle()) {
            return;
        }
        if (!createEvent()) {
            closeEvent();
            return;
        }
        isRunning.store(true);
        while (isRunning.load()) {
            monitor();
        }
        closeEvent();
        closeHandle();
    }

    MonitorDir *q_ptr;

    DWORD filters = FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME
                    | FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE
                    | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_LAST_ACCESS
                    | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_SECURITY;
    HANDLE directoryHandle = INVALID_HANDLE_VALUE;
    OVERLAPPED overlapped;

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
    if (m_isRunning.load()) {
        std::cerr << "MonitorDir is already running" << std::endl;
        return false;
    }
    m_isRunning.store(true);
    d_ptr->monitorThread = std::thread([this]() {
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

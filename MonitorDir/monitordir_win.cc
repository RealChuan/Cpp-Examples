#include "monitordir.hpp"

#include <condition_variable>
#include <iostream>
#include <thread>
#include <windows.h>

std::string utf8_encode(const std::wstring &wstr)
{
    if (wstr.empty()) {
        return std::string();
    }

    int size = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    if (size == 0) {
        return std::string();
    }

    std::string str(size, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &str[0], size, NULL, NULL);

    return str;
}

class MonitorDir::MonitorDirPrivate
{
public:
    explicit MonitorDirPrivate(MonitorDir *q)
        : q_ptr(q)
    {}

    ~MonitorDirPrivate() = default;

    auto createHandle(const std::string &dir) -> bool
    {
        // 创建目录句柄
        directoryHandle = CreateFile(dir.c_str(),
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

    void monitor(const std::string &dir)
    {
        char notifyBuffer[BUFSIZ] = {0};
        DWORD bytesReturned = 0;
        // 监视目录变化
        BOOL result = ReadDirectoryChangesW(directoryHandle,
                                            notifyBuffer,
                                            sizeof(notifyBuffer),
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
        DWORD waitResult = WaitForSingleObject(overlapped.hEvent, INFINITE);
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
        std::string fileEvent;
        PFILE_NOTIFY_INFORMATION notifyInfo = (PFILE_NOTIFY_INFORMATION) notifyBuffer;
        do {
            std::wstring fileName(notifyInfo->FileName,
                                  notifyInfo->FileNameLength / sizeof(wchar_t));
            std::string fileNameUtf8 = utf8_encode(fileName);

            switch (notifyInfo->Action) {
            case FILE_ACTION_ADDED: fileEvent = "added: " + fileNameUtf8; break;
            case FILE_ACTION_REMOVED: fileEvent = "removed: " + fileNameUtf8; break;
            case FILE_ACTION_MODIFIED: fileEvent = "modified: " + fileNameUtf8; break;
            case FILE_ACTION_RENAMED_OLD_NAME:
                fileEvent = "renamed old name: " + fileNameUtf8;
                break;
            case FILE_ACTION_RENAMED_NEW_NAME:
                fileEvent = "renamed new name: " + fileNameUtf8;
                break;
            default: break;
            }

            notifyInfo = (PFILE_NOTIFY_INFORMATION) ((LPBYTE) notifyInfo
                                                     + notifyInfo->NextEntryOffset);
        } while (notifyInfo->NextEntryOffset != 0);

        std::cout << fileEvent << std::endl;
    }

    void run(const std::string &dir)
    {
        if (!createHandle(dir)) {
            return;
        }
        if (!createEvent()) {
            closeEvent();
            return;
        }
        isRunning.store(true);
        while (isRunning.load()) {
            monitor(dir);
        }
        closeEvent();
        closeHandle();
    }

    MonitorDir *q_ptr;

    std::string fileExtension;
    std::string filePattern;
    std::string filePatternRegex;
    std::string filePatternRegexFlags;

    DWORD filters = FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME
                    | FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE
                    | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_LAST_ACCESS
                    | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_SECURITY;
    HANDLE directoryHandle = INVALID_HANDLE_VALUE;
    OVERLAPPED overlapped;

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
    if (m_isRunning.load()) {
        std::cerr << "MonitorDir is already running" << std::endl;
        return false;
    }
    m_isRunning.store(true);
    d_ptr->monitorThread = std::thread([this]() {
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

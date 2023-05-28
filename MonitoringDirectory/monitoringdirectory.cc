#include "monitoringdirectory.hpp"

#include <cassert>
#include <iostream>

MonitoringDirectory::MonitoringDirectory(const std::filesystem::path &directory)
    : m_directory(directory)
{}

MonitoringDirectory::~MonitoringDirectory()
{
    stop();
}

void MonitoringDirectory::start()
{
    if (!createHandle()) {
        return;
    }

    if (!createEvent()) {
        closeHandle();
        return;
    }

    DWORD filters = FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME
                    | FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE
                    | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_LAST_ACCESS
                    | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_SECURITY;
    m_isRunning = true;
    while (m_isRunning) {
        readDirectoryChanges();
    }
}

void MonitoringDirectory::stop()
{
    m_isRunning = false;

    closeHandle();
}

bool MonitoringDirectory::createHandle()
{
    m_directoryHandle = CreateFileW(m_directory.c_str(),
                                    FILE_LIST_DIRECTORY,
                                    FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                    NULL,
                                    OPEN_EXISTING,
                                    FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
                                    NULL);

    if (m_directoryHandle == INVALID_HANDLE_VALUE) {
        std::cerr << "CreateFileW failed: " << GetLastError() << std::endl;
        return false;
    }

    return true;
}

bool MonitoringDirectory::createEvent()
{
    m_overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    if (m_overlapped.hEvent == NULL) {
        std::cerr << "CreateEvent failed: " << GetLastError() << std::endl;
        return false;
    }

    return true;
}

void MonitoringDirectory::closeHandle()
{
    if (m_directoryHandle != INVALID_HANDLE_VALUE) {
        CloseHandle(m_directoryHandle);
        m_directoryHandle = INVALID_HANDLE_VALUE;
    }
    if (m_overlapped.hEvent != NULL) {
        CloseHandle(m_overlapped.hEvent);
        m_overlapped.hEvent = NULL;
    }
}

void MonitoringDirectory::readDirectoryChanges()
{
    assert(m_directoryHandle != INVALID_HANDLE_VALUE);
    assert(m_overlapped.hEvent != NULL);

    DWORD bytesTransferred = 0;
    BOOL result = ReadDirectoryChangesW(m_directoryHandle,
                                        m_notifyBuffer,
                                        sizeof(m_notifyBuffer),
                                        TRUE,
                                        filters,
                                        &bytesTransferred,
                                        &m_overlapped,
                                        NULL);

    if (!result) {
        std::cerr << "ReadDirectoryChangesW failed: " << GetLastError() << std::endl;
        return;
    }

    DWORD waitResult = WaitForSingleObject(m_overlapped.hEvent, INFINITE);
    switch (waitResult) {
    case WAIT_OBJECT_0: processDirectoryChanges(); break;
    case WAIT_TIMEOUT: std::cerr << "WaitForSingleObject timed out" << std::endl; break;
    default: std::cerr << "WaitForSingleObject failed: " << GetLastError() << std::endl; break;
    }
}

void MonitoringDirectory::processDirectoryChanges()
{
    assert(m_directoryHandle != INVALID_HANDLE_VALUE);
    assert(m_overlapped.hEvent != NULL);

    DWORD bytesTransferred = 0;
    BOOL result = GetOverlappedResult(m_directoryHandle, &m_overlapped, &bytesTransferred, FALSE);

    if (!result) {
        std::cerr << "GetOverlappedResult failed: " << GetLastError() << std::endl;
        return;
    }

    if (bytesTransferred == 0) {
        std::cerr << "GetOverlappedResult returned 0 bytes transferred" << std::endl;
        return;
    }

    PFILE_NOTIFY_INFORMATION notifyInfo = reinterpret_cast<PFILE_NOTIFY_INFORMATION>(m_notifyBuffer);
    while (m_isRunning) {
        std::wstring fileName(notifyInfo->FileName, notifyInfo->FileNameLength / sizeof(WCHAR));
        std::wcout << "File name: " << fileName << std::endl;

        switch (notifyInfo->Action) {
        case FILE_ACTION_ADDED: std::cout << "File added" << std::endl; break;
        case FILE_ACTION_REMOVED: std::cout << "File removed" << std::endl; break;
        case FILE_ACTION_MODIFIED: std::cout << "File modified" << std::endl; break;
        case FILE_ACTION_RENAMED_OLD_NAME: std::cout << "File renamed old name" << std::endl; break;
        case FILE_ACTION_RENAMED_NEW_NAME: std::cout << "File renamed new name" << std::endl; break;
        default: std::cout << "Unknown action: " << notifyInfo->Action << std::endl; break;
        }

        if (notifyInfo->NextEntryOffset == 0) {
            break;
        }

        notifyInfo = reinterpret_cast<PFILE_NOTIFY_INFORMATION>(reinterpret_cast<LPBYTE>(notifyInfo)
                                                                + notifyInfo->NextEntryOffset);
    }
}

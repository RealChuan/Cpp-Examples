#include <filesystem>
#include <string>
#include <windows.h>

class MonitoringDirectory
{
public:
    MonitoringDirectory(const std::filesystem::path &directory);
    ~MonitoringDirectory();

    void setDirectory(const std::filesystem::path &directory) { m_directory = directory; }
    std::filesystem::path getDirectory() const { return m_directory; }

    void start();
    void stop();

    bool isRunning() const { return m_isRunning; }
    std::string getLastFileEvent() const
    {
        std::string lastEvent = m_notifyBuffer;
        return lastEvent;
    }

private:
    bool createHandle();
    bool createEvent();
    void closeHandle();

    void readDirectoryChanges();
    void processDirectoryChanges();

    std::filesystem::path m_directory;
    std::string m_fileExtension;
    std::string m_filePattern;
    std::string m_filePatternRegex;
    std::string m_filePatternRegexFlags;
    bool m_isRunning;

    DWORD filters = FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME
                    | FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE
                    | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_LAST_ACCESS
                    | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_SECURITY;
    HANDLE m_directoryHandle = INVALID_HANDLE_VALUE;
    OVERLAPPED m_overlapped;
    char m_notifyBuffer[BUFSIZ] = {0};
};
#pragma once

#include <atomic>
#include <utils/object.hpp>

#include <filesystem>
#include <string>

class MonitorDir : public noncopyable
{
public:
    explicit MonitorDir(const std::filesystem::path &dir);
    ~MonitorDir();

    void setDir(const std::filesystem::path &dir) { m_dir = dir; }
    [[nodiscard]] auto getDir() const -> std::filesystem::path { return m_dir; }

    auto start() -> bool;
    void stop();

    [[nodiscard]] auto isRunning() const -> bool { return m_isRunning.load(); }

private:
    class MonitorDirPrivate;
    std::unique_ptr<MonitorDirPrivate> d_ptr;
    std::filesystem::path m_dir;
    std::atomic_bool m_isRunning;
};

void wakeWatch(const std::string &dir);

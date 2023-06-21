#pragma once

#include <object.hpp>

#include <curl/curl.h>

#include <string>
#include <vector>

class TcpClient : noncopyable
{
public:
    TcpClient();
    TcpClient(const std::string &host, int port);
    ~TcpClient();

    auto connect() -> bool;
    auto connect(const std::string &host, int port) -> bool;
    void disconnect();

    void send(const std::string &data);
    void send(const std::vector<char> &data);
    void send(const char *data, size_t size);

    auto recv() -> std::string;
    auto recv(size_t size) -> std::string;
    auto recv(std::vector<char> &data) -> size_t;
    auto recv(char *data, size_t size) -> size_t;

    [[nodiscard]] auto getHost() const -> std::string { return m_host; }
    [[nodiscard]] auto getPort() const -> int { return m_port; }

    [[nodiscard]] auto isConnected() const -> bool { return m_connected; }

    [[nodiscard]] auto getLastError() const -> CURLcode { return m_res; }
    [[nodiscard]] auto getLastErrorString() const -> std::string
    {
        return curl_easy_strerror(m_res);
    }

private:
    std::string m_host;
    int m_port;

    CURL *m_curl;
    CURLcode m_res;

    bool m_connected;
};

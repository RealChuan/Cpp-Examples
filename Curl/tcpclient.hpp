#pragma once

#include <utils/object.hpp>

#include <curl/curl.h>

#include <memory>
#include <string>
#include <vector>

class TcpClient : noncopyable
{
public:
    TcpClient();
    explicit TcpClient(const std::string &host, int port);
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

    [[nodiscard]] auto getHost() const -> std::string;
    [[nodiscard]] auto getPort() const -> int;

    [[nodiscard]] auto isConnected() const -> bool;

    [[nodiscard]] auto getLastError() const -> CURLcode;
    [[nodiscard]] auto getLastErrorString() const -> std::string;

private:
    class TcpClientPrivate;
    std::unique_ptr<TcpClientPrivate> d_ptr;
};

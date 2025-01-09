#pragma once

#include <utils/object.hpp>

#include <curl/curl.h>

#include <filesystem>
#include <map>
#include <memory>

class HttpClient : noncopyable
{
public:
    using Headers = std::map<std::string, std::string>;

    HttpClient();
    ~HttpClient();

    auto get(const std::string &url, const Headers &headers = {}) -> std::string;
    auto post(const std::string &url, const std::string &data, const Headers &headers = {})
        -> std::string;
    auto put(const std::string &url, const std::string &data, const Headers &headers = {})
        -> std::string;
    auto del(const std::string &url, const Headers &headers = {}) -> std::string;
    auto options(const std::string &url, const Headers &headers = {}) -> std::string;
    auto patch(const std::string &url, const std::string &data, const Headers &headers = {})
        -> std::string;

    auto sendCustomRequest(const std::string &url,
                           const std::string &method,
                           const std::string &data,
                           const Headers &headers = {}) -> std::string;

    auto download(const std::string &url,
                  const std::filesystem::path &path,
                  const Headers &headers = {}) -> bool;
    auto upload_put(const std::string &url,
                    const std::filesystem::path &path,
                    const Headers &headers = {}) -> bool;
    auto upload_post(const std::string &url,
                     const std::filesystem::path &path,
                     const Headers &headers = {}) -> bool;

    [[nodiscard]] auto error() const -> std::string;

private:
    class HttpClientPrivate;
    std::unique_ptr<HttpClientPrivate> d_ptr;
};

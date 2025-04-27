#pragma once

#include <utils/object.hpp>

#include <curl/curl.h>

#include <filesystem>
#include <functional>
#include <map>

class HttpClientAsync : noncopyable
{
public:
    using Callback = std::function<void(const std::string &)>;
    using Headers = std::map<std::string, std::string>;

    HttpClientAsync();
    ~HttpClientAsync();

    auto get(const std::string &url, const Headers &headers = {}, Callback callback = nullptr)
        -> CURL *;
    auto post(const std::string &url,
              const std::string &data,
              const Headers &headers = {},
              Callback callback = nullptr) -> CURL *;
    auto put(const std::string &url,
             const std::string &data,
             const Headers &headers = {},
             Callback callback = nullptr) -> CURL *;
    auto del(const std::string &url, const Headers &headers = {}, Callback callback = nullptr)
        -> CURL *;
    auto options(const std::string &url, const Headers &headers = {}, Callback callback = nullptr)
        -> CURL *;
    auto patch(const std::string &url,
               const std::string &data,
               const Headers &headers = {},
               Callback callback = nullptr) -> CURL *;

    auto sendCustomRequest(const std::string &url,
                           const std::string &method,
                           const std::string &data,
                           const Headers &headers,
                           Callback callback) -> CURL *;

    void cancel(CURL *handle);

    auto download(const std::string &url,
                  const std::filesystem::path &path,
                  const Headers &headers = {},
                  Callback callback = nullptr) -> CURL *;
    auto upload_put(const std::string &url,
                    const std::filesystem::path &path,
                    const Headers &headers = {},
                    Callback callback = nullptr) -> CURL *;
    auto upload_post(const std::string &url,
                     const std::filesystem::path &path,
                     const Headers &headers = {},
                     Callback callback = nullptr) -> CURL *;

private:
    class HttpClientAsyncPrivate;
    std::unique_ptr<HttpClientAsyncPrivate> d_ptr;
};

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

    CURL *get(const std::string &url, const Headers &headers = {}, Callback callback = nullptr);
    CURL *post(const std::string &url,
               const std::string &data,
               const Headers &headers = {},
               Callback callback = nullptr);
    CURL *put(const std::string &url,
              const std::string &data,
              const Headers &headers = {},
              Callback callback = nullptr);
    CURL *del(const std::string &url, const Headers &headers = {}, Callback callback = nullptr);
    CURL *options(const std::string &url, const Headers &headers = {}, Callback callback = nullptr);
    CURL *patch(const std::string &url,
                const std::string &data,
                const Headers &headers = {},
                Callback callback = nullptr);

    CURL *sendCustomRequest(const std::string &url,
                            const std::string &method,
                            const std::string &data,
                            const Headers &headers,
                            Callback callback);

    void cancel(CURL *handle);

    CURL *download(const std::string &url,
                   const std::filesystem::path &path,
                   const Headers &headers = {},
                   Callback callback = nullptr);
    CURL *upload_put(const std::string &url,
                     const std::filesystem::path &path,
                     const Headers &headers = {},
                     Callback callback = nullptr);
    CURL *upload_post(const std::string &url,
                      const std::filesystem::path &path,
                      const Headers &headers = {},
                      Callback callback = nullptr);

private:
    class HttpClientAsyncPrivate;
    std::unique_ptr<HttpClientAsyncPrivate> d_ptr;
};

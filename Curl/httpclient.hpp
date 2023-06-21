#pragma once

#include <object.hpp>

#include <curl/curl.h>

#include <map>
#include <stdexcept>
#include <string>

class HttpClient : noncopyable
{
public:
    using Headers = std::map<std::string, std::string>;

    HttpClient()
        : m_curl(curl_easy_init())
        , m_headers(nullptr)
    {
        if (m_curl == nullptr) {
            throw std::runtime_error("curl_easy_init() failed");
        }
    }
    ~HttpClient()
    {
        curl_easy_cleanup(m_curl);
        curl_slist_free_all(m_headers);
    }

    auto get(const std::string &url, const Headers &headers = Headers()) -> std::string
    {
        return sendCustomRequest(url, "GET", "", headers);
    }
    auto post(const std::string &url, const std::string &data, const Headers &headers = Headers())
        -> std::string
    {
        return sendCustomRequest(url, "POST", data, headers);
    }
    auto put(const std::string &url, const std::string &data, const Headers &headers = Headers())
        -> std::string
    {
        return sendCustomRequest(url, "PUT", data, headers);
    }
    auto del(const std::string &url, const Headers &headers = Headers()) -> std::string
    {
        return sendCustomRequest(url, "DELETE", "", headers);
    }
    auto options(const std::string &url, const Headers &headers = Headers()) -> std::string
    {
        return sendCustomRequest(url, "OPTIONS", "", headers);
    }
    auto patch(const std::string &url, const std::string &data, const Headers &headers = Headers())
        -> std::string
    {
        return sendCustomRequest(url, "PATCH", data, headers);
    }

    auto sendCustomRequest(const std::string &url,
                           const std::string &method,
                           const std::string &data,
                           const Headers &headers = Headers()) -> std::string;

    [[nodiscard]] auto error() const -> std::string { return m_buffer; }

private:
    static auto writeCallback(char *ptr, size_t size, size_t nmemb, void *userdata) -> size_t
    {
        auto *client = static_cast<HttpClient *>(userdata);
        client->m_buffer.append(ptr, size * nmemb);
        return size * nmemb;
    }
    static auto headerCallback(char *ptr, size_t size, size_t nmemb, void *userdata) -> size_t
    {
        auto *client = static_cast<HttpClient *>(userdata);
        client->m_buffer.append(ptr, size * nmemb);
        return size * nmemb;
    }

    CURL *m_curl;
    CURLcode m_res;
    struct curl_slist *m_headers;
    std::string m_buffer;
};

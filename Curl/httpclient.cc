#include "httpclient.hpp"
#include "file_utils.hpp"

#include <cassert>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

class HttpClient::HttpClientPrivate
{
public:
    explicit HttpClientPrivate(HttpClient *q)
        : q_ptr(q)
        , curl(curl_easy_init())
    {
        if (curl == nullptr) {
            throw std::runtime_error("curl_easy_init() failed");
        }

        // setVerbose();
        setHeaderCallback();
    }

    void setVerbose()
    {
        assert(curl != nullptr);
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    }

    void setHeaderCallback()
    {
        assert(curl != nullptr);
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, headerCallback);
        curl_easy_setopt(curl, CURLOPT_HEADERDATA, q_ptr);
    }

    void setErrorCallback()
    {
        assert(curl != nullptr);
        curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, q_ptr->d_ptr->errorBuffer.data());
    }

    void setHeader(const Headers &headers)
    {
        assert(curl != nullptr);
        for (const auto &[key, value] : std::as_const(headers)) {
            this->headers = curl_slist_append(this->headers, (key + ": " + value).c_str());
        }
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, this->headers);
    }

    void clear()
    {
        clearBuffer();
        curl_easy_cleanup(curl);
    }

    void clearBuffer()
    {
        headerBuffer.clear();
        bodyBuffer.clear();
        errorBuffer.clear();
        curl_slist_free_all(headers);
        headers = nullptr;
    }

    void printError()
    {
        if (!errorBuffer.empty()) {
            std::cerr << "Error: " << errorBuffer << std::endl;
        }
    }

    static auto headerCallback(char *ptr, size_t size, size_t nmemb, void *userdata) -> size_t
    {
        auto *client = static_cast<HttpClient *>(userdata);
        client->d_ptr->headerBuffer.append(ptr, size * nmemb);
        return size * nmemb;
    }

    static auto writeCallback(char *ptr, size_t size, size_t nmemb, void *userdata) -> size_t
    {
        auto *client = static_cast<HttpClient *>(userdata);
        client->d_ptr->bodyBuffer.append(ptr, size * nmemb);
        return size * nmemb;
    }

    static auto downloadCallback(char *ptr, size_t size, size_t nmemb, void *userdata) -> size_t
    {
        auto *client = static_cast<HttpClient *>(userdata);
        assert(client->d_ptr->file.is_open());
        client->d_ptr->file.write(ptr, size * nmemb);
        return size * nmemb;
    }

    static auto downloadProgressCallback(void *clientp,
                                         curl_off_t dltotal,
                                         curl_off_t dlnow,
                                         curl_off_t ultotal,
                                         curl_off_t ulnow) -> int
    {
        std::cout << "Download progress: " << formatBytes(dlnow) << "/" << formatBytes(dltotal)
                  << std::endl;
        return 0;
    }

    static auto uploadCallback(char *ptr, size_t size, size_t nmemb, void *userdata) -> size_t
    {
        auto *client = static_cast<HttpClient *>(userdata);
        auto &file = client->d_ptr->file;
        assert(file.is_open());
        file.read(ptr, size * nmemb);
        return file.gcount();
    }

    static auto uploadProgressCallback(void *clientp,
                                       curl_off_t dltotal,
                                       curl_off_t dlnow,
                                       curl_off_t ultotal,
                                       curl_off_t ulnow) -> int
    {
        std::cout << "Upload progress: " << formatBytes(ulnow) << "/" << formatBytes(ultotal)
                  << std::endl;
        return 0;
    }

    HttpClient *q_ptr;

    CURL *curl = nullptr;
    CURLcode res = CURLE_OK;
    struct curl_slist *headers = nullptr;
    std::string headerBuffer = {};
    std::string bodyBuffer = {};
    std::string errorBuffer = {};
    std::fstream file;
};

HttpClient::HttpClient()
    : d_ptr(std::make_unique<HttpClientPrivate>(this))
{}

HttpClient::~HttpClient()
{
    d_ptr->clear();
}

auto HttpClient::get(const std::string &url, const Headers &headers) -> std::string
{
    return sendCustomRequest(url, "GET", "", headers);
}

auto HttpClient::post(const std::string &url, const std::string &data, const Headers &headers)
    -> std::string
{
    return sendCustomRequest(url, "POST", data, headers);
}

auto HttpClient::put(const std::string &url, const std::string &data, const Headers &headers)
    -> std::string
{
    return sendCustomRequest(url, "PUT", data, headers);
}

auto HttpClient::del(const std::string &url, const Headers &headers) -> std::string
{
    return sendCustomRequest(url, "DELETE", "", headers);
}

auto HttpClient::options(const std::string &url, const Headers &headers) -> std::string
{
    return sendCustomRequest(url, "OPTIONS", "", headers);
}

auto HttpClient::patch(const std::string &url, const std::string &data, const Headers &headers)
    -> std::string
{
    return sendCustomRequest(url, "PATCH", data, headers);
}

auto HttpClient::sendCustomRequest(const std::string &url,
                                   const std::string &method,
                                   const std::string &data,
                                   const Headers &headers) -> std::string
{
    d_ptr->clearBuffer();

    std::cout << "Sending (" << method << ") request to (" << url << ")" << std::endl;

    curl_easy_setopt(d_ptr->curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(d_ptr->curl, CURLOPT_CUSTOMREQUEST, method.c_str());
    curl_easy_setopt(d_ptr->curl, CURLOPT_POSTFIELDS, data.c_str());
    curl_easy_setopt(d_ptr->curl, CURLOPT_POSTFIELDSIZE, data.size());
    curl_easy_setopt(d_ptr->curl, CURLOPT_WRITEFUNCTION, HttpClientPrivate::writeCallback);
    curl_easy_setopt(d_ptr->curl, CURLOPT_WRITEDATA, this);

    d_ptr->setHeader(headers);

    d_ptr->res = curl_easy_perform(d_ptr->curl);
    if (d_ptr->res != CURLE_OK) {
        throw std::runtime_error(curl_easy_strerror(d_ptr->res));
    }

    d_ptr->printError();

    return d_ptr->bodyBuffer;
}

auto HttpClient::download(const std::string &url,
                          const std::filesystem::path &path,
                          const Headers &headers) -> bool
{
    d_ptr->clearBuffer();

    d_ptr->file.open(path, std::ios::out | std::ios::binary);
    if (!d_ptr->file.is_open()) {
        d_ptr->bodyBuffer = "Failed to open file: " + path.string();
        return false;
    }

    std::cout << "Downloading (" << url << ") to (" << path << ")" << std::endl;

    curl_easy_setopt(d_ptr->curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(d_ptr->curl, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(d_ptr->curl, CURLOPT_WRITEFUNCTION, HttpClientPrivate::downloadCallback);
    curl_easy_setopt(d_ptr->curl, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(d_ptr->curl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(d_ptr->curl,
                     CURLOPT_XFERINFOFUNCTION,
                     HttpClientPrivate::downloadProgressCallback);

    d_ptr->setHeader(headers);

    d_ptr->res = curl_easy_perform(d_ptr->curl);

    d_ptr->file.close();
    if (d_ptr->res != CURLE_OK) {
        throw std::runtime_error(curl_easy_strerror(d_ptr->res));
    }

    d_ptr->printError();

    return true;
}

auto HttpClient::upload_put(const std::string &url,
                            const std::filesystem::path &path,
                            const Headers &headers) -> bool
{
    d_ptr->clearBuffer();

    d_ptr->file.open(path, std::ios::in | std::ios::binary);
    if (!d_ptr->file.is_open()) {
        std::cerr << "Failed to open file: " << path << std::endl;
        return false;
    }

    std::cout << "Uploading (" << path << ") to (" << url << ")" << std::endl;

    curl_easy_setopt(d_ptr->curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(d_ptr->curl, CURLOPT_CUSTOMREQUEST, "PUT");
    curl_easy_setopt(d_ptr->curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(d_ptr->curl, CURLOPT_READFUNCTION, HttpClientPrivate::uploadCallback);
    curl_easy_setopt(d_ptr->curl, CURLOPT_READDATA, this);
    curl_easy_setopt(d_ptr->curl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(d_ptr->curl,
                     CURLOPT_XFERINFOFUNCTION,
                     HttpClientPrivate::uploadProgressCallback);

    d_ptr->setHeader(headers);

    d_ptr->res = curl_easy_perform(d_ptr->curl);

    d_ptr->file.close();
    if (d_ptr->res != CURLE_OK) {
        throw std::runtime_error(curl_easy_strerror(d_ptr->res));
    }

    d_ptr->printError();

    return true;
}

auto HttpClient::upload_post(const std::string &url,
                             const std::filesystem::path &path,
                             const Headers &headers) -> bool
{
    d_ptr->clearBuffer();

    std::cout << "Uploading (" << path << ") to (" << url << ")" << std::endl;

    curl_easy_setopt(d_ptr->curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(d_ptr->curl, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(d_ptr->curl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(d_ptr->curl,
                     CURLOPT_XFERINFOFUNCTION,
                     HttpClientPrivate::uploadProgressCallback);

    d_ptr->setHeader(headers);

    auto *form = curl_mime_init(d_ptr->curl);
    auto *part = curl_mime_addpart(form);
    curl_mime_name(part, "file");
    curl_mime_filedata(part, path.string().c_str());
    curl_easy_setopt(d_ptr->curl, CURLOPT_MIMEPOST, form);

    d_ptr->res = curl_easy_perform(d_ptr->curl);

    if (d_ptr->res != CURLE_OK) {
        throw std::runtime_error(curl_easy_strerror(d_ptr->res));
    }

    d_ptr->printError();

    curl_mime_free(form);

    return true;
}

auto HttpClient::error() const -> std::string
{
    return d_ptr->errorBuffer;
}

#include "httpclient_async.hpp"
#include "file_utils.hpp"

#include <curl/easy.h>
#include <curl/multi.h>

#include <atomic>
#include <cassert>
#include <fstream>
#include <iostream>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

class HttpClientAsync::HttpClientAsyncPrivate
{
public:
    struct Context
    {
        ~Context()
        {
            if (headers) {
                curl_slist_free_all(headers);
            }

            if (form) {
                curl_mime_free(form);
            }

            if (file.is_open()) {
                file.close();
            }
        }

        void printError()
        {
            if (!errorBuffer.empty()) {
                std::cerr << errorBuffer << std::endl;
            }
        }

        struct curl_slist *headers = nullptr;
        struct curl_mime *form = nullptr;
        std::string headerBuffer = {};
        std::string bodyBuffer = {};
        std::string errorBuffer = {};
        std::fstream file;
        Callback callback = nullptr;
    };

    explicit HttpClientAsyncPrivate(HttpClientAsync *q)
        : q_ptr(q)
    {
        init();
    }

    ~HttpClientAsyncPrivate()
    {
        stop();
        cleanup();
    }

    void init()
    {
        multi = curl_multi_init();
        if (multi == nullptr) {
            throw std::runtime_error("curl_multi_init() failed");
        }

        running.store(true);
        thread = std::thread(&HttpClientAsyncPrivate::run, this);
    }

    void run()
    {
        int stillRunning = 0;
        while (running.load()) {
            auto res = curl_multi_perform(multi, &stillRunning);
            if (res != CURLM_OK) {
                std::cerr << "curl_multi_perform() failed: " << curl_multi_strerror(res)
                          << std::endl;
                break;
            }
            checkMultiInfo();

            res = curl_multi_wait(multi, nullptr, 0, 1000, nullptr);
            if (res != CURLM_OK) {
                std::cerr << "curl_multi_wait() failed: " << curl_multi_strerror(res) << std::endl;
                break;
            }
        }
    }

    void checkMultiInfo()
    {
        int pending = 0;
        while (auto *message = curl_multi_info_read(multi, &pending)) {
            if (message->msg == CURLMSG_DONE) {
                auto *handle = message->easy_handle;
                auto it = contexts.find(handle);
                if (it != contexts.end()) {
                    auto *context = it->second.get();
                    if (context->callback) {
                        context->callback(context->bodyBuffer);
                    }
                    context->printError();
                    removeHandle(handle);
                } else {
                    std::cerr << "Handle not found in contexts" << std::endl;
                }
            }
        }
    }

    void addHandle(CURL *handle, Context *context)
    {
        std::lock_guard<std::mutex> lock(mutex);
        contexts[handle].reset(context);
        curl_multi_add_handle(multi, handle);
    }

    void removeHandle(CURL *handle)
    {
        std::lock_guard<std::mutex> lock(mutex);
        contexts.erase(handle);
        curl_multi_remove_handle(multi, handle);
        curl_easy_cleanup(handle);
    }

    void stop()
    {
        running.store(false);

        if (thread.joinable()) {
            thread.join();
        }
    }

    void cleanup()
    {
        for (auto &it : contexts) {
            curl_multi_remove_handle(multi, it.first);
            curl_easy_cleanup(it.first);
        }
        contexts.clear();
        curl_multi_cleanup(multi);
    }

    void setVerbose(CURL *handle)
    {
        assert(handle != nullptr);
        curl_easy_setopt(handle, CURLOPT_VERBOSE, 1L);
    }

    void setHeaderCallback(CURL *handle, Context *context)
    {
        assert(handle != nullptr && context != nullptr);
        curl_easy_setopt(handle, CURLOPT_HEADERFUNCTION, headerCallback);
        curl_easy_setopt(handle, CURLOPT_HEADERDATA, context);
    }

    void setErrorCallback(CURL *handle, Context *context)
    {
        assert(handle != nullptr && context != nullptr);
        curl_easy_setopt(handle, CURLOPT_ERRORBUFFER, context->errorBuffer.data());
    }

    void setHeader(CURL *handle, Context *context, const Headers &headers)
    {
        assert(handle != nullptr);
        for (const auto &[key, value] : std::as_const(headers)) {
            context->headers = curl_slist_append(context->headers, (key + ": " + value).c_str());
        }
        curl_easy_setopt(handle, CURLOPT_HTTPHEADER, context->headers);
    }

    static auto headerCallback(char *ptr, size_t size, size_t nmemb, void *userdata) -> size_t
    {
        auto *context = static_cast<Context *>(userdata);
        context->headerBuffer.append(ptr, size * nmemb);
        return size * nmemb;
    }

    static auto writeCallback(char *ptr, size_t size, size_t nmemb, void *userdata) -> size_t
    {
        auto *context = static_cast<Context *>(userdata);
        context->bodyBuffer.append(ptr, size * nmemb);
        return size * nmemb;
    }

    static auto downloadCallback(char *ptr, size_t size, size_t nmemb, void *userdata) -> size_t
    {
        auto *context = static_cast<Context *>(userdata);
        assert(context->file.is_open());
        context->file.write(ptr, size * nmemb);
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
        auto *context = static_cast<Context *>(userdata);
        auto &file = context->file;
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

    HttpClientAsync *q_ptr;

    CURLM *multi;
    std::thread thread;
    std::atomic_bool running;
    std::mutex mutex;
    std::map<CURL *, std::unique_ptr<Context>> contexts;
};

HttpClientAsync::HttpClientAsync()
    : d_ptr(new HttpClientAsyncPrivate(this))
{}

HttpClientAsync::~HttpClientAsync() {}

CURL *HttpClientAsync::get(const std::string &url, const Headers &headers, Callback callback)
{
    return sendCustomRequest(url, "GET", "", headers, callback);
}

CURL *HttpClientAsync::post(const std::string &url,
                            const std::string &data,
                            const Headers &headers,
                            Callback callback)
{
    return sendCustomRequest(url, "POST", data, headers, callback);
}

CURL *HttpClientAsync::put(const std::string &url,
                           const std::string &data,
                           const Headers &headers,
                           Callback callback)
{
    return sendCustomRequest(url, "PUT", data, headers, callback);
}

CURL *HttpClientAsync::del(const std::string &url, const Headers &headers, Callback callback)
{
    return sendCustomRequest(url, "DELETE", "", headers, callback);
}

CURL *HttpClientAsync::options(const std::string &url, const Headers &headers, Callback callback)
{
    return sendCustomRequest(url, "OPTIONS", "", headers, callback);
}

CURL *HttpClientAsync::patch(const std::string &url,
                             const std::string &data,
                             const Headers &headers,
                             Callback callback)
{
    return sendCustomRequest(url, "PATCH", data, headers, callback);
}

CURL *HttpClientAsync::sendCustomRequest(const std::string &url,
                                         const std::string &method,
                                         const std::string &data,
                                         const Headers &headers,
                                         Callback callback)
{
    auto *handle = curl_easy_init();
    if (handle == nullptr) {
        throw std::runtime_error("curl_easy_init() failed");
    }

    std::cout << "Sending (" << method << ") request to (" << url << ")" << std::endl;

    auto *context = new HttpClientAsyncPrivate::Context;
    context->callback = callback;

    curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, method.c_str());
    curl_easy_setopt(handle, CURLOPT_POSTFIELDS, data.c_str());
    curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, data.size());
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, HttpClientAsyncPrivate::writeCallback);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, context);

    d_ptr->setHeaderCallback(handle, context);
    d_ptr->setErrorCallback(handle, context);
    // d_ptr->setVerbose(handle);

    d_ptr->setHeader(handle, context, headers);

    d_ptr->addHandle(handle, context);

    return handle;
}

void HttpClientAsync::cancel(CURL *handle)
{
    d_ptr->removeHandle(handle);
}

CURL *HttpClientAsync::download(const std::string &url,
                                const std::filesystem::path &path,
                                const Headers &headers,
                                Callback callback)
{
    std::unique_ptr<HttpClientAsyncPrivate::Context> contextPtr(new HttpClientAsyncPrivate::Context);
    contextPtr->file.open(path, std::ios::out | std::ios::binary);
    if (!contextPtr->file.is_open()) {
        contextPtr->bodyBuffer = "Failed to open file: " + path.string();
        return nullptr;
    }

    std::cout << "Downloading (" << url << ") to (" << path << ")" << std::endl;

    auto *handle = curl_easy_init();
    if (handle == nullptr) {
        throw std::runtime_error("curl_easy_init() failed");
    }

    auto *context = contextPtr.release();
    context->callback = callback;

    curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, HttpClientAsyncPrivate::downloadCallback);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, context);
    curl_easy_setopt(handle, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(handle,
                     CURLOPT_XFERINFOFUNCTION,
                     HttpClientAsyncPrivate::downloadProgressCallback);

    d_ptr->setHeaderCallback(handle, context);
    d_ptr->setErrorCallback(handle, context);
    // d_ptr->setVerbose(handle);

    d_ptr->setHeader(handle, context, headers);

    d_ptr->addHandle(handle, context);

    return handle;
}

CURL *HttpClientAsync::upload_put(const std::string &url,
                                  const std::filesystem::path &path,
                                  const Headers &headers,
                                  Callback callback)
{
    std::unique_ptr<HttpClientAsyncPrivate::Context> contextPtr(new HttpClientAsyncPrivate::Context);
    contextPtr->file.open(path, std::ios::in | std::ios::binary);
    if (!contextPtr->file.is_open()) {
        std::cerr << "Failed to open file: " << path << std::endl;
        return nullptr;
    }

    std::cout << "Uploading (" << path << ") to (" << url << ")" << std::endl;

    auto *handle = curl_easy_init();
    if (handle == nullptr) {
        throw std::runtime_error("curl_easy_init() failed");
    }

    auto *context = contextPtr.release();
    context->callback = callback;

    curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "PUT");
    curl_easy_setopt(handle, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(handle, CURLOPT_READFUNCTION, HttpClientAsyncPrivate::uploadCallback);
    curl_easy_setopt(handle, CURLOPT_READDATA, context);
    curl_easy_setopt(handle, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(handle,
                     CURLOPT_XFERINFOFUNCTION,
                     HttpClientAsyncPrivate::uploadProgressCallback);

    d_ptr->setHeaderCallback(handle, context);
    d_ptr->setErrorCallback(handle, context);
    // d_ptr->setVerbose(handle);

    d_ptr->setHeader(handle, context, headers);

    d_ptr->addHandle(handle, context);

    return handle;
}

CURL *HttpClientAsync::upload_post(const std::string &url,
                                   const std::filesystem::path &path,
                                   const Headers &headers,
                                   Callback callback)
{
    auto *handle = curl_easy_init();
    if (handle == nullptr) {
        throw std::runtime_error("curl_easy_init() failed");
    }

    std::cout << "Uploading (" << path << ") to (" << url << ")" << std::endl;

    auto *context = new HttpClientAsyncPrivate::Context;
    context->callback = callback;

    curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(handle, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(handle,
                     CURLOPT_XFERINFOFUNCTION,
                     HttpClientAsyncPrivate::uploadProgressCallback);

    context->form = curl_mime_init(handle);
    auto *part = curl_mime_addpart(context->form);
    curl_mime_name(part, "file");
    curl_mime_filedata(part, path.string().c_str());
    curl_easy_setopt(handle, CURLOPT_MIMEPOST, context->form);

    d_ptr->setHeaderCallback(handle, context);
    d_ptr->setErrorCallback(handle, context);
    // d_ptr->setVerbose(handle);

    d_ptr->setHeader(handle, context, headers);

    d_ptr->addHandle(handle, context);

    return handle;
}

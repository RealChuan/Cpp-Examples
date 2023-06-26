#pragma once

#include <object.hpp>

#include <curl/curl.h>
#include <curl/easy.h>
#include <curl/multi.h>

#include <atomic>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <map>
#include <mutex>
#include <string>
#include <thread>

class HttpClientAsync : noncopyable
{
public:
    using Callback = std::function<void(const std::string &)>;
    using Headers = std::map<std::string, std::string>;

    HttpClientAsync() { init(); }
    ~HttpClientAsync()
    {
        stop();
        cleanup();
    }

    void get(const std::string &url, const Headers &headers = Headers(), Callback callback = nullptr)
    {
        sendCustomRequest(url, "GET", "", headers, callback);
    }
    void post(const std::string &url,
              const std::string &data,
              const Headers &headers = Headers(),
              Callback callback = nullptr)
    {
        sendCustomRequest(url, "POST", data, headers, callback);
    }
    void put(const std::string &url,
             const std::string &data,
             const Headers &headers = Headers(),
             Callback callback = nullptr)
    {
        sendCustomRequest(url, "PUT", data, headers, callback);
    }
    void del(const std::string &url, const Headers &headers = Headers(), Callback callback = nullptr)
    {
        sendCustomRequest(url, "DELETE", "", headers, callback);
    }
    void options(const std::string &url,
                 const Headers &headers = Headers(),
                 Callback callback = nullptr)
    {
        sendCustomRequest(url, "OPTIONS", "", headers, callback);
    }
    void patch(const std::string &url,
               const std::string &data,
               const Headers &headers = Headers(),
               Callback callback = nullptr)
    {
        sendCustomRequest(url, "PATCH", data, headers, callback);
    }

    void sendCustomRequest(const std::string &url,
                           const std::string &method,
                           const std::string &data,
                           const Headers &headers,
                           Callback callback);

private:
    static auto writeCallback(char *ptr, size_t size, size_t nmemb, void *userdata) -> size_t
    {
        auto *context = static_cast<Context *>(userdata);
        context->response.append(ptr, size * nmemb);
        return size * nmemb;
    }
    static auto headerCallback(char *ptr, size_t size, size_t nmemb, void *userdata) -> size_t
    {
        auto *context = static_cast<Context *>(userdata);
        context->response.append(ptr, size * nmemb);
        return size * nmemb;
    }
    static auto readCallback(char *buffer, size_t size, size_t nitems, void *instream) -> size_t
    {
        return 0;
    }
    static auto progressCallback(
        void *clientp, double dltotal, double dlnow, double ultotal, double ulnow) -> int
    {
        return 0;
    }

    struct Context
    {
        Callback callback;
        std::string response;
    };

    void init()
    {
        m_multi = curl_multi_init();
        if (m_multi == nullptr) {
            throw std::runtime_error("curl_multi_init() failed");
        }

        m_running = true;
        m_thread = std::thread(&HttpClientAsync::run, this);
    }
    void cleanup()
    {
        for (auto &it : m_contexts) {
            curl_multi_remove_handle(m_multi, it.first);
            curl_easy_cleanup(it.first);
        }
        m_contexts.clear();
        curl_multi_cleanup(m_multi);
    }

    void addHandle(CURL *handle, Context *context)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_contexts[handle].reset(context);
        curl_multi_add_handle(m_multi, handle);
    }
    void removeHandle(CURL *handle)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        m_contexts.erase(handle);
        curl_multi_remove_handle(m_multi, handle);
        curl_easy_cleanup(handle);
    }
    void checkMultiInfo()
    {
        CURLMsg *message = nullptr;
        int pending = 0;
        while ((message = curl_multi_info_read(m_multi, &pending))) {
            switch (message->msg) {
            case CURLMSG_DONE: {
                CURL *handle = message->easy_handle;
                auto it = m_contexts.find(handle);
                if (it == m_contexts.end()) {
                    throw std::runtime_error("curl_multi_info_read() failed");
                }
                auto *context = it->second.get();
                context->callback(context->response);
                removeHandle(handle);
                break;
            }
            default: throw std::runtime_error("curl_multi_info_read() failed");
            }
        }
    }

    void run()
    {
        int stillRunning = 0;
        while (m_running) {
            curl_multi_perform(m_multi, &stillRunning);
            checkMultiInfo();

            std::unique_lock<std::mutex> lock(m_mutex);
            m_condition.wait_for(lock, std::chrono::milliseconds(100), [this] {
                int stillRunning = 0;
                curl_multi_perform(m_multi, &stillRunning);
                return stillRunning == 0;
            });
        }
    }
    void stop()
    {
        m_running = false;
        m_condition.notify_one();

        if (m_thread.joinable()) {
            m_thread.join();
        }
    }

    CURLM *m_multi;
    std::thread m_thread;
    std::atomic_bool m_running;
    std::mutex m_mutex;
    std::condition_variable m_condition;
    std::map<CURL *, std::unique_ptr<Context>> m_contexts;
    std::string m_buffer;
};

#include "httpclient_async.hpp"

void HttpClientAsync::sendCustomRequest(const std::string &url,
                                        const std::string &method,
                                        const std::string &data,
                                        const Headers &headers,
                                        Callback callback)
{
    CURL *handle = curl_easy_init();
    if (handle == nullptr) {
        throw std::runtime_error("curl_easy_init() failed");
    }

    auto *context = new Context{callback, ""};

    curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(handle, CURLOPT_CUSTOMREQUEST, method.c_str());
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, context);
    curl_easy_setopt(handle, CURLOPT_HEADERFUNCTION, headerCallback);
    curl_easy_setopt(handle, CURLOPT_HEADERDATA, context);
    // curl_easy_setopt(handle, CURLOPT_READFUNCTION, readCallback);
    // curl_easy_setopt(handle, CURLOPT_PROGRESSFUNCTION, progressCallback);
    curl_easy_setopt(handle, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(handle, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(handle, CURLOPT_MAXREDIRS, 50L);
    curl_easy_setopt(handle, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(handle, CURLOPT_TCP_KEEPIDLE, 120L);
    curl_easy_setopt(handle, CURLOPT_TCP_KEEPINTVL, 60L);
    curl_easy_setopt(handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");
    curl_easy_setopt(handle, CURLOPT_HTTPHEADER, nullptr);

    if (!data.empty()) {
        curl_easy_setopt(handle, CURLOPT_POSTFIELDS, data.c_str());
        curl_easy_setopt(handle, CURLOPT_POSTFIELDSIZE, data.size());
    }

    if (!headers.empty()) {
        struct curl_slist *chunk = nullptr;
        for (const auto &header : headers) {
            chunk = curl_slist_append(chunk, (header.first + ": " + header.second).c_str());
        }
        curl_easy_setopt(handle, CURLOPT_HTTPHEADER, chunk);
    }

    addHandle(handle, context);
}

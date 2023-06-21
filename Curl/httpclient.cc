#include "httpclient.hpp"

auto HttpClient::sendCustomRequest(const std::string &url,
                                   const std::string &method,
                                   const std::string &data,
                                   const Headers &headers) -> std::string
{
    m_buffer.clear();
    m_headers = nullptr;

    curl_easy_setopt(m_curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(m_curl, CURLOPT_CUSTOMREQUEST, method.c_str());
    curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, data.c_str());
    curl_easy_setopt(m_curl, CURLOPT_WRITEFUNCTION, writeCallback);
    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, this);
    curl_easy_setopt(m_curl, CURLOPT_HEADERFUNCTION, headerCallback);
    curl_easy_setopt(m_curl, CURLOPT_HEADERDATA, this);

    for (const auto &[key, value] : headers) {
        m_headers = curl_slist_append(m_headers, (key + ": " + value).c_str());
    }
    curl_easy_setopt(m_curl, CURLOPT_HTTPHEADER, m_headers);

    m_res = curl_easy_perform(m_curl);
    if (m_res != CURLE_OK) {
        throw std::runtime_error(curl_easy_strerror(m_res));
    }

    return m_buffer;
}
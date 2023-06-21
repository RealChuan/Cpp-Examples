#include "tcpclient.hpp"

TcpClient::TcpClient()
    : m_host()
    , m_port(0)
    , m_curl(nullptr)
    , m_res(CURLE_OK)
    , m_connected(false)
{}

TcpClient::TcpClient(const std::string &host, int port)
    : m_host(host)
    , m_port(port)
    , m_curl(nullptr)
    , m_res(CURLE_OK)
    , m_connected(false)
{}

TcpClient::~TcpClient()
{
    disconnect();
}

auto TcpClient::connect() -> bool
{
    if (m_connected) {
        return true;
    }

    m_curl = curl_easy_init();
    if (!m_curl) {
        return false;
    }

    curl_easy_setopt(m_curl, CURLOPT_URL, m_host.c_str());
    curl_easy_setopt(m_curl, CURLOPT_PORT, m_port);
    curl_easy_setopt(m_curl, CURLOPT_TCP_NODELAY, 1L);
    curl_easy_setopt(m_curl, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(m_curl, CURLOPT_TCP_KEEPIDLE, 120L);
    curl_easy_setopt(m_curl, CURLOPT_TCP_KEEPINTVL, 60L);

    m_res = curl_easy_perform(m_curl);
    if (m_res != CURLE_OK) {
        curl_easy_cleanup(m_curl);
        m_curl = nullptr;
        return false;
    }

    m_connected = true;
    return m_connected;
}

auto TcpClient::connect(const std::string &host, int port) -> bool
{
    m_host = host;
    m_port = port;
    return connect();
}

void TcpClient::disconnect()
{
    if (!m_connected) {
        return;
    }

    curl_easy_cleanup(m_curl);
    m_curl = nullptr;
    m_connected = false;
}

void TcpClient::send(const std::string &data)
{
    if (!m_connected) {
        return;
    }

    curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, data.c_str());
    curl_easy_setopt(m_curl, CURLOPT_POSTFIELDSIZE, data.size());

    m_res = curl_easy_perform(m_curl);
    if (m_res != CURLE_OK) {
        disconnect();
        return;
    }
}

void TcpClient::send(const std::vector<char> &data)
{
    if (!m_connected) {
        return;
    }

    curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, data.data());
    curl_easy_setopt(m_curl, CURLOPT_POSTFIELDSIZE, data.size());

    m_res = curl_easy_perform(m_curl);
    if (m_res != CURLE_OK) {
        disconnect();
        return;
    }
}

void TcpClient::send(const char *data, size_t size)
{
    if (!m_connected) {
        return;
    }

    curl_easy_setopt(m_curl, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(m_curl, CURLOPT_POSTFIELDSIZE, size);

    m_res = curl_easy_perform(m_curl);
    if (m_res != CURLE_OK) {
        disconnect();
        return;
    }
}

auto TcpClient::recv() -> std::string
{
    if (!m_connected) {
        return std::string();
    }

    std::string data;
    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &data);

    m_res = curl_easy_perform(m_curl);
    if (m_res != CURLE_OK) {
        disconnect();
        return std::string();
    }

    return data;
}

auto TcpClient::recv(size_t size) -> std::string
{
    if (!m_connected) {
        return std::string();
    }

    std::string data;
    data.resize(size);
    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &data);

    m_res = curl_easy_perform(m_curl);
    if (m_res != CURLE_OK) {
        disconnect();
        return std::string();
    }

    return data;
}

auto TcpClient::recv(std::vector<char> &data) -> size_t
{
    if (!m_connected) {
        return 0;
    }

    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, &data);

    m_res = curl_easy_perform(m_curl);
    if (m_res != CURLE_OK) {
        disconnect();
        return 0;
    }

    return data.size();
}

auto TcpClient::recv(char *data, size_t size) -> size_t
{
    if (!m_connected) {
        return 0;
    }

    curl_easy_setopt(m_curl, CURLOPT_WRITEDATA, data);

    m_res = curl_easy_perform(m_curl);
    if (m_res != CURLE_OK) {
        disconnect();
        return 0;
    }

    return size;
}

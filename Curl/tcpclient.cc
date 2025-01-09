#include "tcpclient.hpp"

class TcpClient::TcpClientPrivate
{
public:
    explicit TcpClientPrivate(TcpClient *q)
        : q_ptr(q)
    {}

    TcpClient *q_ptr;

    std::string host = {};
    int port = 0;

    CURL *curl = nullptr;
    CURLcode res = CURLE_OK;

    bool connected = false;
};

TcpClient::TcpClient()
    : d_ptr(std::make_unique<TcpClientPrivate>(this))
{}

TcpClient::TcpClient(const std::string &host, int port)
    : d_ptr(std::make_unique<TcpClientPrivate>(this))
{
    d_ptr->host = host;
    d_ptr->port = port;
}

TcpClient::~TcpClient()
{
    disconnect();
}

auto TcpClient::connect() -> bool
{
    if (d_ptr->connected) {
        return true;
    }

    d_ptr->curl = curl_easy_init();
    if (d_ptr->curl == nullptr) {
        return false;
    }

    curl_easy_setopt(d_ptr->curl, CURLOPT_URL, d_ptr->host.c_str());
    curl_easy_setopt(d_ptr->curl, CURLOPT_PORT, d_ptr->port);
    curl_easy_setopt(d_ptr->curl, CURLOPT_TCP_NODELAY, 1L);
    curl_easy_setopt(d_ptr->curl, CURLOPT_TCP_KEEPALIVE, 1L);
    curl_easy_setopt(d_ptr->curl, CURLOPT_TCP_KEEPIDLE, 120L);
    curl_easy_setopt(d_ptr->curl, CURLOPT_TCP_KEEPINTVL, 60L);

    d_ptr->res = curl_easy_perform(d_ptr->curl);
    if (d_ptr->res != CURLE_OK) {
        curl_easy_cleanup(d_ptr->curl);
        d_ptr->curl = nullptr;
        return false;
    }

    d_ptr->connected = true;
    return d_ptr->connected;
}

auto TcpClient::connect(const std::string &host, int port) -> bool
{
    d_ptr->host = host;
    d_ptr->port = port;
    return connect();
}

void TcpClient::disconnect()
{
    if (!d_ptr->connected) {
        return;
    }

    curl_easy_cleanup(d_ptr->curl);
    d_ptr->curl = nullptr;
    d_ptr->connected = false;
}

void TcpClient::send(const std::string &data)
{
    if (!d_ptr->connected) {
        return;
    }

    curl_easy_setopt(d_ptr->curl, CURLOPT_POSTFIELDS, data.c_str());
    curl_easy_setopt(d_ptr->curl, CURLOPT_POSTFIELDSIZE, data.size());

    d_ptr->res = curl_easy_perform(d_ptr->curl);
    if (d_ptr->res != CURLE_OK) {
        disconnect();
        return;
    }
}

void TcpClient::send(const std::vector<char> &data)
{
    if (!d_ptr->connected) {
        return;
    }

    curl_easy_setopt(d_ptr->curl, CURLOPT_POSTFIELDS, data.data());
    curl_easy_setopt(d_ptr->curl, CURLOPT_POSTFIELDSIZE, data.size());

    d_ptr->res = curl_easy_perform(d_ptr->curl);
    if (d_ptr->res != CURLE_OK) {
        disconnect();
        return;
    }
}

void TcpClient::send(const char *data, size_t size)
{
    if (!d_ptr->connected) {
        return;
    }

    curl_easy_setopt(d_ptr->curl, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(d_ptr->curl, CURLOPT_POSTFIELDSIZE, size);

    d_ptr->res = curl_easy_perform(d_ptr->curl);
    if (d_ptr->res != CURLE_OK) {
        disconnect();
        return;
    }
}

auto TcpClient::recv() -> std::string
{
    if (!d_ptr->connected) {
        return std::string();
    }

    std::string data;
    curl_easy_setopt(d_ptr->curl, CURLOPT_WRITEDATA, &data);

    d_ptr->res = curl_easy_perform(d_ptr->curl);
    if (d_ptr->res != CURLE_OK) {
        disconnect();
        return std::string();
    }

    return data;
}

auto TcpClient::recv(size_t size) -> std::string
{
    if (!d_ptr->connected) {
        return std::string();
    }

    std::string data;
    data.resize(size);
    curl_easy_setopt(d_ptr->curl, CURLOPT_WRITEDATA, &data);

    d_ptr->res = curl_easy_perform(d_ptr->curl);
    if (d_ptr->res != CURLE_OK) {
        disconnect();
        return std::string();
    }

    return data;
}

auto TcpClient::recv(std::vector<char> &data) -> size_t
{
    if (!d_ptr->connected) {
        return 0;
    }

    curl_easy_setopt(d_ptr->curl, CURLOPT_WRITEDATA, &data);

    d_ptr->res = curl_easy_perform(d_ptr->curl);
    if (d_ptr->res != CURLE_OK) {
        disconnect();
        return 0;
    }

    return data.size();
}

auto TcpClient::recv(char *data, size_t size) -> size_t
{
    if (!d_ptr->connected) {
        return 0;
    }

    curl_easy_setopt(d_ptr->curl, CURLOPT_WRITEDATA, data);

    d_ptr->res = curl_easy_perform(d_ptr->curl);
    if (d_ptr->res != CURLE_OK) {
        disconnect();
        return 0;
    }

    return size;
}

auto TcpClient::getHost() const -> std::string
{
    return d_ptr->host;
}
auto TcpClient::getPort() const -> int
{
    return d_ptr->port;
}

auto TcpClient::isConnected() const -> bool
{
    return d_ptr->connected;
}

auto TcpClient::getLastError() const -> CURLcode
{
    return d_ptr->res;
}

auto TcpClient::getLastErrorString() const -> std::string
{
    return curl_easy_strerror(d_ptr->res);
}
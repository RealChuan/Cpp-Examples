#include "tcpclient.hpp"

#include <iostream>

auto main() -> int
{
    TcpClient client("http://www.baidu.com", 80);
    if (!client.connect()) {
        std::cout << "connect failed: " << client.getLastErrorString() << std::endl;
        return 1;
    }

    client.send("GET / HTTP/1.1\r\nHost: www.baidu.com\r\n\r\n");
    std::cout << client.recv() << std::endl;

    return 0;
}

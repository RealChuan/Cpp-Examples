#include "httpclient.hpp"

#include <iostream>

auto main() -> int
{
    auto url = "http://www.baidu.com?name=hello&age=18";

    HttpClient client;

    // 为什么用了get方法，下面的其他方法都不输出了？从WireShark看，确实是发送了请求，也收到了响应，但是就是不输出
    // std::cout << "get_data: " << client.get("http://www.baidu.com") << std::endl;
    // std::cout << "--------------------------" << std::endl;

    std::cout << client.post(url, "hello world") << std::endl;
    std::cout << "--------------------------" << std::endl;

    std::cout << client.put(url, "hello world") << std::endl;
    std::cout << "--------------------------" << std::endl;

    std::cout << client.del(url) << std::endl;
    std::cout << "--------------------------" << std::endl;

    std::cout << client.options(url) << std::endl;
    std::cout << "--------------------------" << std::endl;

    std::cout << client.patch(url, "hello world") << std::endl;
    std::cout << "--------------------------" << std::endl;

    std::cout << client.sendCustomRequest(url, "GET", "hello world") << std::endl;

    return 0;
}

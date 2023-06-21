#include "httpclient.hpp"

#include <iostream>

auto main() -> int
{
    HttpClient client;

    // 为什么用了get方法，下面的其他方法都不输出了？从WireShark看，确实是发送了请求，也收到了响应，但是就是不输出
    // std::cout << "get_data: " << client.get("http://www.baidu.com") << std::endl;
    // std::cout << "--------------------------" << std::endl;

    std::cout << "post_data: "
              << client.post("http://www.baidu.com?name=hello&age=18", "hello world") << std::endl;
    std::cout << "--------------------------" << std::endl;

    std::cout << "put_data: " << client.put("http://www.baidu.com?name=hello&age=18", "hello world")
              << std::endl;
    std::cout << "--------------------------" << std::endl;

    std::cout << "del_data: " << client.del("http://www.baidu.com?name=hello&age=18") << std::endl;
    std::cout << "--------------------------" << std::endl;

    std::cout << "options_data: " << client.options("http://www.baidu.com?name=hello&age=18")
              << std::endl;
    std::cout << "--------------------------" << std::endl;

    std::cout << "patch_data: "
              << client.patch("http://www.baidu.com?name=hello&age=18", "hello world") << std::endl;
    std::cout << "--------------------------" << std::endl;

    std::cout << "custom_data: "
              << client.sendCustomRequest("http://www.baidu.com?name=hello&age=18",
                                          "GET",
                                          "hello world")
              << std::endl;

    return 0;
}

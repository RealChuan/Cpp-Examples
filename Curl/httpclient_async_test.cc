#include "httpclient_async.hpp"

#include <iostream>
#include <thread>

auto main() -> int
{
    auto url = "http://www.baidu.com?name=hello&age=18";

    HttpClientAsync client;

    // client.get("http://www.baidu.com", {}, [](const std::string &response) {
    //     std::cout << "GET response: " << std::endl;
    //     std::cout << response << std::endl;
    // });

    client.post(url, "hello world", {}, [](const std::string &response) {
        std::cout << "POST response: " << std::endl;
        std::cout << response << std::endl;
    });

    client.put(url, "hello world", {}, [](const std::string &response) {
        std::cout << "PUT response: " << std::endl;
        std::cout << response << std::endl;
    });

    client.del(url, {}, [](const std::string &response) {
        std::cout << "DELETE response: " << std::endl;
        std::cout << response << std::endl;
    });

    client.options(url, {}, [](const std::string &response) {
        std::cout << "OPTIONS response: " << std::endl;
        std::cout << response << std::endl;
    });

    client.patch(url, "hello world", {}, [](const std::string &response) {
        std::cout << "PATCH response: " << std::endl;
        std::cout << response << std::endl;
    });

    client.sendCustomRequest(url, "GET", "", {}, [](const std::string &response) {
        std::cout << "Custom request response: " << std::endl;
        std::cout << response << std::endl;
    });

    std::this_thread::sleep_for(std::chrono::seconds(5));

    return 0;
}

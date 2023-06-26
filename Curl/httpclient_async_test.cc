#include "httpclient_async.hpp"

auto main() -> int
{
    HttpClientAsync client;

    client.get("http://www.baidu.com", {}, [](const std::string &response) {
        std::cout << "GET response: " << std::endl;
        std::cout << response << std::endl;
    });

    client.post("http://www.baidu.com",
                "hello world",
                {{"Content-Type", "text/plain"}},
                [](const std::string &response) {
                    std::cout << "POST response: " << std::endl;
                    std::cout << response << std::endl;
                });

    client.put("http://www.baidu.com",
               "hello world",
               {{"Content-Type", "text/plain"}},
               [](const std::string &response) {
                   std::cout << "PUT response: " << std::endl;
                   std::cout << response << std::endl;
               });

    client.del("http://www.baidu.com", {}, [](const std::string &response) {
        std::cout << "DELETE response: " << std::endl;
        std::cout << response << std::endl;
    });

    client.options("http://www.baidu.com", {}, [](const std::string &response) {
        std::cout << "OPTIONS response: " << std::endl;
        std::cout << response << std::endl;
    });

    client.patch("http://www.baidu.com",
                 "hello world",
                 {{"Content-Type", "text/plain"}},
                 [](const std::string &response) {
                     std::cout << "PATCH response: " << std::endl;
                     std::cout << response << std::endl;
                 });

    client.sendCustomRequest("http://www.baidu.com", "GET", "", {}, [](const std::string &response) {
        std::cout << "Custom request response: " << std::endl;
        std::cout << response << std::endl;
    });

    std::this_thread::sleep_for(std::chrono::seconds(3));

    return 0;
}

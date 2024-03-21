#pragma once

// #define OPENSSL_NO_DEPRECATED

#include <openssl/err.h>
#include <openssl/opensslv.h>

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

// openssl version
void openssl_version()
{
    std::cout << "openssl version: " << OpenSSL_version(OPENSSL_VERSION) << '\n';
    std::cout << "openssl version number: " << OpenSSL_version_num() << '\n';
}

// openssl error
void openssl_error()
{
    auto error = ERR_get_error();
    std::cout << "openssl error: " << ERR_error_string(error, nullptr) << '\n';
    std::cout << "openssl error reason: " << ERR_reason_error_string(error) << '\n';
    std::cout << "openssl error library: " << ERR_lib_error_string(error) << '\n';
}

auto toHex(const std::string &str) -> std::string
{
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (auto c : str) {
        ss << std::setw(2) << static_cast<int>(c);
    }
    return ss.str();
}

auto fromHex(const std::string &str) -> std::string
{
    std::string ret;
    for (size_t i = 0; i < str.size(); i += 2) {
        std::string hex(str.substr(i, 2));
        ret.push_back(static_cast<char>(std::stoi(hex, nullptr, 16)));
    }
    return ret;
}

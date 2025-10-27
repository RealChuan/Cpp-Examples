#include "openssl_common.hpp"

#include <iomanip>
#include <sstream>

std::string toHex(const std::string &str)
{
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (unsigned char c : str) {
        ss << std::setw(2) << static_cast<int>(c);
    }
    return ss.str();
}

std::string fromHex(const std::string &str)
{
    std::string result;
    for (size_t i = 0; i < str.length(); i += 2) {
        std::string byteString = str.substr(i, 2);
        char byte = static_cast<char>(std::stoi(byteString, nullptr, 16));
        result.push_back(byte);
    }
    return result;
}

void handleOpenSSLError()
{
    char errorBuffer[256];
    ERR_error_string_n(ERR_get_error(), errorBuffer, sizeof(errorBuffer));
    throw std::runtime_error(std::string("OpenSSL error: ") + errorBuffer);
}

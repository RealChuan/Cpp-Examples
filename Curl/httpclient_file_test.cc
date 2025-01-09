#include "file_utils.hpp"
#include "httpclient.hpp"

#include <gtest/gtest.h>

#include <utility>

std::vector<std::string> paths = {"", "curl", "curl/1", "curl/1/2", "curl/1/2/3"};

void test_download(const std::string &filename)
{
    auto filepath = std::filesystem::current_path() / filename;
    auto url = "http://127.0.0.1:8080/files/" + filename;
    HttpClient httpClient;
    httpClient.download(url, filepath);
}

void test_upload_put(const std::string &filename)
{
    auto filepath = std::filesystem::current_path() / filename;
    auto url = "http://127.0.0.1:8080/files/" + filename;
    HttpClient httpClient;
    httpClient.upload_put(url, filepath);
}

void test_upload_post(const std::string &path, const std::string &filename)
{
    auto filepath = std::filesystem::current_path() / filename;
    std::string url = "http://127.0.0.1:8080/files";
    if (!path.empty()) {
        url += "/" + path;
    }
    HttpClient httpClient;
    httpClient.upload_post(url, filepath);
}

void test_delete(const std::string &filename)
{
    auto url = "http://127.0.0.1:8080/files/" + filename;
    HttpClient httpClient;
    httpClient.del(url);
}

TEST(UploadTest, UploadPutFile)
{
    auto filename = "curl_upload_put_file.txt";
    auto data = "curl upload put file data";
    for (const auto &path : std::as_const(paths)) {
        std::string filepath = filename;
        std::string filedata = data;
        if (!path.empty()) {
            filepath = path + "/" + filename;
            filedata += " " + path;
        }

        test_delete(filepath);

        createFile(filepath, filedata);
        test_upload_put(filepath);

        removeFile(filepath);
        test_download(filepath);
        assertFileData(filepath, filedata);
    }
}

TEST(UploadTest, UploadPostFile)
{
    auto filename = "curl_upload_post_file.txt";
    auto data = "curl upload post file data";
    for (const auto &path : std::as_const(paths)) {
        std::string filepath = filename;
        std::string filedata = data;
        if (!path.empty()) {
            filepath = path + "/" + filename;
            filedata += " " + path;
        }
        test_delete(filepath);

        createFile(filepath, filedata);
        test_upload_post(path, filepath);

        removeFile(filepath);

        test_download(filepath);
        assertFileData(filepath, filedata);
    }
}

auto main(int argc, char *argv[]) -> int
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

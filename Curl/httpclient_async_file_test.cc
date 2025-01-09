#include "file_utils.hpp"
#include "httpclient_async.hpp"

#include <gtest/gtest.h>

#include <thread>
#include <utility>

static HttpClientAsync httpClientAsync;
static std::condition_variable cv;
static std::mutex mutex;

static std::vector<std::string> paths = {"", "curl", "curl/1", "curl/1/2", "curl/1/2/3"};

void wait()
{
    std::unique_lock<std::mutex> lock(mutex);
    cv.wait(lock);
}

void test_download(const std::string &filename)
{
    auto filepath = std::filesystem::current_path() / filename;
    auto url = "http://127.0.0.1:8080/files/" + filename;

    httpClientAsync.download(url, filepath, {}, [](const std::string &) { cv.notify_one(); });
    wait();
    // 等待Content销毁，fstream完全关闭
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
}

void test_upload_put(const std::string &filename)
{
    auto filepath = std::filesystem::current_path() / filename;
    auto url = "http://127.0.0.1:8080/files/" + filename;
    httpClientAsync.upload_put(url, filepath, {}, [](const std::string &) { cv.notify_one(); });
    wait();
}

void test_upload_post(const std::string &path, const std::string &filename)
{
    auto filepath = std::filesystem::current_path() / filename;
    std::string url = "http://127.0.0.1:8080/files";
    if (!path.empty()) {
        url += "/" + path;
    }
    httpClientAsync.upload_post(url, filepath, {}, [](const std::string &) { cv.notify_one(); });
    wait();
}

void test_delete(const std::string &filename)
{
    auto url = "http://127.0.0.1:8080/files/" + filename;
    httpClientAsync.del(url, {}, [](const std::string &) { cv.notify_one(); });
    wait();
}

TEST(UploadTest, UploadPutFile)
{
    auto filename = "curl_async_upload_put_file.txt";
    auto data = "curl async upload put file data";
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
    auto filename = "curl_async_upload_post_file.txt";
    auto data = "curl async upload post file data";
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

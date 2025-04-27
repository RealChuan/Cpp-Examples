#include "file_utils.hpp"
#include "httpclient_async.hpp"

#include <gtest/gtest.h>

#include <thread>
#include <utility>

class AsyncHttpClientManager
{
public:
    static auto getHttpClientAsync() -> HttpClientAsync &
    {
        static HttpClientAsync httpClientAsync;
        return httpClientAsync;
    }

    static auto getConditionVariable() -> std::condition_variable &
    {
        static std::condition_variable conditionVariable;
        return conditionVariable;
    }

    static auto getMutex() -> std::mutex &
    {
        static std::mutex mutex;
        return mutex;
    }

    static auto getPaths() -> std::vector<std::string> &
    {
        static std::vector<std::string> paths = {"", "curl", "curl/1", "curl/1/2", "curl/1/2/3"};
        return paths;
    }
};

// 使用时
HttpClientAsync &httpClientAsync = AsyncHttpClientManager::getHttpClientAsync();
std::condition_variable &cv = AsyncHttpClientManager::getConditionVariable();
std::mutex &mutex = AsyncHttpClientManager::getMutex();
std::vector<std::string> &paths = AsyncHttpClientManager::getPaths();

void wait()
{
    std::unique_lock<std::mutex> lock(mutex);
    cv.wait(lock);
}

void test_download(const std::string &filename)
{
    const auto filepath = std::filesystem::current_path() / filename;
    const auto url = "http://127.0.0.1:8080/files/" + filename;

    httpClientAsync.download(url, filepath, {}, [](const std::string &) { cv.notify_one(); });
    wait();
    // 等待Content销毁，fstream完全关闭
    constexpr int kSleepDuration = 200;
    std::this_thread::sleep_for(std::chrono::milliseconds(kSleepDuration));
}

void test_upload_put(const std::string &filename)
{
    const auto filepath = std::filesystem::current_path() / filename;
    const auto url = "http://127.0.0.1:8080/files/" + filename;
    httpClientAsync.upload_put(url, filepath, {}, [](const std::string &) { cv.notify_one(); });
    wait();
}

void test_upload_post(const std::string &path, const std::string &filename)
{
    const auto filepath = std::filesystem::current_path() / filename;
    std::string url = "http://127.0.0.1:8080/files";
    if (!path.empty()) {
        url += "/" + path;
    }
    httpClientAsync.upload_post(url, filepath, {}, [](const std::string &) { cv.notify_one(); });
    wait();
}

void test_delete(const std::string &filename)
{
    const auto url = "http://127.0.0.1:8080/files/" + filename;
    httpClientAsync.del(url, {}, [](const std::string &) { cv.notify_one(); });
    wait();
}

TEST(UploadTest, UploadPutFile)
{
    const auto *filename = "curl_async_upload_put_file.txt";
    const auto *data = "curl async upload put file data";
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
    const auto *filename = "curl_async_upload_post_file.txt";
    const auto *data = "curl async upload post file data";
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

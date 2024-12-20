# Cpp-Examples

-   [Simplified Chinese](README.md)
-   [English](README.en.md)

## Code structure

1.  [Algorithm](/Algorithm/)
    1.  [Search](/Algorithm/Search/search.hpp)——Implementation of various search algorithms, as well as unit testing and performance testing based on Google benchmark;
        1.  [std_search_examples](/Algorithm/Search/std_search_examples.cc)——Examples of search algorithms in stl;
    2.  [Sort](/Algorithm/Sort/sort.hpp)——Implementation of various sorting algorithms, as well as unit testing and performance testing based on Google benchmark;
        1.  [std_sort_examples](/Algorithm/Sort/std_sort_examples.cc)——Examples of sorting algorithms in stl;
2.  [BinaryTree](/BinaryTree/binarytree.hpp)——Binary tree related operations, including insertion, removal, search, and printing;
3.  [Breakpad](/Breakpad/breakpad.hpp)——Simple encapsulation of google breakpad;
4.  [Exchange Order](/ByteOrder/byteorder.hpp)——Determine the byte order of the system;
5.  [Client](/Client/client.cpp)——A simple Linux select socket client;
6.  [CountDownLatch](/CountDownLatch/countdownlatch.hpp)——A simple countdown latch implemented using std::mutex and std::condition_variable (std::latch c++20);
7.  [Crashpad](/Crashpad/crashpad.hpp)——Simple encapsulation of google crashpad;
8.  [Curl](/Curl/)——Simple use of curl;
9.  [TcpClient](/Curl/tcpclient.hpp)——Simple tcp client implemented using curl;
10. [HttpClient](/Curl/httpclient.hpp)——Simple http synchronization client implemented using curl;
11. [HttpClientAsync](/Curl/httpclient_async.hpp)——Simple http asynchronous client implemented using curl;
12. [Design patterns](/DesignPattern)——Some examples of design patterns;
13. [Factory](/DesignPattern/Factory/factory.hpp)——Factory mode;
14. [MVC](/DesignPattern/MVC/model.hpp)——mvc mode;
15. [Observer](/DesignPattern/Observer/observer.hpp)——Observer mode;
16. [Singleton](/DesignPattern/Singleton/singleton.hpp)——Single case mode;
17. [GlobMatch](/GlobMatch/globmatcher.hpp)——Simple implementation of glob pattern matching;
18. [Hawthorn](/Glog/main.cc)——Google glog example;
19. [Icmp](/Icmp/icmp.hpp)——Simple encapsulation of linux icmp protocol;
20. [LinkedList](/LinkedList/linkedlist.hpp)——Related operations of linked lists, including insertion, removal, reversal, and printing;
21. [Memcpy](/Memcpy/memcpy.hpp)——`memcpy`function implementation;
22. [MonitorDir](/MonitorDir/monitordir.hpp)——windows(`ReadDirectoryChangesW`)，macos(`FSEvents`) and linux(`fanotify`and`inotify`) Simple example of directory monitoring;
    1.  `fanotify`Use global mode in`fanotify_mark`Join in`FAN_MARK_FILESYSTEM`this`flag`, all events on the specified file system will be monitored, and then the required events can be filtered according to the specified monitored folder directory. This function is better than`inotify`more powerful;
23. [Mutex](/Mutex/mutex.hpp)——Simple mutex lock and spin lock implemented using std::atomic_flag;
24. [OpenSSL](/OpenSSL)——Some examples of openssl;
    1.  [aes](/OpenSSL/openssl_aes.cc)——AES encryption and decryption examples;
    2.  [base64](/OpenSSL/openssl_base64.cc)——Examples of base64 encoding and decoding;
    3.  [hash](/OpenSSL/openssl_hash.cc)——Example of sha256;
    4.  [hmac](/OpenSSL/openssl_hmac.cc)——Example of hmac;
    5.  [pem](/OpenSSL/openssl_pem.cc)——Example of pem format;
    6.  [rsa](/OpenSSL/openssl_rsa.cc)——Examples of rsa encryption and decryption;
    7.  [sm4](/OpenSSL/openssl_sm4.cc)——Examples of sm4 encryption and decryption;
    8.  [Kskh09](/OpenSSL/openssl_x509.cc)——Example of x509 certificate;
    9.  [bash](/OpenSSL/openssl_bash.sh)——openssl command line example;
25. [Server](/Server)——Some examples of linux server;
    1.  [server_epoll](/Server/server_epoll.cc)——epoll example;
    2.  [server_poll](/Server/server_poll.cc)——Poll example;
    3.  [server_select](/Server/server_select.cc)——Example of select;
26. [Thread](/Thread/)——Thread class implemented based on std::thread, including thread pool;
    1.  [Thread](/Thread/thread.hpp)——Thread class;
    2.  [ThreadPool](/Thread/threadpool.hpp)——Thread pool;

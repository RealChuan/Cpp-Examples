# Cpp-Examples

-   [Simplified Chinese](README.md)
-   [English](README.en.md)

## Code structure

1.  [Algorithm](/Algorithm/)
    1.  [Search](/Algorithm/Search/search.hpp)——Implementation of various search algorithms, as well as unit testing and performance testing based on Google benchmark;
        1.  [std_search_examples](/Algorithm/Search/std_search_examples.cc)——Example of search algorithm in stl;
    2.  [Sort](/Algorithm/Sort/sort.hpp)——Implementation of various sorting algorithms, as well as unit testing and performance testing based on Google benchmark;
        1.  [std_sort_examples](/Algorithm/Sort/std_sort_examples.cc)——Example of sorting algorithm in stl;
2.  [BinaryTree](/BinaryTree/binarytree.hpp)——Related operations of binary trees, insert, remove, find, and print;
3.  [Breakpad](/Breakpad/breakpad.hpp)——A simple package of google breakpad;
4.  [Byte order](/ByteOrder/byteorder.hpp)——Judge the endianness of the system;
5.  [Client](/Client/client.cpp)——A simple Linux select socket client;
6.  [CountDownLatch](/CountDownLatch/countdownlatch.hpp)——Simple countdown latch (std::latch c++20) implemented using std::mutex and std::condition_variable;
7.  [Crashpad](/Crashpad/crashpad.hpp)——Simple encapsulation of google crashpad;
8.  [Curl](/Curl/)——Simple use of curl;
9.  [TcpClient](/Curl/tcpclient.hpp)——Simple tcp client implemented using curl;
10. [HttpClient](/Curl/httpclient.hpp)——Simple http synchronization client implemented using curl;
11. [HttpClientAsync](/Curl/httpclient_async.hpp)——Simple http asynchronous client implemented using curl;
12. [Design pattern](/DesignPattern)——Some examples of design patterns;
    1.  [Factory](/DesignPattern/Factory/factory.hpp)——Factory model;
    2.  [MVC](/DesignPattern/MVC/model.hpp)——MVC mode;
    3.  [Observer](/DesignPattern/Observer/observer.hpp)——Observer mode;
    4.  [Singleton](/DesignPattern/Singleton/singleton.hpp)——Singleton mode;
13. [GlobMatch](/GlobMatch/globmatcher.hpp)——Simple implementation of glob pattern matching;
14. [Glog](/Glog/main.cc)——Google glog example;
15. [Icmp](/Icmp/icmp.hpp)——Simple encapsulation of linux icmp protocol;
16. [LinkedList](/LinkedList/linkedlist.hpp)——Related operations of linked lists, insert, remove, invert, and print;
17. [Memcpy](/Memcpy/memcpy.hpp)——`memcpy`Function implementation;
18. [Is a monitor](/MonitorDir/monitordir.hpp)——windows(`ReadDirectoryChangesW`)，macos(`FSEvents`) and linux(`fanotify`and`inotify`) Simple examples of directory monitoring;
    1.  `fanotify`Used in`global`Mode, in`fanotify_mark`Join`FAN_MARK_FILESYSTEM`(need`CAP_SYS_ADMIN`ability, that is, root permissions)`flag`, All events on the specified file system can be monitored, and then the required events can be filtered according to the specified monitored folder directory. This function is more`inotify`More powerful;
        1.  Get the upper path where the event file is located, using`open_by_handle_at`This method will appear under ordinary users`Operation not permitted`Error, also required`CAP_SYS_ADMIN`Ability, that is, root permissions;
        2.  It is recommended to use root permissions to run. If you must run it under normal users, it is recommended to use it.`inotify`Instead`fanotify`, open a monitoring file descriptor (`fd`) cannot be achieved`subtree`monitor;
19. [MonitorDir_EFSW](/MonitorDir_EFSW/main.cc)——A simple example of directory monitoring implemented using efsw;
20. [OpenSSL](/OpenSSL)——Some examples of openssl;
    1.  [aes](/OpenSSL/openssl_aes.cc)——Aes encryption and decryption example;
    2.  [base64](/OpenSSL/openssl_base64.cc)——Example of base64 encoding and decoding;
    3.  [hash](/OpenSSL/openssl_hash.cc)——Sha256 example;
    4.  [hmac](/OpenSSL/openssl_hmac.cc)——The example of hmac;
    5.  [pem](/OpenSSL/openssl_pem.cc)——Example of pem format;
    6.  [rsa](/OpenSSL/openssl_rsa.cc)——Example of rsa encryption and decryption;
    7.  [SM4](/OpenSSL/openssl_sm4.cc)——Example of sm4 encryption and decryption;
    8.  [Dirt 09](/OpenSSL/openssl_x509.cc)——Example of x509 certificate;
    9.  [bash](/OpenSSL/openssl_bash.sh)——Example of the openssl command line;
21. [SafeCallback](/SafeCallback/safecallback.hpp)——Implementation of life cycle safe callback function, reference[muduo WeakCallback](https://github.com/chenshuo/muduo/blob/cpp17/muduo/base/WeakCallback.h)；
22. [Server](/Server)——Some examples of linux server;
    1.  [server_epoll](/Server/server_epoll.cc)——The example of epoll;
    2.  [server_poll](/Server/server_poll.cc)——A example of poll;
    3.  [server_select](/Server/server_select.cc)——Select example;
23. [SpinMutex](/SpinMutex)——Simple mutex and spin lock implemented using std::atomic_flag;
24. [Thread](/Thread/)——Thread class based on std::jthread implementation, including thread pool (`Apple Clang`Not supported);
    1.  [Thread](/Thread/thread.hpp)——Thread class;
    2.  [ThreadPool](/Thread/threadpool.hpp)——Thread pool;

# Cpp-Examples

- [简体中文](README.md)
- [English](README.en.md)

## 代码结构

1. [Algorithm](/Algorithm/)
   1. [Search](/Algorithm/Search/search.hpp)——各种查找算法的实现，以及单元测试和基于google benchmark的性能测试；
      1. [std_search_examples](/Algorithm/Search/std_search_examples.cc)——stl中查找算法的例子；
   2. [Sort](/Algorithm/Sort/sort.hpp)——各种排序算法的实现，以及单元测试和基于google benchmark的性能测试；
      1. [std_sort_examples](/Algorithm/Sort/std_sort_examples.cc)——stl中排序算法的例子；
2. [BinaryTree](/BinaryTree/binarytree.hpp)——二叉树的相关操作，插入、移除、查找、打印；
3. [Breakpad](/Breakpad/breakpad.hpp)——google breakpad的简单封装；
4. [ByteOrder](/ByteOrder/byteorder.hpp)——判断系统的字节序；
5. [Client](/Client/client.cpp)——一个简单的Linux select socket客户端；
6. [CountDownLatch](/CountDownLatch/countdownlatch.hpp)——使用std::mutex和std::condition_variable实现的简单倒计时门闩（std::latch c++20）；
7. [Crashpad](/Crashpad/crashpad.hpp)——google crashpad的简单封装;
8. [Curl](/Curl/)——curl的简单使用；
9. [TcpClient](/Curl/tcpclient.hpp)——使用curl实现的简单tcp客户端；
10. [HttpClient](/Curl/httpclient.hpp)——使用curl实现的简单http同步客户端；
11. [HttpClientAsync](/Curl/httpclient_async.hpp)——使用curl实现的简单http异步客户端；
12. [DesignPattern](/DesignPattern)——设计模式的一些例子；
13. [Factory](/DesignPattern/Factory/factory.hpp)——工厂模式；
14. [MVC](/DesignPattern/MVC/model.hpp)——mvc模式；
15. [Observer](/DesignPattern/Observer/observer.hpp)——观察者模式；
16. [Singleton](/DesignPattern/Singleton/singleton.hpp)——单例模式；
17. [Glog](/Glog/main.cc)——google glog的例子；
18. [Icmp](/Icmp/icmp.hpp)——linux icmp协议的简单封装；
19. [LinkedList](/LinkedList/linkedlist.hpp)——链表的相关操作，插入、移除、反转、打印；
20. [Memcpy](/Memcpy/memcpy.hpp)——`memcpy`函数实现；
21. [MonitorDir](/MonitorDir/monitordir.hpp)——windows(`ReadDirectoryChangesW`)，macos(`FSEvents`)和linux(`inotify`)目录监控的简单例子；
22. [Mutex](/Mutex/mutex.hpp)——使用std::atomic_flag实现的简单互斥锁和自旋锁；
23. [OpenSSL](/OpenSSL)——openssl的一些例子；
    1. [aes](/OpenSSL/openssl_aes.cc)——aes加解密的例子；
    2. [base64](/OpenSSL/openssl_base64.cc)——base64编解码的例子；
    3. [hash](/OpenSSL/openssl_hash.cc)——sha256的例子；
    4. [hmac](/OpenSSL/openssl_hmac.cc)——hmac的例子；
    5. [pem](/OpenSSL/openssl_pem.cc)——pem格式的例子；
    6. [rsa](/OpenSSL/openssl_rsa.cc)——rsa加解密的例子；
    7. [sm4](/OpenSSL/openssl_sm4.cc)——sm4加解密的例子；
    8. [x509](/OpenSSL/openssl_x509.cc)——x509证书的例子；
    9. [bash](/OpenSSL/openssl_bash.sh)——openssl命令行的例子；
24. [Server](/Server)——linux server的一些例子；
    1. [server_epoll](/Server/server_epoll.cc)——epoll的例子；
    2. [server_poll](/Server/server_poll.cc)——poll的例子；
    3. [server_select](/Server/server_select.cc)——select的例子；
25. [Thread](/Thread/)——基于std::thread实现的线程类，包括线程池；
    1. [Thread](/Thread/thread.hpp)——线程类；
    2. [ThreadPool](/Thread/threadpool.hpp)——线程池；

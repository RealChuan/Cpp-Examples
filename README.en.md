# Cpp-Examples

-   [Simplified Chinese](README.md)
-   [English](README.en.md)

## Some obsolete C/C++ code

1.  [Algorithm](./Algorithm/)
    1.  [Search](./Algorithm/Search/search.hpp)----------> Implementation of various search algorithms, as well as unit testing and performance testing based on Google Benchmark;
        1.  [std_search_examples](./Algorithm/Search/std_search_examples.cc)---------->Example of search algorithm in STL;
    2.  [Sort](./Algorithm/Sort/sort.hpp)----------> Implementation of various sorting algorithms, as well as unit testing and performance testing based on Google Benchmark;
        1.  [std_sort_examples](./Algorithm/Sort/std_sort_examples.cc)----------> Examples of sorting algorithms in STL;
2.  [AtomicQueue](./AtomicQueue/atomicqueue.hpp)---------->A thread-safe queue implemented using std::atomic;
3.  [BinaryTree](./BinaryTree/binarytree.hpp)---------->Binary tree related operations, insert, remove, search, print;
4.  [Client](./Client/client.cpp)----------> A simple Linux select socket client;
5.  [CountDownLatch](./CountDownLatch/countdownlatch.hpp)----------> Simple countdown latch implemented using std::mutex and std::condition_variable;
6.  [Breakpad](./Breakpad/breakpad.hpp)----------> Simple package of Google Breakpad;
7.  [Design pattern](./DesignPattern)----------> Some examples of design patterns;
    1.  [Factory](./DesignPattern/Factory/factory.hpp)----------> factory mode;
    2.  [MVC](./DesignPattern/MVC/model.hpp)---------->MVC mode;
    3.  [Observer](./DesignPattern/Observer/observer.hpp)----------> Observer mode;
    4.  [Singleton](./DesignPattern/Singleton/singleton.hpp)----------> singleton mode;
8.  [Hawthorn](./Glog/main.cc)---------->Google Glog example;
9.  [Icmp](./Icmp/icmp.hpp)----------> Simple encapsulation of Linux ICMP protocol;
10. [LinkedList](./LinkedList/linkedlist.hpp)----------> Linked list related operations, insert, remove, reverse, print;
11. [MonitoringDirectory](./MonitoringDirectory/monitoring_directory.hpp)---------->Simple encapsulation of file changes in the Windows monitoring directory;
12. [Mutex](./Mutex/mutex.hpp)----------> Simple mutex and spin lock implemented using std::atomic_flag;
13. [Server](./Server)---------->Some examples of Linux Server;
14. [server_epoll](./Server/server_epoll.cc)---------->Epoll example;
15. [server_poll](./Server/server_poll.cc)----------> Poll example;
16. [server_select](./Server/server_select.cc)---------->Select example;
17. [Thread](./Thread/)---------->Thread class implemented based on std::thread, including thread pool;
    1.  [Thread](./Thread/thread.hpp)----------> thread class;
    2.  [ThreadPool](./Thread/threadpool.hpp)----------> thread pool;

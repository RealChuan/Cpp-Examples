# Cpp-Examples

- [简体中文](README.md)
- [English](README.en.md)

## 一些过时的C/C++代码

1. [Algorithm](./Algorithm/)
   1. [Search](./Algorithm/Search/search.hpp)---------->各种查找算法的实现，以及单元测试和基于Google Benchmark的性能测试；
      1. [std_search_examples](./Algorithm/Search/std_search_examples.cc)---------->STL中查找算法的例子；
   2. [Sort](./Algorithm/Sort/sort.hpp)---------->各种排序算法的实现，以及单元测试和基于Google Benchmark的性能测试；
      1. [std_sort_examples](./Algorithm/Sort/std_sort_examples.cc)---------->STL中排序算法的例子；
2. [AtomicQueue](./AtomicQueue/atomicqueue.hpp)---------->使用std::atomic实现的线程安全的队列；
3. [BinaryTree](./BinaryTree/binarytree.hpp)---------->二叉树的相关操作，插入、移除、查找、打印；
4. [Client](./Client/client.cpp)---------->一个简单的Linux select socket客户端；
5. [Breakpad](./Breakpad/breakpad.hpp)---------->Google Breakpad的简单封装；
6. [DesignPattern](./DesignPattern)---------->设计模式的一些例子；
   1. [Factory](./DesignPattern/Factory/factory.hpp)---------->工厂模式；
   2. [MVC](./DesignPattern/MVC/model.hpp)---------->MVC模式；
   3. [Observer](./DesignPattern/Observer/observer.hpp)---------->观察者模式；
   4. [Singleton](./DesignPattern/Singleton/singleton.hpp)---------->单例模式；
7. [Glog](./Glog/main.cc)---------->Google Glog的例子；
8. [Icmp](./Icmp/icmp.hpp)---------->Linux ICMP协议的简单封装；
9. [LinkedList](./LinkedList/linkedlist.hpp)---------->链表的相关操作，插入、移除、反转、打印；
10. [MonitoringDirectory](./MonitoringDirectory/monitoring_directory.hpp)---------->Windows 监控目录中文件变化的简单封装；
11. [Mutex](./Mutex/mutex.hpp)---------->使用std::atomic_flag实现的简单互斥锁和自旋锁；
12. [Server](./Server)---------->Linux Server的一些例子；
   1. [server_epoll](./Server/server_epoll.cc)---------->Epoll的例子；
   2. [server_poll](./Server/server_poll.cc)---------->Poll的例子；
   3. [server_select](./Server/server_select.cc)---------->Select的例子；
13. [Thread](./Thread/)---------->基于std::thread实现的线程类，包括线程池；
    1.  [Thread](./Thread/thread.hpp)---------->线程类；
    2.  [ThreadPool](./Thread/threadpool.hpp)---------->线程池；

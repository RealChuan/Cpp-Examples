# Cpp-Examples

- [简体中文](README.md)
- [English](README.en.md)

## 一些过时的C/C++代码

1. [AtomicQueue](./AtomicQueue/atomicqueue.hpp)---------->使用std::atomic实现的线程安全的队列；
2. [Client](./Client/client.cpp)---------->一个简单的Linux select socket客户端；
3. [Breakpad](./Breakpad/breakpad.hpp)---------->Google Breakpad的简单封装；
4. [DesignPattern](./DesignPattern)---------->设计模式的一些例子；
   1. [Factory](./DesignPattern/Factory/factory.hpp)---------->工厂模式；
   2. [MVC](./DesignPattern/MVC/model.hpp)---------->MVC模式；
   3. [Observer](./DesignPattern/Observer/observer.hpp)---------->观察者模式；
   4. [Singleton](./DesignPattern/Singleton/singleton.hpp)---------->单例模式；
5. [Glog](./Glog/main.cc)---------->Google Glog的例子；
6. [Icmp](./Icmp/icmp.hpp)---------->Linux ICMP协议的简单封装；
7. [LinkedList](./LinkedList/linkedlist.hpp)---------->链表的相关操作，插入、移除、反转、打印；
8. [MonitoringDirectory](./MonitoringDirectory/monitoring_directory.hpp)---------->Windows 监控目录中文件变化的简单封装；
9. [Mutex](./Mutex/mutex.hpp)---------->使用std::atomic_flag实现的简单互斥锁和自旋锁；
10. [Server](./Server)---------->Linux Server的一些例子；
   1. [server_epoll](./Server/server_epoll.cc)---------->Epoll的例子；
   2. [server_poll](./Server/server_poll.cc)---------->Poll的例子；
   3. [server_select](./Server/server_select.cc)---------->Select的例子；

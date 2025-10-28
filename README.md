# Cpp-Examples

一个包含多种 C++ 实用示例的代码库。

- [简体中文](README.md)
- [English](README.en.md)

## 项目概述

这个项目收集了各种 C++ 编程中的实用示例，涵盖了系统编程、加密算法、多线程、崩溃处理、设计模式等多个领域。每个示例都力求简洁明了，便于学习和在实际项目中使用。

## 模块详情

### 1. [Breakpad](src/Breakpad/)

Google Breakpad 的简单封装，用于应用程序崩溃报告和堆栈跟踪。

- **核心文件**:
  - `breakpad.hpp` / `breakpad.cc` - Breakpad 封装类
  - `example.cc` - 使用示例
  - `breakpad_unittest.cc` - 单元测试

### 2. [ByteOrder](src/ByteOrder/)

系统字节序检测工具。

- **核心文件**:
  - `byteorder.cc` -  使用多种方法判断系统的字节序（大端序/小端序）

### 3. [Crashpad](src/Crashpad/)

Google Crashpad 的简单封装，现代化的崩溃报告系统。

- **核心文件**:
  - `crashpad.hpp` / `crashpad.cc` - Crashpad 封装类
  - `example.cc` - 使用示例
  - `crashpad_server.py` - 崩溃报告服务器脚本

### 4. [Memcpy](src/Memcpy/)

内存拷贝函数实现及相关测试。

- **核心文件**:
  - `mymemcpy.hpp` / `mymemcpy.cc` - 自定义 memcpy 函数实现
  - `mymemcpy_unittest.cc` - 单元测试

### 5. [MonitorDir](src/MonitorDir/)

跨平台目录监控实现，支持 Windows、macOS 和 Linux。

- **核心文件**:
  - `monitordir.hpp` - 目录监控接口
  - `monitordir_win.cc` - Windows 实现（使用 `ReadDirectoryChangesW`）
  - `monitordir_mac.cc` - macOS 实现（使用 `FSEvents`）
  - `monitordir_linux_inotify.cc` - Linux inotify 实现
  - `monitordir_linux_fanotify.cc` - Linux fanotify 实现
  - `main.cc` - 使用示例

**Linux 平台说明**:

- `fanotify` 使用全局模式（`FAN_MARK_FILESYSTEM`），可以监控整个文件系统的事件
- 需要 `CAP_SYS_ADMIN` 能力（root 权限）
- 普通用户建议使用 `inotify` 实现

### 6. [MonitorDir_EFSW](src/MonitorDir_EFSW/)

基于 efsw 库的目录监控示例。

- **核心文件**:
  - `main.cc` - 使用 efsw 实现的目录监控示例

### 7. [OpenSSL](src/OpenSSL/)

OpenSSL 加密算法使用示例。

- **核心文件**:
  - `openssl_common.hpp` / `openssl_common.cc` - 公共工具函数
  - `openssl_aes.cc` - AES 加解密示例
  - `openssl_base64.cc` - Base64 编解码示例
  - `openssl_hash.cc` - SHA256 哈希计算示例
  - `openssl_rsa.cc` - RSA 加解密示例

### 8. [Singleton](src/Singleton/)

现代化线程安全的单例模式实现，使用 C++11 特性确保跨平台兼容性。

- **核心文件**:
  - `singleton.hpp` - 模板化的单例基类，提供线程安全的实例访问
  - `singleton_unitest.cc` - 完整的单元测试，验证单例的唯一性、线程安全性等特性

### 9. [Thread](src/Thread/)

基于 std::jthread 实现的线程和线程池（注意：Apple Clang 不支持）。

- **核心文件**:
  - `thread.hpp` - 线程类封装
  - `threadpool.hpp` - 线程池实现
  - `queue.hpp` - 线程安全队列
  - `thread_unittest.cc` - 线程单元测试
  - `threadpool_unittest.cc` - 线程池单元测试
  - `queue_unittest.cc` - 队列单元测试

### 10. [utils](src/utils/)

通用工具类。

- **核心文件**:
  - `scopeguard.hpp` - RAII 范围守卫
  - `object.hpp` - 对象工具类
  - `utils.hpp` / `utils.cc` - 通用工具函数
  -

s

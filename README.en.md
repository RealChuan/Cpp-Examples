# Cpp-Examples

A library of practical C++ examples.

-   [Simplified Chinese](README.md)
-   [English](README.en.md)

## 项目概述

This project collects practical examples in various C++ programming, covering many fields such as system programming, encryption algorithms, multi-threading, crash handling, design patterns, etc. Each example strives to be concise and clear, making it easy to learn and use in actual projects.

## Module details

### 1.[Breakpad](src/Breakpad/)

A simple wrapper for Google Breakpad for application crash reporting and stack tracing.

-   **core file**:
    -   `breakpad.hpp`/`breakpad.cc`- Breakpad encapsulation class
    -   `example.cc`- Usage examples
    -   `breakpad_unittest.cc`- unit testing

### 2.[Exchange Order](src/ByteOrder/)

System byte order detection tool.

-   **core file**:
    -   `byteorder.cc`- Use multiple methods to determine the byte order of the system (big endian/little endian)

### 3.[Crashpad](src/Crashpad/)

A simple wrapper for Google Crashpad, a modern crash reporting system.

-   **core file**:
    -   `crashpad.hpp`/`crashpad.cc`- Crashpad encapsulation class
    -   `example.cc`- Usage examples
    -   `crashpad_server.py`- Crash reporting server script

### 4.[Memcpy](src/Memcpy/)

Memory copy function implementation and related tests.

-   **core file**:
    -   `mymemcpy.hpp`/`mymemcpy.cc`- Custom memcpy function implementation
    -   `mymemcpy_unittest.cc`- unit testing

### 5.[MonitorDir](src/MonitorDir/)

Cross-platform directory monitoring implementation, supporting Windows, macOS and Linux.

-   **core file**:
    -   `monitordir.hpp`- Directory monitoring interface
    -   `monitordir_win.cc`- Windows implementation (using`ReadDirectoryChangesW`）
    -   `monitordir_mac.cc`- macOS implementation (using`FSEvents`）
    -   `monitordir_linux_inotify.cc`- Linux inotify implementation
    -   `monitordir_linux_fanotify.cc`- Linux fanotify implementation
    -   `main.cc`- Usage examples

**Linux platform instructions**:

-   `fanotify`Use global mode (`FAN_MARK_FILESYSTEM`), can monitor events of the entire file system
-   need`CAP_SYS_ADMIN`Capabilities (root permissions)
-   Recommended for ordinary users`inotify`accomplish

### 6.[MonitorDir_EFSW](src/MonitorDir_EFSW/)

Directory monitoring example based on efsw library.

-   **core file**:
    -   `main.cc`- Directory monitoring example using efsw

### 7.[OpenSSL](src/OpenSSL/)

OpenSSL encryption algorithm usage example.

-   **core file**:
    -   `openssl_common.hpp`/`openssl_common.cc`- Public utility functions
    -   `openssl_aes.cc`- AES encryption and decryption example
    -   `openssl_base64.cc`- Base64 encoding and decoding example
    -   `openssl_hash.cc`- SHA256 hash calculation example
    -   `openssl_rsa.cc`- RSA encryption and decryption example

### 8.[Singleton](src/Singleton/)

A modern thread-safe implementation of the Singleton pattern using C++11 features to ensure cross-platform compatibility.

-   **core file**:
    -   `singleton.hpp`- Templated singleton base class, providing thread-safe instance access
    -   `singleton_unitest.cc`- Complete unit testing to verify the uniqueness, thread safety and other characteristics of the singleton

### 9.[Thread](src/Thread/)

Threads and thread pools implemented based on std::jthread (note: not supported by Apple Clang).

-   **core file**:
    -   `thread.hpp`- Thread class encapsulation
    -   `threadpool.hpp`-Thread pool implementation
    -   `queue.hpp`- Thread safe queue
    -   `thread_unittest.cc`- Thread unit testing
    -   `threadpool_unittest.cc`- Thread pool unit testing
    -   `queue_unittest.cc`- Queue unit testing

### 10.[utils](src/utils/)

General tool class.

-   **core file**:
    -   `scopeguard.hpp`- RAII range guard
    -   `object.hpp`- Object tool class
    -   `utils.hpp`/`utils.cc`- Common tool functions
    -

s

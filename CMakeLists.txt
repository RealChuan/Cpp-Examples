# 设定版本号
cmake_minimum_required(VERSION 3.5)

if(CMAKE_HOST_WIN32)
  set(CMAKE_TOOLCHAIN_FILE
      "C:/vcpkg/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "Vcpkg toolchain file")
elseif(CMAKE_HOST_APPLE)
  set(CMAKE_TOOLCHAIN_FILE
      "/usr/local/share/vcpkg/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "Vcpkg toolchain file")
elseif(CMAKE_HOST_UNIX)
  set(CMAKE_TOOLCHAIN_FILE
      "/usr/local/share/vcpkg/scripts/buildsystems/vcpkg.cmake"
      CACHE STRING "Vcpkg toolchain file")
endif()

# 设定工程名
project(
  Cpp-Example
  VERSION 0.1
  LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CURRENT_SOURCE_DIR ON)

# set(CMAKE_CXX_FLAGS_DEBUG "-O0") set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_DEBUG_POSTFIX d)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message("Setting build type to 'RelWithDebInfo' as none was specified.")
  set(CMAKE_BUILD_TYPE
      RelWithDebInfo
      CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release"
                                               "MinSizeRel" "RelWithDebInfo")
endif()

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(CURRENT_PLATFORM "-64")
else()
  set(CURRENT_PLATFORM "-32")
endif()

message(STATUS "Current Platform is ${CURRENT_PLATFORM}")

# 设定可执行二进制文件的目录
set(EXECUTABLE_OUTPUT_PATH
    ${PROJECT_SOURCE_DIR}/bin${CURRENT_PLATFORM}/${CMAKE_BUILD_TYPE}) # 源文件目录
# SET(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)     #输出文件目录

# 设定存放编译出来的库文件的目录
set(LIBRARY_OUTPUT_PATH
    ${PROJECT_SOURCE_DIR}/lib${CURRENT_PLATFORM}/${CMAKE_BUILD_TYPE})
# SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

# 并且把该目录设为连接目录
link_directories(
  ${PROJECT_SOURCE_DIR}/lib${CURRENT_PLATFORM}/${CMAKE_BUILD_TYPE})
# LINK_DIRECTORIES(${CMAKE_BINARY_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR})

message("CMAKE_MAJOR_VERSION: ${CMAKE_MAJOR_VERSION}")
message("CMAKE_MINOR_VERSION: ${CMAKE_MINOR_VERSION}")
message("CMAKE_PATCH_VERSION: ${CMAKE_PATCH_VERSION}")
message("CMAKE_TWEAK_VERSION: ${CMAKE_TWEAK_VERSION}")
message("CMAKE_VERSION: ${CMAKE_VERSION}")
message("CMAKE_GENERATOR: ${CMAKE_GENERATOR}")
message("CMAKE_C_COMPILER_ID: ${CMAKE_C_COMPILER_ID}")
message("CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")

find_package(unofficial-breakpad CONFIG REQUIRED)
if(unofficial-breakpad_FOUND)
  message(STATUS "found unofficial-breakpad")
endif()
find_package(glog CONFIG REQUIRED)
if(glog_FOUND)
  message(STATUS "found glog")
endif()
find_package(benchmark CONFIG REQUIRED)
if(benchmark_FOUND)
  message(STATUS "found benchmark")
endif()
find_package(GTest CONFIG REQUIRED)
if(GTest_FOUND)
  message(STATUS "found GTest")
endif()

enable_testing()

add_subdirectory(Algorithm)
add_subdirectory(AtomicQueue)
add_subdirectory(BinaryTree)
add_subdirectory(Breakpad)
add_subdirectory(DesignPattern)
add_subdirectory(Glog)
add_subdirectory(LinkedList)
add_subdirectory(Mutex)
add_subdirectory(Thread)

if(CMAKE_HOST_WIN32)
  add_subdirectory(MonitoringDirectory)
elseif(CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
  add_subdirectory(Client)
  add_subdirectory(Icmp)
  add_subdirectory(Server)
endif()

cmake_minimum_required(VERSION 3.25.1)

include(cmake/macOSDeploymentTarget.cmake)
include(cmake/VcpkgToolchain.cmake)

project(
  Cpp-Examples
  VERSION 0.0.1
  DESCRIPTION "This is Cpp-Examples"
  HOMEPAGE_URL "https://github.com/RealChuan/Cpp-Examples"
  LANGUAGES C CXX)

include(cmake/ProjectSettings.cmake)
include(cmake/InstallCrashpad.cmake)

find_package(GTest CONFIG REQUIRED)
if(GTest_FOUND)
  message(STATUS "found GTest")
endif()
find_package(Threads REQUIRED)
if(Threads_FOUND)
  message(STATUS "found Threads")
endif()

include(CTest)
enable_testing()

include_directories(src)

add_subdirectory(src)

include(cmake/CMakeDebugInfo.cmake)

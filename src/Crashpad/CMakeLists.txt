find_package(crashpad CONFIG REQUIRED)
if(crashpad_FOUND)
  message(STATUS "found crashpad")
endif()

add_executable(crashpad_test crashpad.cc crashpad.hpp main.cc)
target_link_libraries(crashpad_test PRIVATE crashpad::crashpad Threads::Threads)

setup_crashpad_handler("${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

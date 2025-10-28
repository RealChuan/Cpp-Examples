find_package(crashpad CONFIG REQUIRED)
if(crashpad_FOUND)
  message(STATUS "found crashpad")
endif()

add_executable(crashpad_exapmle crashpad.cc crashpad.hpp exapmle.cc
                                ${CMAKE_SOURCE_DIR}/src/utils/utils.cc)
target_link_libraries(crashpad_exapmle PRIVATE crashpad::crashpad
                                               Threads::Threads)

setup_crashpad_handler("${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

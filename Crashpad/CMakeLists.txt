add_executable(crashpad_test crashpad.cc crashpad.hpp main.cc)
target_link_libraries(crashpad_test PRIVATE crashpad::crashpad Threads::Threads)

string(REPLACE "share/crashpad" "tools/crashpad" crash_handler_path
               ${crashpad_DIR})
message(STATUS "crashpad tools directory: ${crash_handler_path}")

add_custom_command(
  TARGET crashpad_test
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory ${crash_handler_path}/
          "$<TARGET_FILE_DIR:crashpad_test>")

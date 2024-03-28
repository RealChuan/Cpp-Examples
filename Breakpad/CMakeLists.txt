add_executable(breakpad_test main.cc breakpad.cc breakpad.hpp)
target_link_libraries(
  breakpad_test PRIVATE unofficial::breakpad::libbreakpad
                        unofficial::breakpad::libbreakpad_client)

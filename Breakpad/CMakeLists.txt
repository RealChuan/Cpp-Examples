add_executable(Breakpad main.cc breakpad.cc breakpad.hpp)
target_link_libraries(Breakpad PRIVATE unofficial::breakpad::libbreakpad
                                       unofficial::breakpad::libbreakpad_client)

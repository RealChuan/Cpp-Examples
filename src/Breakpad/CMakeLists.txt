find_package(unofficial-breakpad CONFIG REQUIRED)
if(unofficial-breakpad_FOUND)
  message(STATUS "found unofficial-breakpad")
endif()

add_executable(breakpad_unittest breakpad_unittest.cc breakpad.cc breakpad.hpp
                                 ${CMAKE_SOURCE_DIR}/src/utils/utils.cc)
target_link_libraries(
  breakpad_unittest
  PRIVATE unofficial::breakpad::libbreakpad
          unofficial::breakpad::libbreakpad_client GTest::gtest
          GTest::gtest_main GTest::gmock GTest::gmock_main)
add_test(NAME breakpad_unittest COMMAND breakpad_unittest)

add_executable(breakpad_example example.cc breakpad.cc breakpad.hpp
                                ${CMAKE_SOURCE_DIR}/src/utils/utils.cc)
target_link_libraries(
  breakpad_example PRIVATE unofficial::breakpad::libbreakpad
                           unofficial::breakpad::libbreakpad_client)

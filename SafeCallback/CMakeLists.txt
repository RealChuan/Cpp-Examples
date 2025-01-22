add_executable(safecallback_unittest safecallback_unittest.cc safecallback.hpp)
target_link_libraries(
  safecallback_unittest PRIVATE GTest::gtest GTest::gtest_main GTest::gmock
                                GTest::gmock_main)
add_test(NAME safecallback_unittest COMMAND safecallback_unittest)

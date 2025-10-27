add_executable(mymemcpy_unittest mymemcpy_unittest.cc mymemcpy.cc mymemcpy.hpp)
target_link_libraries(mymemcpy_unittest PRIVATE GTest::gtest GTest::gtest_main
                                                GTest::gmock GTest::gmock_main)
add_test(NAME mymemcpy_unittest COMMAND mymemcpy_unittest)

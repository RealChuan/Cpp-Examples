add_executable(memcpy_unittest memcpy_unittest.cc memcpy.hpp)
target_link_libraries(memcpy_unittest PRIVATE GTest::gtest GTest::gtest_main
                                              GTest::gmock GTest::gmock_main)
add_test(NAME memcpy_unittest COMMAND memcpy_unittest)

add_executable(singleton_unittest singleton_unittest.cc singleton.cc
                                  singleton.hpp)
target_link_libraries(singleton_unittest PRIVATE GTest::gtest GTest::gtest_main
                                                 GTest::gmock GTest::gmock_main)
add_test(NAME singleton_unittest COMMAND singleton_unittest)

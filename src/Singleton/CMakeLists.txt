add_executable(singleton_unitest singleton_unitest.cc singleton.hpp)
target_link_libraries(singleton_unitest PRIVATE GTest::gtest GTest::gtest_main
                                                GTest::gmock GTest::gmock_main)
add_test(NAME singleton_unitest COMMAND singleton_unitest)

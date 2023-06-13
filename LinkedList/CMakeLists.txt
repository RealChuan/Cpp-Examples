add_executable(linkedlist_unittest linkedlist_unittest.cc linkedlist.hpp)
target_link_libraries(
  linkedlist_unittest PRIVATE GTest::gtest GTest::gtest_main GTest::gmock
                              GTest::gmock_main)
add_test(NAME linkedlist_unittest COMMAND linkedlist_unittest)

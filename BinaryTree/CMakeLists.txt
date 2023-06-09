add_executable(binarytree_unittest binarytree_unittest.cc binarytree.hpp)
target_link_libraries(
  binarytree_unittest PRIVATE GTest::gtest GTest::gtest_main GTest::gmock
                              GTest::gmock_main)
add_test(NAME binarytree_unittest COMMAND binarytree_unittest)

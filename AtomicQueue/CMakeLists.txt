add_executable(atomicqueue_unittest atomicqueue.hpp atomicqueue_unittest.cc)
target_link_libraries(
  atomicqueue_unittest PRIVATE GTest::gtest GTest::gtest_main GTest::gmock
                               GTest::gmock_main)
add_test(NAME atomicqueue_unittest COMMAND atomicqueue_unittest)

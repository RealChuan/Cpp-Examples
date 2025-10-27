add_executable(queue_unittest queue_unittest.cc queue.hpp)
target_link_libraries(queue_unittest PRIVATE GTest::gtest GTest::gtest_main
                                             GTest::gmock GTest::gmock_main)
add_test(NAME queue_unittest COMMAND queue_unittest)

add_executable(thread_unittest thread_unittest.cc thread.hpp)
target_link_libraries(thread_unittest PRIVATE GTest::gtest GTest::gtest_main
                                              GTest::gmock GTest::gmock_main)
add_test(NAME thread_unittest COMMAND thread_unittest)

add_executable(threadpool_unittest queue.hpp thread.hpp threadpool_unittest.cc
                                   threadpool.hpp)
target_link_libraries(
  threadpool_unittest PRIVATE GTest::gtest GTest::gtest_main GTest::gmock
                              GTest::gmock_main)
add_test(NAME threadpool_unittest COMMAND threadpool_unittest)

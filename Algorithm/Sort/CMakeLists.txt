add_executable(sort_unittest sort_unittest.cc sort.hpp)
target_link_libraries(sort_unittest PRIVATE GTest::gtest GTest::gtest_main
                                            GTest::gmock GTest::gmock_main)
add_test(NAME sort_unittest COMMAND sort_unittest)

add_executable(sort_benchmark sort_benchmark.cc sort.hpp)
target_link_libraries(sort_benchmark PRIVATE benchmark::benchmark
                                             benchmark::benchmark_main)

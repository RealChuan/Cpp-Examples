add_executable(saerch_unittest search_unittest.cc search.hpp)
target_link_libraries(saerch_unittest PRIVATE GTest::gtest GTest::gtest_main
                                              GTest::gmock GTest::gmock_main)
add_test(NAME saerch_unittest COMMAND saerch_unittest)

add_executable(search_benchmark search_benchmark.cc search.hpp)
target_link_libraries(search_benchmark PRIVATE benchmark::benchmark
                                               benchmark::benchmark_main)

add_executable(std_search_examples std_search_examples.cc)

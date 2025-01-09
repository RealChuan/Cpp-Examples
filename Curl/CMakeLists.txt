add_executable(tcpclient_test tcpclient_test.cc tcpclient.cc tcpclient.hpp)
target_link_libraries(
  tcpclient_test PRIVATE CURL::libcurl GTest::gtest GTest::gtest_main
                         GTest::gmock GTest::gmock_main)

set(HttpClientSource file_utils.cc file_utils.hpp httpclient.cc httpclient.hpp)

add_executable(httpclient_test ${HttpClientSource} httpclient_test.cc)
target_link_libraries(
  httpclient_test PRIVATE CURL::libcurl GTest::gtest GTest::gtest_main
                          GTest::gmock GTest::gmock_main)

add_executable(httpclient_file_test ${HttpClientSource} httpclient_file_test.cc)
target_link_libraries(
  httpclient_file_test PRIVATE CURL::libcurl GTest::gtest GTest::gtest_main
                               GTest::gmock GTest::gmock_main)

set(HttpClientAsyncSource file_utils.cc file_utils.hpp httpclient_async.cc
                          httpclient_async.hpp)

add_executable(httpclient_async_test ${HttpClientAsyncSource}
                                     httpclient_async_test.cc)
target_link_libraries(
  httpclient_async_test PRIVATE CURL::libcurl GTest::gtest GTest::gtest_main
                                GTest::gmock GTest::gmock_main)

add_executable(httpclient_async_file_test ${HttpClientAsyncSource}
                                          httpclient_async_file_test.cc)
target_link_libraries(
  httpclient_async_file_test
  PRIVATE CURL::libcurl GTest::gtest GTest::gtest_main GTest::gmock
          GTest::gmock_main)

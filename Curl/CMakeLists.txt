add_executable(tcpclient_test tcpclient_test.cc tcpclient.cc tcpclient.hpp)
target_link_libraries(tcpclient_test PRIVATE CURL::libcurl)

add_executable(httpclient_test httpclient_test.cc httpclient.cc httpclient.hpp)
target_link_libraries(httpclient_test PRIVATE CURL::libcurl)

add_executable(httpclient_async_test httpclient_async_test.cc
                                     httpclient_async.cc httpclient_async.hpp)
target_link_libraries(httpclient_async_test PRIVATE CURL::libcurl)

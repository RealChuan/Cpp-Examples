find_package(OpenSSL REQUIRED)
if(OpenSSL_FOUND)
  message(STATUS "found OpenSSL")
endif()

add_executable(openssl_aes openssl_aes.cc openssl_common.cc openssl_common.hpp)
target_link_libraries(
  openssl_aes PRIVATE OpenSSL::SSL OpenSSL::Crypto GTest::gtest
                      GTest::gtest_main GTest::gmock GTest::gmock_main)
add_test(NAME openssl_aes COMMAND openssl_aes)

add_executable(openssl_base64 openssl_base64.cc openssl_common.cc
                              openssl_common.hpp)
target_link_libraries(
  openssl_base64 PRIVATE OpenSSL::SSL OpenSSL::Crypto GTest::gtest
                         GTest::gtest_main GTest::gmock GTest::gmock_main)
add_test(NAME openssl_base64 COMMAND openssl_base64)

add_executable(openssl_hash openssl_base64.cc openssl_common.cc
                            openssl_common.hpp)
target_link_libraries(
  openssl_hash PRIVATE OpenSSL::SSL OpenSSL::Crypto GTest::gtest
                       GTest::gtest_main GTest::gmock GTest::gmock_main)
add_test(NAME openssl_hash COMMAND openssl_hash)

add_executable(openssl_rsa openssl_rsa.cc openssl_common.cc openssl_common.hpp)
target_link_libraries(
  openssl_rsa PRIVATE OpenSSL::SSL OpenSSL::Crypto GTest::gtest
                      GTest::gtest_main GTest::gmock GTest::gmock_main)
add_test(NAME openssl_rsa COMMAND openssl_rsa)

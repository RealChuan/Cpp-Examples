add_executable(openssl_aes openssl_aes.cc openssl_utils.hpp)
target_link_libraries(
  openssl_aes PRIVATE OpenSSL::SSL OpenSSL::Crypto GTest::gtest
                      GTest::gtest_main GTest::gmock GTest::gmock_main)
add_test(NAME openssl_aes COMMAND openssl_aes)

add_executable(openssl_base64 openssl_base64.cc openssl_utils.hpp)
target_link_libraries(
  openssl_base64 PRIVATE OpenSSL::SSL OpenSSL::Crypto GTest::gtest
                         GTest::gtest_main GTest::gmock GTest::gmock_main)
add_test(NAME openssl_base64 COMMAND openssl_base64)

add_executable(openssl_hash openssl_hash.cc openssl_utils.hpp)
target_link_libraries(openssl_hash PRIVATE OpenSSL::SSL OpenSSL::Crypto)

add_executable(openssl_hmac openssl_hmac.cc openssl_utils.hpp)
target_link_libraries(openssl_hmac PRIVATE OpenSSL::SSL OpenSSL::Crypto)

add_executable(openssl_pem openssl_pem.cc openssl_utils.hpp)
target_link_libraries(
  openssl_pem PRIVATE OpenSSL::SSL OpenSSL::Crypto GTest::gtest
                      GTest::gtest_main GTest::gmock GTest::gmock_main)
add_test(NAME openssl_pem COMMAND openssl_pem)

add_executable(openssl_rsa openssl_rsa.cc openssl_utils.hpp)
target_link_libraries(
  openssl_rsa PRIVATE OpenSSL::SSL OpenSSL::Crypto GTest::gtest
                      GTest::gtest_main GTest::gmock GTest::gmock_main)
add_test(NAME openssl_rsa COMMAND openssl_rsa)

add_executable(openssl_sm4 openssl_sm4.cc openssl_utils.hpp)
target_link_libraries(
  openssl_sm4 PRIVATE OpenSSL::SSL OpenSSL::Crypto GTest::gtest
                      GTest::gtest_main GTest::gmock GTest::gmock_main)
add_test(NAME openssl_sm4 COMMAND openssl_sm4)

add_executable(openssl_x509 openssl_x509.cc openssl_utils.hpp)
target_link_libraries(openssl_x509 PRIVATE OpenSSL::SSL OpenSSL::Crypto)

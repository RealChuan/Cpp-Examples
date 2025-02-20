add_executable(spinmutex_unittest spinmutex_unittest.cc spinmutex.hpp)
target_link_libraries(spinmutex_unittest PRIVATE GTest::gtest GTest::gtest_main
                                                 GTest::gmock GTest::gmock_main)
add_test(NAME spinmutex_unittest COMMAND spinmutex_unittest)

if(CMAKE_HOST_UNIX)
  target_link_libraries(spinmutex_unittest PRIVATE pthread)
endif()

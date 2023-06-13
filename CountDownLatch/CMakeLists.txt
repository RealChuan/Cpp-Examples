add_executable(CountDownLatch_test main.cc countdownlatch.hpp)

if(CMAKE_HOST_UNIX)
  target_link_libraries(CountDownLatch_test pthread)
endif()

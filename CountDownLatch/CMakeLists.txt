add_executable(countdownlatch_test main.cc countdownlatch.hpp)

if(CMAKE_HOST_UNIX)
  target_link_libraries(countdownlatch_test pthread)
endif()

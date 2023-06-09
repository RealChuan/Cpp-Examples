add_executable(Mutex_Test main.cc mutex.hpp)

if(CMAKE_HOST_UNIX)
  target_link_libraries(Mutex_Test pthread)
endif()

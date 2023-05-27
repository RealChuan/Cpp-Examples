add_executable(Mutex main.cc mutex.hpp)

if(CMAKE_HOST_UNIX)
  target_link_libraries(Mutex pthread)
endif()

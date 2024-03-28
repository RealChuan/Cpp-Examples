if(CMAKE_HOST_WIN32)
  add_executable(MonitorDir main.cc monitordir_win.cc monitordir.cc
                            monitordir.hpp)
elseif(CMAKE_HOST_APPLE)
  add_executable(MonitorDir main.cc monitordir_mac.cc monitordir.cc
                            monitordir.hpp)
  target_link_libraries(MonitorDir "-framework CoreServices")
elseif(CMAKE_HOST_LINUX)
  add_executable(MonitorDir main.cc monitordir_linux.cc monitordir.cc
                            monitordir.hpp)
  target_link_libraries(MonitorDir PRIVATE pthread)
endif()

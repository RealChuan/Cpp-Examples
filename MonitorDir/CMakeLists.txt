if(CMAKE_HOST_WIN32)
  add_executable(MonitorDir main.cc monitordir_win.cc monitordir.cc
                            monitordir.hpp)
elseif(CMAKE_HOST_APPLE)

elseif(CMAKE_HOST_UNIX)
  add_executable(MonitorDir main.cc monitordir_linux.cc monitordir.cc
                            monitordir.hpp)
endif()

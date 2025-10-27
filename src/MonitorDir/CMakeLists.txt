if(CMAKE_HOST_WIN32)
  add_executable(monitor_dir main.cc monitordir_win.cc monitordir.cc
                             monitordir.hpp)
elseif(CMAKE_HOST_APPLE)
  add_executable(monitor_dir main.cc monitordir_mac.cc monitordir.cc
                             monitordir.hpp)
  target_link_libraries(monitor_dir "-framework CoreServices")
elseif(CMAKE_HOST_LINUX)
  add_executable(monitor_dir_inotify main.cc monitordir_linux_inotify.cc
                                     monitordir.cc monitordir.hpp)
  target_link_libraries(monitor_dir_inotify PRIVATE pthread)

  include(CheckSymbolExists)
  check_symbol_exists(fanotify_init "sys/fanotify.h" HAVE_FANOTIFY_INIT)
  if(HAVE_FANOTIFY_INIT)
    message(STATUS "Fanotify support is available.")
    add_executable(monitor_dir_fanotify main.cc monitordir_linux_fanotify.cc
                                        monitordir.cc monitordir.hpp)
    target_link_libraries(monitor_dir_fanotify PRIVATE pthread)
  endif()
endif()

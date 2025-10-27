find_package(efsw CONFIG REQUIRED)
if(efsw_FOUND)
  message(STATUS "found efsw")
endif()

add_executable(monitor_dir_efsw main.cc)
target_link_libraries(monitor_dir_efsw PRIVATE efsw::efsw)

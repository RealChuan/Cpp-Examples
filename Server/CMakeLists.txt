add_executable(server_select server_select.cc socket.hpp socket.cc)
add_executable(server_poll server_poll.cc socket.hpp socket.cc)
add_executable(server_epoll server_epoll.cc socket.hpp socket.cc)

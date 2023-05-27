#ifndef SOCKET_H
#define SOCKET_H

int socketListenBind();

int server_accept(int serverfd);

void show_info(int connfd);

void setNonBlocking(int fd);

void setBlock(int fd);

#endif // SOCKET_H

#include "socket.hpp"

#include <arpa/inet.h>
#include <fcntl.h>
#include <iostream>
#include <netinet/tcp.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

#define port 8888

int socketListenBind()
{
    int serverfd = 0;
    serverfd = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverfd < 0) {
        perror("socket Server error:");
        return -1;
    }

    int flag = 1, opt = 1;
    //int recv_size = 1024;
    //端口复用S
    setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    //接收缓冲区大小设置
    //setsockopt(serverfd, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<const char*> (&recv_size), sizeof (int));
    //无延时
    setsockopt(serverfd,
               IPPROTO_TCP,
               TCP_NODELAY,
               reinterpret_cast<const char *>(&flag),
               sizeof(flag));

    //bind
    int ret = 0;
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(struct sockaddr_in));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    ret = ::bind(serverfd,
                 reinterpret_cast<struct sockaddr *>(&serverAddr),
                 static_cast<socklen_t>(sizeof(struct sockaddr_in)));
    if (ret < 0) {
        close(serverfd);
        perror("bind error:");
        return -1;
    }

    //listen
    ret = ::listen(serverfd, SOMAXCONN);
    if (ret < 0) {
        close(serverfd);
        perror("listen error:");
        return -1;
    }
    cout << "打开服务器成功,正在监听..." << endl;
    cout << "服务器 :" << inet_ntoa(serverAddr.sin_addr) << ":" << ntohs(serverAddr.sin_port)
         << endl;
    return serverfd;
}

int server_accept(int serverfd)
{
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int connfd = ::accept(serverfd,
                          reinterpret_cast<struct sockaddr *>(&clientAddr),
                          &clientAddrLen);
    if (connfd < 0) {
        perror("accept error:");
        return -1;
    }
    cout << "客户端 :" << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port)
         << endl;
    return connfd;
}

void show_info(int connfd)
{
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);
    int ret = getpeername(connfd, reinterpret_cast<struct sockaddr *>(&clientAddr), &clientAddrLen);
    if (ret < 0) {
        perror("getpeername error:");
        return;
    }
    cout << "客户端 :" << inet_ntoa(clientAddr.sin_addr) << ":" << ntohs(clientAddr.sin_port)
         << endl;
}

/*----------------------------------------------------------/
/对文件描述符的操作必须先使用fcntl函数获得其标志，然后再设置标志/
/----------------------------------------------------------*/
//配置非阻塞模式
void setNonBlocking(int fileDescriptor)
{
    int flags = fcntl(fileDescriptor, F_GETFL, 0);
    flags |= O_NONBLOCK;
    fcntl(fileDescriptor, F_SETFL, flags);
}

//配置为阻塞模式
void setBlock(int fileDescriptor)
{
    int flags = fcntl(fileDescriptor, F_GETFL, 0);
    flags &= ~O_NONBLOCK;
    fcntl(fileDescriptor, F_SETFL, flags);
}
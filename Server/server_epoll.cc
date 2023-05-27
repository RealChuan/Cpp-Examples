#include <Server/socket.hpp>

#include <cstdio>
#include <cstring>
#include <iostream>
#include <list>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAXCN 10    //连接队列中的个数
#define quit "quit" //关闭命令

void server_epoll(int serverfd)
{
    std::list<int> clientfds;
    struct epoll_event event;
    //事件数组
    struct epoll_event eventList[MAXCN];
    int epollfd = epoll_create(MAXCN); //
    int timeout = 3000;
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = serverfd;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, serverfd, &event) < 0) {
        perror("Epoll Add serverfd Fail:");
        return;
    }
    setNonBlocking(serverfd); //配置非阻塞模式
    event.data.fd = STDIN_FILENO;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, STDIN_FILENO, &event) < 0) {
        perror("Epoll Add STDIN_FILENO Fail:");
        return;
    }
    setNonBlocking(STDIN_FILENO);
    //epoll
    while (1) {
        std::cout << "等待客户端连接..." << std::endl;
        //epoll_wait
        int ret = epoll_wait(epollfd, eventList, MAXCN, timeout);
        if (ret < 0) {
            perror("epoll error:");
            break;
        } else if (ret == 0) {
            printf("timeout ...\n");
            continue;
        }
        //直接获取了事件数量,给出了活动的流,这里是和poll区别的关键
        int i = 0;
        for (i = 0; i < ret; i++) {
            //错误退出
            if ((eventList[i].events & EPOLLERR) || (eventList[i].events & EPOLLHUP)
                || !(eventList[i].events & EPOLLIN)) {
                perror("epoll error:");
                close(eventList[i].data.fd);
                continue;
            }
            if (eventList[i].data.fd == STDIN_FILENO) {
                char buf[BUFSIZ];
                memset(buf, 0, sizeof buf);
                if (fgets(buf, BUFSIZ, stdin) == nullptr) {
                    perror("End:");
                    exit(EXIT_FAILURE);
                } else {
                    buf[strlen(buf) - 1] = '\0'; //减一的原因是不要回车字符
                    // 经验值：这一步非常重要的哦！！！！！！！！
                    if (strcmp(buf, quit) == 0) {
                        printf("Quit command!\n");
                        for (int j : clientfds) {
                            send(j, buf, strlen(buf), 0);
                            close(j);
                        }
                        close(serverfd);
                        printf("关闭服务端\n");
                        clientfds.clear();
                        break; //关闭服务端
                    }
                    for (int j : clientfds) {
                        ssize_t nsend = send(j, buf, strlen(buf), 0);
                        if (nsend < 0) {
                            perror("send");
                        }
                        printf("clinet[%d]\n", j);
                    }
                }
            } else if (eventList[i].data.fd == serverfd) {
                int clientfd = server_accept(serverfd);
                if (clientfd <= 0) {
                    perror("accept error:");
                    continue;
                }
                show_info(clientfd);
                if (clientfds.size() >= (MAXCN - 2)) {
                    fprintf(stderr, "connfd size over %d\n", MAXCN);
                    close(clientfd);
                } else {
                    //将新建立的连接添加到EPOLL的监听中
                    struct epoll_event event_;
                    event_.data.fd = clientfd;
                    event_.events = EPOLLIN | EPOLLET;
                    epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &event_);
                    setNonBlocking(clientfd);      //配置非阻塞模式
                    clientfds.push_back(clientfd); //
                }
            } else {
                char buf[BUFSIZ];
                memset(buf, 0, sizeof buf);
                int sockfd = eventList[i].data.fd;
                //接受客户端数据
                ssize_t nread = recv(sockfd, buf, sizeof buf, 0);
                show_info(sockfd);
                if (nread <= 0) {
                    int res = epoll_ctl(epollfd,
                                        EPOLL_CTL_DEL,
                                        sockfd,
                                        nullptr); //将该文件描述符从红黑树摘除
                    if (res == -1) {
                        perror("epoll_ctl error");
                    }
                    close(sockfd); //关闭与该客户端的链接
                    printf("客户端连接关闭\n");
                    clientfds.remove(sockfd);
                } else { //正常接收到服务器的数据
                    printf("client[%d] send:%s\n", i, buf);
                    snprintf(buf, 1024, "%s", "hello client!");
                    send(eventList[i].data.fd, buf, 1024, 0);
                }
            }
        }
    }
    if (serverfd) {
        close(serverfd);
    }
    if (epollfd) {
        close(epollfd);
    }
}

int main()
{
    std::cout << "Hello World!" << std::endl;
    int serverfd = socketListenBind();
    if (serverfd < 0) {
        perror("Failed to open server:");
        return -1;
    }
    printf("serverfd=%d\n", serverfd);
    server_epoll(serverfd);
    return 0;
}

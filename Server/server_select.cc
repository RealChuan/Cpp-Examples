#include <Server/socket.hpp>

#include <cstring>
#include <iostream>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAXCN 3     //连接队列中的个数
#define quit "quit" //关闭命令

void selectServer(int serverfd)
{
    fd_set fd_in;
    struct timeval tv;
    int sockfd[MAXCN]; //连接的fd
    int count = 0, i = 0, maxfd = serverfd;

    tv.tv_sec = 30; //最多等待30秒
    tv.tv_usec = 0;

    setNonBlocking(serverfd); //配置非阻塞模式
    setNonBlocking(STDIN_FILENO);

    while (1) {
        //初始化文件描述符集合
        FD_ZERO(&fd_in);              //重置集合
        FD_SET(serverfd, &fd_in);     //监视serverfd的输入事件
        FD_SET(STDIN_FILENO, &fd_in); //监视stdin的输入事件

        //添加活动的连接
        for (i = 0; i < count; i++) {
            FD_SET(sockfd[i], &fd_in);
        }
        if (count < MAXCN) {
            std::cout << "等待客户端连接..." << std::endl;
        }
        //如果文件描述符中有连接请求 会做相应的处理，实现I/O的复用 多用户的连接通讯
        int ret = select(maxfd + 1, &fd_in, nullptr, nullptr, &tv);
        if (ret < 0) { //没有找到有效的连接 失败
            perror("select error:");
            break;
        } else if (ret == 0) { // 指定的时间到，
            printf("timeout \n");
            continue;
        }
        if (FD_ISSET(serverfd, &fd_in)) {
            int clientfd = server_accept(serverfd);
            if (clientfd <= 0) {
                perror("accept error:");
                continue;
            }
            show_info(clientfd);
            //添加新的fd 到数组中 判断有效的连接数是否小于最大的连接数，如果小于的话，就把新的连接套接字加入集合
            if (count + 1 > MAXCN) {
                fprintf(stderr, "connfd size over %d\n", MAXCN);
                close(clientfd);
            } else {
                sockfd[count++] = clientfd;
                maxfd = std::max(clientfd, maxfd) + 1;
                setNonBlocking(clientfd); //配置非阻塞模式
            }
        }
        //循环判断有效的连接是否有数据到达
        for (i = 0; i < count; i++) {
            if (FD_ISSET(sockfd[i], &fd_in)) {
                char buf[BUFSIZ];
                memset(buf, 0, sizeof buf);
                ssize_t nread = recv(sockfd[i], buf, sizeof(buf), 0);
                show_info(sockfd[i]);
                if (nread <= 0) { //客户端连接关闭，清除文件描述符集中的相应的位
                    printf("client[%d] close\n", i);
                    close(sockfd[i]);
                    FD_CLR(sockfd[i], &fd_in);
                    memcpy(sockfd + i, sockfd + i + 1, static_cast<size_t>(count - i - 1)); //
                    count--;
                    i--; //数组发生变化，重新判断i的fd
                    continue;
                } else { //否则有相应的数据发送过来 ，进行相应的处理
                    printf("client[%d] send:%s\n", i, buf);
                    snprintf(buf, 1024, "%s", "hello client!");
                    send(sockfd[i], buf, 1024, 0);
                }
            }
        }
        if (FD_ISSET(STDIN_FILENO, &fd_in)) {
            char buf[BUFSIZ];
            memset(buf, 0, sizeof buf);
            if (fgets(buf, BUFSIZ, stdin) == nullptr) {
                printf("End...\n");
                exit(EXIT_FAILURE);
            } else {
                buf[strlen(buf) - 1] = '\0'; //减一的原因是不要回车字符
                // 经验值：这一步非常重要的哦！！！！！！！！
                if (strcmp(buf, quit) == 0) {
                    printf("Quit command!\n");
                    for (int j = 0; j < count; j++) {
                        close(sockfd[j]);
                    }
                    close(serverfd);
                    printf("关闭服务端\n");
                    exit(0); //关闭服务端
                }
                for (i = 0; i < count; i++) {
                    if (sockfd[i] == 0) {
                        continue;
                    }
                    ssize_t nsend = send(sockfd[i], buf, strlen(buf), 0);
                    if (nsend < 0) {
                        perror("send");
                    }
                }
            }
        }
    }
    for (i = 0; i < count; i++) {
        if (sockfd[i] != 0) {
            close(sockfd[i]);
        }
    }
    if (serverfd != 0) {
        close(serverfd);
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
    selectServer(serverfd);
    return 0;
}

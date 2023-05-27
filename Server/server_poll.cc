#include <Server/socket.hpp>

#include <cstring>
#include <iostream>
#include <poll.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAXCN 10    //连接队列中的个数
#define quit "quit" //关闭命令

/// Handle poll input types
#define READ_ONLY (POLLIN | POLLPRI)
/// Handle poll error types
#define POLL_ERROR (POLLERR | POLLHUP | POLLNVAL)
/// Handle pool output types
#define POLL_OUTPUT (POLLOUT)

void pollServer(int serverfd)
{
    int i = 0;
    struct pollfd pollfds[MAXCN];
    for (i = 1; i < MAXCN; i++) {
        pollfds[i].fd = -1; //初始化poll结构中的文件描述符fd
    }
    pollfds[0].fd = STDIN_FILENO; //标准输入描述符
    pollfds[1].fd = serverfd;     //tcp描述符

    pollfds[0].events = READ_ONLY; // 普通或优先级带数据可读
    pollfds[1].events = READ_ONLY; // 普通或优先级带数据可读POLLIN

    setNonBlocking(STDIN_FILENO); //配置非阻塞模式
    setNonBlocking(serverfd);

    int timeout = 30000; /* 设定3秒后超时 */

    int maxfd = 1;

    //对已连接的客户端的数据处理
    while (1) {
        if (maxfd < MAXCN) {
            std::cout << "等待客户端连接..." << std::endl;
        }
        //对加入poll结构体数组所有元素进行监测
        int ret = poll(pollfds, static_cast<unsigned long>(maxfd + 1), timeout);
        if (ret < 0) //没有找到有效的连接 失败
        {
            perror("poll error:");
            break;
        } else if (ret == 0) {
            printf("timeout \n");
            continue;
        }
        if (pollfds[0].revents & READ_ONLY) {
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
                    for (int j = 2; j < maxfd; j++) {
                        close(pollfds[j].fd);
                    }
                    close(serverfd);
                    printf("关闭服务端\n");
                    break; //关闭服务端
                }
                for (i = 2; i <= maxfd; i++) {
                    if (pollfds[i].fd == -1)
                        continue;
                    ssize_t nsend = send(pollfds[i].fd, buf, strlen(buf), 0);
                    if (nsend < 0) {
                        perror("send");
                    }
                    printf("clinet[%d]\n", i);
                }
            }
        }
        //监测serverfd(监听套接字)是否存在连接
        if (pollfds[1].revents & POLLIN) {
            //从tcp完成连接中提取客户端
            int clientfd = server_accept(serverfd);
            if (clientfd <= 0) {
                perror("accept error:");
                continue;
            }
            show_info(clientfd);
            //添加新的fd 到数组中 判断有效的连接数是否小于最大的连接数，如果小于的话，就把新的连接套接字加入集合
            if (maxfd >= MAXCN) {
                fprintf(stderr, "connfd size over %d\n", MAXCN);
                close(clientfd);
            } else {
                for (i = 2; i < MAXCN; i++) {
                    if (pollfds[i].fd < 0) {
                        pollfds[i].fd = clientfd;
                        pollfds[i].events = READ_ONLY;
                        setNonBlocking(clientfd); //配置非阻塞模式
                        printf("client[%d]=%d\n", i, clientfd);
                        break;
                    }
                }
            }
            if (i > maxfd) {
                maxfd = i;
            }
        }
        //继续响应就绪的描述符
        for (i = 2; i <= maxfd; i++) {
            if (pollfds[i].fd < 0) {
                continue;
            }
            if (pollfds[i].revents & (READ_ONLY | POLL_ERROR)) {
                char buf[BUFSIZ];
                memset(buf, 0, sizeof buf);
                //接受客户端数据
                ssize_t nread = recv(pollfds[i].fd, buf, sizeof buf, 0);
                show_info(pollfds[i].fd);
                if (nread <= 0) {
                    close(pollfds[i].fd);
                    pollfds[i].fd = -1;
                    printf("客户端连接关闭client[%d]\n", i);
                } else { //正常接收到服务器的数据
                    printf("client[%d] send:%s\n", i, buf);
                    snprintf(buf, 1024, "%s", "hello client!");
                    send(pollfds[i].fd, buf, 1024, 0);
                }
            }
        }
    }
    for (i = 2; i <= maxfd; i++) {
        if (pollfds[i].fd != 0) {
            close(pollfds[i].fd);
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
    printf("serverfd=%d\n", serverfd);
    pollServer(serverfd);
    return 0;
}

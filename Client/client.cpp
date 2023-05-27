#include <arpa/inet.h> //inet_ntoa
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h> //sockaddr_in
#include <pthread.h>
#include <stdio.h>  //perror
#include <stdlib.h> //exit()
#include <string.h>
#include <sys/socket.h> //socket()
#include <unistd.h>

using namespace std;

#define ip "0.0.0.0"
#define port 8888
#define maxsize 1024
#define quit "quit" //关闭命令

static int sockfd;

//配置非阻塞模式
void setNonBlocking(int fd)
{
    int flag = fcntl(fd, F_GETFL, 0);
    if (flag < 0) {
        perror("fcntl F_GETFL fail");
        return;
    }
    flag |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flag) < 0) {
        perror("fcntl F_SETFL fail");
    }
}

int connectToServer()
{
    struct sockaddr_in clIentaAddr;

    while (1) {
        if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
            perror("sockfd failed!");
            continue;
        }

        //设置服务器的地址
        clIentaAddr.sin_family = AF_INET;
        clIentaAddr.sin_port = htons(port);
        //inet_pton(AF_INET, ip, &clientsock.sin_addr.s_addr);
        clIentaAddr.sin_addr.s_addr = inet_addr(ip);

        cout << inet_ntoa(clIentaAddr.sin_addr) << ":" << clIentaAddr.sin_port << endl;

        if ((connect(sockfd, reinterpret_cast<struct sockaddr *>(&clIentaAddr), sizeof(clIentaAddr)))
            == -1) {
            perror("connect failed!");
            close(sockfd);
            sleep(1);
            continue;
        } else
            break;
    }
    cout << "服务器连接成功:" << sockfd << endl;
    setNonBlocking(sockfd);
    return sockfd;
}

void *readdata(void *)
{
    char readbuf[maxsize];
    fd_set fds;
    struct timeval tv;

    tv.tv_sec = 30; //5s超时设置
    tv.tv_usec = 0;

    while (1) {
        FD_ZERO(&fds);        //每次循环都要清空集合，否则不能检测描述符变化
        FD_SET(sockfd, &fds); //添加描述符
        ssize_t ret = select(sockfd + 1, &fds, nullptr, nullptr, &tv);
        //cout<<"z="<<z<<endl;
        if (ret == -1) {
            //perror("select error:");
            break;
        } else if (FD_ISSET(sockfd, &fds)) {
            memset(readbuf, 0, sizeof(readbuf));
            ssize_t nread = recv(sockfd, readbuf, sizeof(readbuf), 0);
            //cout<<"read"<<nread<<endl;
            if (nread == 0) {
                if (sockfd) {
                    cout << "服务端关闭\n关闭客户端" << endl;
                    close(sockfd);
                }
                exit(1);
            }
            if (nread < 0) {
                if (sockfd) {
                    close(sockfd);
                }
                exit(-1);
                //sockfd=connecttoserver();   //重连
            } else if (nread > 0) {
                cout << "服务端：" << readbuf << endl;
            }
        }
    }
    //cout<<"close accept:"<<sockfd<<endl;
    //cout<<"close read"<<endl;
    //close(sockfd);
    return nullptr;
}

void *senddata(void *)
{
    fd_set fds;
    char buf[maxsize];
    ssize_t nsend = 0;
    nsend = send(sockfd, "hello server", strlen("hello server"), 0);
    if (nsend <= 0) {
        exit(1);
    }
    while (1) {
        FD_ZERO(&fds); //每次循环都要清空集合，否则不能检测描述符变化
        FD_SET(STDIN_FILENO, &fds);
        if (FD_ISSET(STDIN_FILENO, &fds)) {
            memset(buf, 0, sizeof(buf));
            if (fgets(buf, maxsize, stdin) == nullptr) {
                printf("End...\n");
                exit(EXIT_FAILURE);
            } else {
                buf[strlen(buf) - 1] = '\0'; //减一的原因是不要回车字符
                //经验值：这一步非常重要的哦！！！！！！！！
                if (strcmp(buf, quit) == 0) {
                    cout << "Quit command!" << endl;
                    break; //关闭客户端
                           //exit(1);//关闭服务端
                }
                send(sockfd, buf, strlen(buf), 0);
            }
        }
    }
    cout << "客户端关闭" << endl;
    if (sockfd) {
        close(sockfd);
    }
    return nullptr;
}

int main()
{
    connectToServer();

    pthread_t t1, t2;
    if (pthread_create(&t1, nullptr, readdata, nullptr)) {
        perror("readthread error:");
        return -1;
    }
    if (pthread_create(&t2, nullptr, senddata, nullptr)) {
        perror("sendthread error:");
        return -1;
    }
    if (pthread_join(t1, nullptr) == -1) {
        perror("readjoin error");
        return -1;
    }
    if (pthread_join(t2, nullptr) == -1) {
        perror("sendjoin error");
        return -1;
    }

    close(sockfd);
    return 0;
}

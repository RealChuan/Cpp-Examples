#include <Icmp/icmp.hpp>

#include <algorithm>
#include <arpa/inet.h>
#include <memory.h>
#include <netdb.h>
#include <signal.h>
#include <string>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <vector>

std::string hostname;
std::vector<double> timerVec;
int nsend = 0;
int nreceived = 0;
struct timeval tvStart;

void quit(int signo)
{
    struct timeval tvEnd;
    gettimeofday(&tvEnd, NULL);
    long int t = 1000 * 1000 * (tvEnd.tv_sec - tvStart.tv_sec) + (tvEnd.tv_usec - tvStart.tv_usec);

    std::sort(timerVec.begin(), timerVec.end());
    double min = timerVec[0];
    double max = timerVec[timerVec.size() - 1];
    double avg = 0;
    for (double ms : timerVec) {
        avg += ms;
    }
    avg = avg / timerVec.size();

    printf("\n--------------------%s ping statistics-------------------\n", hostname.c_str());
    printf("%d packets transmitted, %d received , %%%d lost time %.3f ms\n"
           "rtt min/avg/max = %.3f/%.3f/%.3f ms\n",
           nsend,
           nreceived,
           (nsend - nreceived) / nsend * 100,
           double(t) / 1000.0,
           min,
           avg,
           max);
    exit(1);
}

std::string getIp(const char *name)
{
    if (inet_addr(name) != INADDR_NONE) {
        return name;
    }
    std::string ip;
    addrinfo *res = nullptr;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_UNSPEC;
    int result = getaddrinfo(name, nullptr, &hints, &res);
    if (result == 0) {
        std::vector<std::string> ipVec;
        addrinfo *node = res;
        while (node) {
            char buf[128] = {0};
            switch (node->ai_family) {
            case AF_INET: {
                ip = inet_ntop(node->ai_family,
                               &(reinterpret_cast<struct sockaddr_in *>(node->ai_addr))->sin_addr,
                               buf,
                               sizeof(buf));
                if (std::find(ipVec.begin(), ipVec.end(), ip) == ipVec.end()) {
                    ipVec.push_back(ip);
                    printf("IP addr v4: %s \n", ip.c_str());
                }
                break;
            }
            case AF_INET6: {
                ip = inet_ntop(node->ai_family,
                               &(reinterpret_cast<struct sockaddr_in6 *>(node->ai_addr))->sin6_addr,
                               buf,
                               sizeof(buf));
                if (std::find(ipVec.begin(), ipVec.end(), ip) == ipVec.end()) {
                    ipVec.push_back(ip);
                    printf("IP addr v6: %s \n", ip.c_str());
                }
                break;
            }
            default: fprintf(stderr, "Unknown address type \n"); break;
            }
            node = node->ai_next;
        }
        freeaddrinfo(res);
    } else {
        switch (result) {
#ifdef Q_OS_WIN
        case WSAHOST_NOT_FOUND: //authoritative not found
        case WSATRY_AGAIN:      //non authoritative not found
        case WSANO_DATA:        //valid name, no associated address
#else
        case EAI_NONAME:
        case EAI_FAIL:
#ifdef EAI_NODATA // EAI_NODATA is deprecated in RFC 3493
        case EAI_NODATA:
#endif
#endif
            fprintf(stderr, "Host not found  \n");
            break;
        default:
            fprintf(stderr, "UnknownError  \n");
            fprintf(stderr, "%s \n", gai_strerror(result));
            break;
        }
    }
    return ip;

    // std::string ip;
    // struct hostent *host = gethostbyname(name);
    // if (NULL == host) /*是主机名*/
    // {
    //     perror("gethostbyname error");
    //     return ip;
    // }
    // for (int i = 0; host->h_addr_list[i]; i++) {
    //     ip = inet_ntoa(*(struct in_addr *) host->h_addr_list[i]);
    //     printf("IP addr %d: %s\n", i + 1, ip.c_str());
    //     break;
    // }
    // return ip;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("usage:%s hostname/IP address\n", argv[0]);
        return -1;
    }
    hostname = argv[1];

    signal(SIGINT, quit);

    std::string ip(getIp(argv[1]));
    /*判断是主机名还是ip地址*/
    printf("PING %s(%s): %d bytes data in ICMP packets.\n", argv[1], ip.c_str(), 56);

    gettimeofday(&tvStart, NULL);

    Icmp icmp(ip);
    fd_set fds;
    struct timeval tv;
    tv.tv_sec = 10; //10s超时设置
    tv.tv_usec = 0;
    ResultPacket resultPacket;
    while (1) {
        nsend++;
        icmp.sendIcmpPacket();
        FD_ZERO(&fds); //每次循环都要清空集合，否则不能检测描述符变化
        FD_SET(icmp.sockfd(), &fds); //添加描述符
        ssize_t ret = select(icmp.sockfd() + 1, &fds, nullptr, nullptr, &tv);
        if (ret == -1) {
            perror("select error: ");
            break;
        } else if (FD_ISSET(icmp.sockfd(), &fds)) {
            if (!icmp.receiveIcmpPacket(resultPacket)) {
                break;
            }
            nreceived++;
            printf("%d byte from %s: icmp_seq=%u ttl=%d rtt=%.3f ms\n",
                   resultPacket.bytes,
                   resultPacket.ip.c_str(),
                   resultPacket.seq,
                   resultPacket.ttl,
                   resultPacket.time);
            timerVec.push_back(resultPacket.time);
        }
        sleep(1);
    }
}

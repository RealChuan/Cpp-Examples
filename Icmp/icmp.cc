#include "icmp.hpp"

#include <arpa/inet.h>
#include <iostream>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

uint16_t in_cksum(uint16_t *addr, int len)
{
    int nleft = len;
    uint32_t sum = 0;
    uint16_t *w = addr;
    uint16_t answer = 0;

    // Adding 16 bits sequentially in sum
    while (nleft > 1) {
        sum += *w;
        nleft -= 2;
        w++;
    }

    // If an odd byte is left
    if (nleft == 1) {
        *reinterpret_cast<unsigned char *>((&answer)) = *reinterpret_cast<unsigned char *>(w);
        sum += answer;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = ~sum;

    return answer;
}

int openSocket()
{
    int sockfd = ::socket(AF_INET, SOCK_RAW | SOCK_NONBLOCK | SOCK_CLOEXEC, IPPROTO_ICMP);
    if (sockfd < 0) {
        perror("Unable to open ICMP socket\n");
    }
    return sockfd;
}

bool setSocketOpt(int sockfd)
{
    if (sockfd < 0)
        return false;
    //int on = 1;
    //if (setsockopt(sockfd, IPPROTO_IP, IP_HDRINCL, (const char *)&on, sizeof(on)) == -1)
    //{
    //    perror("Unable to set IP_HDRINCL socket option\n");
    //    return false;
    //}

    //扩大套接字接收缓冲区到50K这样做主要为了减小接收缓冲区溢出的
    //的可能性,若无意中ping一个广播地址或多播地址,将会引来大量应答
    int size = 50 * 1024;
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size)) == -1) {
        perror("Unable to set SO_RCVBUF socket option\n");
        return false;
    }

    return true;
}

void tv_sub_(struct timeval *out, struct timeval *in)
{
    if ((out->tv_usec -= in->tv_usec) < 0) {
        --out->tv_sec;
        out->tv_usec += 1000000;
    }
    out->tv_sec -= in->tv_sec;
}

struct timeval tvrecv;

Icmp::Icmp(const std::string &destAddr)
    : m_sockfd(openSocket())
    , m_pid(::getpid())
{
    if (m_sockfd < 0 || !setSocketOpt(m_sockfd)) {
        m_error = true;
    }

    struct in_addr dest_addr;
    inet_pton(AF_INET, destAddr.c_str(), &dest_addr);
    m_destAddr.sin_family = AF_INET;
    m_destAddr.sin_addr.s_addr = dest_addr.s_addr;
}

Icmp::~Icmp()
{
    if (m_sockfd > 0) {
        ::close(m_sockfd);
    }
}

int pack(char *packet, int num, int pid)
{
    struct icmp *icmp = reinterpret_cast<struct icmp *>(packet);
    icmp->icmp_type = ICMP_ECHO;
    icmp->icmp_code = 0;
    icmp->icmp_cksum = 0;
    icmp->icmp_seq = num;
    icmp->icmp_id = pid;
    //packsize = sizeof(struct icmp) + sizeof(struct timeval);
    int packsize = 8 + 56;
    struct timeval *tval = (struct timeval *) icmp->icmp_data;
    gettimeofday(tval, NULL); /*记录发送时间*/
    icmp->icmp_cksum = in_cksum(reinterpret_cast<unsigned short *>(icmp), packsize); /*校验算法*/
    return packsize;
}

size_t Icmp::sendIcmpPacket()
{
    char sendpacket[1024] = {1};
    int packsize = pack(sendpacket, ++m_nsend, m_pid);

    return sendto(m_sockfd,
                  sendpacket,
                  packsize,
                  0,
                  reinterpret_cast<struct sockaddr *>(&m_destAddr),
                  sizeof(m_destAddr));
}

int unPack(char *buf, int len, struct sockaddr_in from, struct ResultPacket *outPacket)
{
    struct ip *ip = reinterpret_cast<struct ip *>(buf);
    int iphdrlen = ip->ip_hl << 2; /*求ip报头长度,即ip报头的长度标志乘4*/
    struct icmp *icmp = reinterpret_cast<struct icmp *>(
        (buf + iphdrlen)); /*越过ip报头,指向ICMP报头*/
    len -= iphdrlen;       /*ICMP报头及ICMP数据报的总长度*/
    if (len < 8) {         /*小于ICMP报头长度则不合理*/
        printf("ICMP packets/'s length is less than 8/n");
        return -1;
    }
    /*确保所接收的是我所发的的ICMP的回应*/
    if ((icmp->icmp_type == ICMP_ECHOREPLY) && (icmp->icmp_id == getpid())) {
        struct timeval *tvsend = (struct timeval *) icmp->icmp_data;
        tv_sub_(&tvrecv, tvsend);
        outPacket->bytes = len;
        outPacket->ip = inet_ntoa(from.sin_addr);
        outPacket->seq = icmp->icmp_seq;
        outPacket->ttl = ip->ip_ttl;
        outPacket->time = tvrecv.tv_sec * 1000 + tvrecv.tv_usec / 1000.0;
        // printf("%d byte from %s: icmp_seq=%u ttl=%d rtt=%.3f ms\n",
        //        len,
        //        inet_ntoa(from.sin_addr),
        //        icmp->icmp_seq,
        //        ip->ip_ttl,
        //        outPacket->time);
    } else {
        return -1;
    }
    return 0;
}

bool Icmp::receiveIcmpPacket(struct ResultPacket &resultPacket) const
{
    char recvpacket[1024] = {0};

    struct sockaddr_in src_addr;
    socklen_t src_addr_size = sizeof(struct sockaddr_in);
    int packet_size = recvfrom(m_sockfd,
                               recvpacket,
                               sizeof(recvpacket),
                               0,
                               reinterpret_cast<struct sockaddr *>(&src_addr),
                               &src_addr_size);
    if (packet_size == 0) {
        return false;
    }
    gettimeofday(&tvrecv, NULL);
    if (unPack(recvpacket, packet_size, src_addr, &resultPacket) != 0) {
        return false;
    }
    return true;
}

std::string Icmp::errorString() const
{
    return strerror(errno);
}
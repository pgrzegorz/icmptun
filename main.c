#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/if.h>
#include <netinet/ip_icmp.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <fcntl.h>
#include <arpa/inet.h>

#include "netif.h"
#include "icmp.h"

#define ID 12345
#define DEFAULT_DEVNAME "tun0"
#define MAX_PACKET_SIE 1500

void usage(char *progname)
{
    printf("usage: %s -h HOSTNAME [-d DEVNAME]\n", progname);
}

int main(int argc, char **argv)
{
    int efd, nread, i, en;
    int tunfd, sockfd, c;
    struct epoll_event evtun, evsock, rev[2];

    char devname[IFNAMSIZ];
    char hostname[255];

    unsigned char buff[MAX_PACKET_SIE];

    struct sockaddr_in addr;
    struct sockaddr_in addr_recv;

    memset(devname, 0, IFNAMSIZ);
    memset(hostname, 0, IFNAMSIZ);
    memset(&addr, 0, sizeof(struct sockaddr_in));

    while ((c = getopt(argc, argv, "i:d:h:")) != -1)
    {
        switch (c)
        {
        case 'h':
            strcpy(hostname, optarg);
            break;
        case 'd':
            strcpy(devname, optarg);
            break;
        case 'i':
        break;
        case '?':
        default:
            usage(argv[0]);
            return -1;
        }
    }

    if (strlen(hostname) == 0)
    {
        usage(argv[0]);
        return -1;
    }

    if (strlen(devname) == 0)
    {
        strcpy(devname, DEFAULT_DEVNAME);
    }

    /* tun */

    tunfd = if_tun_alloc(devname);
    if (tunfd < 0)
    {
        perror("if_tun_alloc");
        return -1;
    }

    if (if_up(devname) < 0)
    {
        perror("if_up");
        return -1;
    };
    printf("ifname: %s\n", devname);

    /* socket */
    sockfd = socket(AF_INET, SOCK_RAW|SOCK_NONBLOCK, IPPROTO_ICMP);
    
    if (setaddr(hostname, &addr) < 0)
    {
        perror("setaddr");
        return -1;
    }
    printf("side: %s (%s)\n", hostname, inet_ntoa(addr.sin_addr));

    /* epoll */

    efd = epoll_create(2);
    if (efd < 0)
    {
        perror("epoll_create");
        return -1;
    }

    evtun.data.fd = tunfd;
    evtun.events = EPOLLIN;

    evsock.data.fd = sockfd;
    evsock.events = EPOLLIN;

    if (epoll_ctl(efd, EPOLL_CTL_ADD, evsock.data.fd, &evsock) < 0)
    {
        perror("epoll_ctl socket");
        return -1;
    };
    if (epoll_ctl(efd, EPOLL_CTL_ADD, evtun.data.fd, &evtun) < 0)
    {
        perror("epoll_ctl tun");
        return -1;
    };

    while (1)
    {

        en = epoll_wait(efd, rev, 2, -1);

        for (i = 0; i < en; i++)
        {
            if (rev[i].data.fd == tunfd)
            {
                memset(buff, 0, MAX_PACKET_SIE);
                nread = read(tunfd, buff, MAX_PACKET_SIE);
                icmp_send(sockfd, buff, nread, &addr, ICMP_ECHO, ID);
            }

            if (rev[i].data.fd == sockfd)
            {  
                memset(buff, 0, MAX_PACKET_SIE);
                if ((nread = icmp_get(sockfd, buff, MAX_PACKET_SIE, &addr_recv, ICMP_ECHOREPLY | ICMP_ECHO, ID)) > 0)
                {   
                      write(tunfd, buff, nread);
                }
            }
        }
    }
    return 0;
}

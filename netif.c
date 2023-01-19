
#include "netif.h"
#include <stdio.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>

int if_tun_alloc(char *name)
{
    struct ifreq ifr;
    int fd, error;

    if ((fd = open("/dev/net/tun", O_RDWR)) < 0)
    {
        perror("tun open");
        return fd;
    }

    bzero(&ifr, sizeof(ifr));

    // setup tunnell flags
    ifr.ifr_flags = IFF_TUN;

    if (*name)
        strncpy(ifr.ifr_name, name, IFNAMSIZ);

    error = ioctl(fd, TUNSETIFF, (void *)&ifr);
    if (error < 0)
    {
        close(fd);
        perror("if_tun_alloc TUNSETIF");
        return error;
    };
    return fd;
}

int if_setflags(char *name, int flags)
{
    struct ifreq ifr;

    int s;

    s = socket(AF_INET, SOCK_DGRAM, 0);
    bzero(&ifr, sizeof(ifr));
    if (*name)
        strncpy(ifr.ifr_name, name, IFNAMSIZ);

    if (ioctl(s, SIOCGIFFLAGS, &ifr) < 0)
    {
        perror("if_setflags SIOCGIFFLAGS");
        close(s);
        return -1;
    }

    ifr.ifr_flags = ifr.ifr_flags | flags;

    if (ioctl(s, SIOCSIFFLAGS, &ifr) < 0)
    {
        perror("if_Setflags SIOCSIFFLAGS");
        close(s);
        return -1;
    }
    close(s);
    return 1;
}

int if_up(char *name)
{
    return if_setflags(name, IFF_UP);
}
#ifndef NETIF_H
#define NETIF_H

int if_tun_alloc(char *name);
int if_setflags(char * name, int flags);
int if_up(char * name);

#endif
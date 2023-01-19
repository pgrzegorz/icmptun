#ifndef ICMP_H
#define ICMP_H


int icmp_get(int sock, 
            unsigned char *buff, 
            int buff_len, 
            struct sockaddr_in *addr, 
            int type, 
            uint16_t id);

int icmp_send(int sock, 
	        unsigned char *buff,
            int buff_len,
            struct sockaddr_in *addr,
            int icmp_type,
            uint16_t id);
              
int setaddr(char * name, struct sockaddr_in *addr);

#endif

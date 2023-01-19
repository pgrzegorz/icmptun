#include <netinet/ip_icmp.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <netdb.h>

#include "icmp.h"

uint16_t checksum(uint16_t *addr, int len);

int icmp_get(int sock, unsigned char *buff, int buff_len, struct sockaddr_in *addr, int type, uint16_t id) {
    struct icmphdr *icmp;
    int ret;
    socklen_t len = sizeof(struct sockaddr_in);
    int icmplen = sizeof(struct icmphdr);
    int iplen = sizeof(struct iphdr);
    int hdrlen = icmplen + iplen;

    ret = recvfrom(sock, buff, buff_len, 0, (struct sockaddr *)addr, &len);
    if (ret)
    {
        icmp = (struct icmphdr *)(buff+iplen);
       if ((icmp->type | type) && (icmp->un.echo.id == id))
        {
            memmove(buff, buff + hdrlen, ret - hdrlen);
            return (ret - hdrlen);
        }
    }
    return (ret);
}

int icmp_send(int sock, 
	            unsigned char *buff,
              int buff_len,
              struct sockaddr_in * addr,
              int icmp_type,
              uint16_t id) {

    int ret, packet_size;
    int header_size = sizeof(struct icmphdr);

    static uint8_t *packet;
    packet_size = header_size + buff_len;
    packet = malloc(packet_size);

    struct icmphdr *icmp = (struct icmphdr *)packet;

    socklen_t len = sizeof(struct sockaddr_in);
    
    icmp->type = icmp_type;
    icmp->code = 0;
    icmp->un.echo.sequence = 0;
    icmp->un.echo.id = id;
    icmp->checksum =0;
    
    memcpy(&packet[header_size], buff, buff_len);
   
    
    icmp->checksum = checksum((uint16_t *)packet, packet_size);
    
    if ((ret = sendto(sock, packet, packet_size, 0, (struct sockaddr *)addr, len) < 0))
    {
        perror("icmp sendto");
        exit(1);
    }
    return (ret);
}

int setaddr(char *name, struct sockaddr_in *addr) {
    
  struct hostent *hp;
  hp = gethostbyname(name);
  if (hp == NULL) return -1;
  addr->sin_family = AF_INET;
  addr->sin_port = 0;
  addr->sin_addr.s_addr = *((unsigned long *)hp->h_addr);

  return 0;
}

uint16_t checksum(uint16_t *addr, int len) {
  int nleft = len;
  uint32_t sum = 0;
  uint16_t *w = addr;
  uint16_t answer = 0;

  while (nleft > 1) {
    sum += *w;
    nleft -= 2;
    w++;
  }

  if (nleft == 1) {
    *(unsigned char *) (&answer) = *(unsigned char *) w;
    sum += answer;
  }

  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  answer = ~sum;

  return answer;
}

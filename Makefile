CC=gcc
CFLAGS=-Wall

all: tun

tun: icmp.o netif.o main.o
		$(CC) $(CFLAGS) -o tun main.o icmp.o netif.o

main.o: main.c netif.h icmp.h
		$(CC) $(CFLAGS) -c main.c -o main.o

netif.o: netif.c
		$(CC) $(CFLAGS) -c netif.c -o netif.o

icmp.o: icmp.c
		$(CC) $(CFLAGS) -c icmp.c -o icmp.o

clean:
		rm -f *o
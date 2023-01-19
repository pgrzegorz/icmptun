# ICMP tunnel
Simple implementation of tunnel over ICMP echo request packets. It's uses epoll, it will compile and run only on Linux systems.

## Build and test
To build run: `make`.

Start tunnel on host A:
```
./tun -h hostB
```

Add IP address to tun device (default tun0), for example:
```
ip a a 10.0.0.1/24 dev tun0
```

Start tunnel on host B:
```
./tun -h hostA
```

Add IP address to tun device (default tun0), for example:
```
ip a a 10.10.10.2/24 dev tun0
```

Verify from host A:
```
ping 10.10.10.2
```

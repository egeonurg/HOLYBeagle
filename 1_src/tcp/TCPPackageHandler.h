#pragma once

#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>


#ifdef CROSS_COMPILE_BBBI
#define TCP_INTERFACE_NAME_ACTIVE "eth0"
#else
#define TCP_INTERFACE_NAME_ACTIVE "enp1s0"
#endif

class TCPPackageHandler 
{
private:
    int sockfd;
    struct sockaddr_ll sa;
    struct ifreq ifr;
    char buffer[4096];
    const char *interface;

    unsigned short checksum(unsigned short *buf, int len);
    void setupSocket();

public:
    TCPPackageHandler(const char *iface) : interface(iface) 
    {
        setupSocket();
    }

    ~TCPPackageHandler() {
        close(sockfd);
    }
    void get_mac_address(const char *iface, unsigned char *mac);
    void sendPacket(const char *srcIP, const char *destIP, const char *payload);
};


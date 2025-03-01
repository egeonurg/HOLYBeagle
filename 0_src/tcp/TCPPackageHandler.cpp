#include "TCPPackageHandler.h"

unsigned short TCPPackageHandler::checksum(unsigned short *buf, int len) 
{
    unsigned long sum = 0;
    while (len > 1) {
        sum += *buf++;
        len -= 2;
    }
    if (len == 1) {
        sum += *(unsigned char *)buf;
    }
    sum = (sum >> 16) + (sum & 0xFFFF);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}

void TCPPackageHandler::setupSocket() 
{
    if ((sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) 
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    strncpy(ifr.ifr_name, interface, IFNAMSIZ);
    if (ioctl(sockfd, SIOCGIFINDEX, &ifr) < 0) 
    {
        perror("ioctl");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    memset(&sa, 0, sizeof(sa));
    sa.sll_family = AF_PACKET;
    sa.sll_protocol = htons(ETH_P_ALL);
    sa.sll_ifindex = ifr.ifr_ifindex;
}

void TCPPackageHandler::get_mac_address(const char *iface, unsigned char *mac) 
{
    struct ifreq ifr;
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    strcpy(ifr.ifr_name, iface);
    if (ioctl(fd, SIOCGIFHWADDR, &ifr) == 0) {
        memcpy(mac, ifr.ifr_hwaddr.sa_data, ETH_ALEN);
    }
    close(fd);
}

void TCPPackageHandler::sendPacket(const char *srcIP, const char *destIP, const char *payload) 
{
    unsigned char dest_mac[ETH_ALEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    unsigned char src_mac[ETH_ALEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    unsigned short eth_type = htons(ETH_P_IP);
    get_mac_address("enp1s0", src_mac);
    memcpy(buffer, dest_mac, ETH_ALEN);
    memcpy(buffer + ETH_ALEN, src_mac, ETH_ALEN);
    memcpy(buffer + 2 * ETH_ALEN, &eth_type, sizeof(eth_type));

    struct iphdr *ip = (struct iphdr *)(buffer + ETH_HLEN);
    struct tcphdr *tcp = (struct tcphdr *)(buffer + ETH_HLEN + sizeof(struct iphdr));

    ip->ihl = 5;
    ip->version = 4;
    ip->ttl = 64;
    ip->protocol = IPPROTO_TCP;
    ip->saddr = inet_addr(srcIP);
    ip->daddr = inet_addr(destIP);

    tcp->source = htons(12345);
    tcp->dest = htons(80);
    tcp->seq = htonl(1);
    tcp->syn = 1;
    tcp->doff = 5;
    tcp->window = htons(5840);

    size_t payload_size = strlen(payload);
    ip->tot_len = htons(sizeof(struct iphdr) + sizeof(struct tcphdr) + payload_size);

    struct pseudo_header {
        unsigned int src_addr;
        unsigned int dst_addr;
        unsigned char reserved;
        unsigned char protocol;
        unsigned short tcp_len;
    } psh;

    psh.src_addr = ip->saddr;
    psh.dst_addr = ip->daddr;
    psh.protocol = IPPROTO_TCP;
    psh.tcp_len = htons(sizeof(struct tcphdr) + payload_size);

    int psize = sizeof(struct pseudo_header) + sizeof(struct tcphdr) + payload_size;
    char *pseudogram = (char *)malloc(psize);
    memcpy(pseudogram, &psh, sizeof(struct pseudo_header));
    memcpy(pseudogram + sizeof(struct pseudo_header), tcp, sizeof(struct tcphdr));
    memcpy(pseudogram + sizeof(struct pseudo_header) + sizeof(struct tcphdr), payload, payload_size);
    memcpy(buffer + ETH_HLEN + sizeof(struct iphdr) + sizeof(struct tcphdr), payload, payload_size);

    tcp->check = checksum((unsigned short *)pseudogram, psize);
    free(pseudogram);
    ip->check = checksum((unsigned short *)ip, sizeof(struct iphdr));

    if (sendto(sockfd, buffer, ETH_HLEN + sizeof(struct iphdr) + sizeof(struct tcphdr) + payload_size, 0, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
        perror("sendto");
        exit(EXIT_FAILURE);
    }
    
    std::cout << "Packet sent with payload: " << payload << " (" << payload_size << " bytes)" << std::endl;
}
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/if_packet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>

// Calculate the checksum for the TCP packet
unsigned short checksum(unsigned short *buf, int len) {
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

int main() {
    int sockfd;
    struct sockaddr_ll sa;
    struct ifreq ifr;
    char buffer[4096];

    // Hardcoded network interface
    const char *interface = "enp1s0";

    // Create a raw socket
    if ((sockfd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL))) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Get the index of the interface
    strncpy(ifr.ifr_name, interface, IFNAMSIZ);
    if (ioctl(sockfd, SIOCGIFINDEX, &ifr) < 0) {
        perror("ioctl");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Prepare the destination address
    memset(&sa, 0, sizeof(sa));
    sa.sll_family = AF_PACKET;
    sa.sll_protocol = htons(ETH_P_ALL);
    sa.sll_ifindex = ifr.ifr_ifindex;

    // Craft the Ethernet header
    unsigned char dest_mac[ETH_ALEN] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}; // Broadcast MAC
    unsigned char src_mac[ETH_ALEN] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // Replace with your MAC
    unsigned short eth_type = htons(ETH_P_IP);

    memcpy(buffer, dest_mac, ETH_ALEN);
    memcpy(buffer + ETH_ALEN, src_mac, ETH_ALEN);
    memcpy(buffer + 2 * ETH_ALEN, &eth_type, sizeof(eth_type));

    // Craft the IP header
    struct iphdr *ip = (struct iphdr *)(buffer + ETH_HLEN);
    ip->ihl = 5;
    ip->version = 4;
    ip->tos = 0;
    ip->id = htons(54321);
    ip->frag_off = 0;
    ip->ttl = 64;
    ip->protocol = IPPROTO_TCP;
    ip->check = 0; // Checksum will be calculated later
    ip->saddr = inet_addr("192.168.1.100"); // Source IP
    ip->daddr = inet_addr("192.168.1.1");   // Destination IP

    // Craft the TCP header
    struct tcphdr *tcp = (struct tcphdr *)(buffer + ETH_HLEN + sizeof(struct iphdr));
    tcp->source = htons(12345); // Source port
    tcp->dest = htons(80);      // Destination port (e.g., HTTP)
    tcp->seq = htonl(1);
    tcp->ack_seq = 0;
    tcp->doff = 5;
    tcp->fin = 0;
    tcp->syn = 1; // SYN flag
    tcp->rst = 0;
    tcp->psh = 0;
    tcp->ack = 0;
    tcp->urg = 0;
    tcp->window = htons(5840);
    tcp->check = 0; // Checksum will be calculated later
    tcp->urg_ptr = 0;

    // Define the payload
    const char *payload = "Darina, my darlin, i'm sending this package is for you! Raaaaaaaaaaaaaaaaaaaawwwwwrrrr";
    size_t payload_size = strlen(payload); // Calculate the payload size

    // Update IP and TCP lengths to include the payload
    ip->tot_len = htons(sizeof(struct iphdr) + sizeof(struct tcphdr) + payload_size);
    tcp->check = 0; // Reset checksum before recalculating

    // Calculate the TCP checksum (pseudo-header)
    struct pseudo_header {
        unsigned int src_addr;
        unsigned int dst_addr;
        unsigned char reserved;
        unsigned char protocol;
        unsigned short tcp_len;
    } psh;

    psh.src_addr = ip->saddr;
    psh.dst_addr = ip->daddr;
    psh.reserved = 0;
    psh.protocol = IPPROTO_TCP;
    psh.tcp_len = htons(sizeof(struct tcphdr) + payload_size);

    int psize = sizeof(struct pseudo_header) + sizeof(struct tcphdr) + payload_size;
    char *pseudogram = malloc(psize);
    memcpy(pseudogram, (char *)&psh, sizeof(struct pseudo_header));
    memcpy(pseudogram + sizeof(struct pseudo_header), tcp, sizeof(struct tcphdr));

    // Copy the payload into the pseudo-header and packet buffer
    memcpy(pseudogram + sizeof(struct pseudo_header) + sizeof(struct tcphdr), payload, payload_size);
    memcpy(buffer + ETH_HLEN + sizeof(struct iphdr) + sizeof(struct tcphdr), payload, payload_size);

    tcp->check = checksum((unsigned short *)pseudogram, psize);
    free(pseudogram);

    // Calculate the IP checksum
    ip->check = checksum((unsigned short *)ip, sizeof(struct iphdr));

    // Send the TCP packet
    if (sendto(sockfd, buffer, ETH_HLEN + sizeof(struct iphdr) + sizeof(struct tcphdr) + payload_size, 0, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
        perror("sendto");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("TCP packet with payload sent via interface %s!\n", interface);
    printf("Payload: %s\n", payload);
    printf("Payload size: %zu bytes\n", payload_size);

    close(sockfd);
    return 0;
}
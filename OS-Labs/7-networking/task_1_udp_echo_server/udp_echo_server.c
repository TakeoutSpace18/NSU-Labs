#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "utils.h"

/* 64 KB is the max datagram size in UDP protocol */
/* such size ensures that no data will be discarded */
#define BUFSIZE 64 * 1024

int main(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: udp_echo_server <host> <port>");
        return EXIT_FAILURE;
    }

    int sfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sfd == -1) {
        perror("socket()");
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_addr = parse_address(argv[1], argv[2]);

    if (bind(sfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind()");
        close(sfd);
        return EXIT_FAILURE;
    }

    char buffer[BUFSIZE];
    while (true) {
        struct sockaddr_in src_addr;
        socklen_t src_addr_len = sizeof(src_addr);
        ssize_t received_len = recvfrom(sfd, buffer, BUFSIZE, 0,  (struct sockaddr*)&src_addr, &src_addr_len);
        if (received_len == -1) {
            perror("recvfrom()");
            close(sfd);
            return EXIT_FAILURE;
        }

        char addr_text[20];
        inet_ntop(AF_INET, &src_addr.sin_addr, addr_text, sizeof(addr_text));
        printf("Received datagram from %s:%i\n", addr_text, ntohs(src_addr.sin_port));

        if (sendto_n(sfd, buffer, received_len, 0, (struct sockaddr*)&src_addr, src_addr_len) == -1) {
            perror("sendto_n()");
            close(sfd);
            return EXIT_FAILURE;
        }
    }


    return EXIT_SUCCESS;
}
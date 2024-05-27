#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <unistd.h>

#include "utils.h"

#define BUFSIZE 64

int main() {

    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1) {
        perror("socket()");
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(6666);
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);


    if (connect(sfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect()");
        close(sfd);
        return EXIT_FAILURE;
    }

    char buffer[BUFSIZE];
    while (fgets(buffer, BUFSIZE, stdin) != NULL) {

        if (send_n(sfd, buffer, strlen(buffer) + 1, 0) == -1) {
            perror("send_n()");
            close(sfd);
            return EXIT_FAILURE;
        }

        if (recv_n(sfd, buffer, strlen(buffer) + 1, 0) == -1)
        {
            perror("recv_n()");
            close(sfd);
            return EXIT_FAILURE;
        }

        fputs(buffer, stdout);
    }


    return EXIT_SUCCESS;
}
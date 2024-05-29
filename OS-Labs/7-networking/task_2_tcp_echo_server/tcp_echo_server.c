#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "utils.h"

#define BUFSIZE 64

int client_process(int sfd, struct sockaddr_in addr)
{
    char addr_text[20];
    inet_ntop(AF_INET, &addr.sin_addr, addr_text, sizeof(addr_text));
    printf("Connected client %s:%i, pid: %i\n", addr_text, ntohs(addr.sin_port), getpid());

    while (true) {
        char buffer[BUFSIZE];

        ssize_t received_len = recv(sfd, buffer, BUFSIZE, 0);
        if (received_len == -1) {
            perror("recv()");
            close(sfd);
            return EXIT_FAILURE;
        }

        if (received_len == 0) {
            break;
        }

        if (send_n(sfd, buffer, received_len, 0) == -1) {
            perror("send_n()");
            close(sfd);
            return EXIT_FAILURE;
        }
    }

    close(sfd);
    printf("Disconnected client %s:%i, pid: %i\n", addr_text, ntohs(addr.sin_port), getpid());

    return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
    int ret_code = EXIT_SUCCESS;

    if (argc < 3) {
        printf("usage: tcp_echo_server <host> <port>");
        return EXIT_FAILURE;
    }

    int sfd = socket(AF_INET, SOCK_STREAM, 0);
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

    if (listen(sfd, 128) == -1) {
        perror("listen()");
        close(sfd);
        return EXIT_FAILURE;
    }

    while (true) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_sfd = accept(sfd, (struct sockaddr*)&client_addr, &client_addr_len);

        if (client_sfd == -1) {
            perror("accept()");
            return EXIT_FAILURE;
        }

        pid_t pid = fork();

        switch (pid) {
        case -1:
            perror("fork()");
            close(client_sfd);
        case 0:
            return client_process(client_sfd, client_addr);
        }
    }

    return EXIT_SUCCESS;
}

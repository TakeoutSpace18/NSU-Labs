#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>

static ssize_t sendto_n(int sockfd, const void *buf, size_t len, int flags,
    const struct sockaddr *dest_addr, socklen_t addrlen)
{
    ssize_t sent_len = 0;
    while (sent_len < len) {
        ssize_t ret = sendto(sockfd, buf + sent_len, len - sent_len, 0, dest_addr, addrlen);
        if (ret == -1) {
            return -1;
        }

        sent_len += ret;
    }
    return sent_len;
}

static ssize_t send_n(int sockfd, const void *buf, size_t len, int flags)
{
    return sendto_n(sockfd, buf, len, flags, NULL, 0);
}

static ssize_t recvfrom_n(int sockfd, void *buf, size_t len, int flags,
    struct sockaddr *src_addr, socklen_t *addrlen)
{
    ssize_t recv_len = 0;
    while (recv_len < len) {
        ssize_t ret = recvfrom(sockfd, buf + recv_len, len - recv_len, 0, src_addr, addrlen);
        if (ret == -1) {
            return -1;
        }

        recv_len += ret;
    }
    return recv_len;
}

static ssize_t recv_n(int sockfd, void *buf, size_t len, int flags)
{
    return recvfrom_n(sockfd, buf, len, flags, NULL, NULL);
}

static struct sockaddr_in parse_address(char *host, char *port)
{
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(port));

    if (strcmp(host, "any")) {
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else {
        if (inet_aton(host, &addr.sin_addr) == 0) {
            printf("host address is invalid");
            exit(EXIT_FAILURE);
        }
    }

    return addr;
}

#endif
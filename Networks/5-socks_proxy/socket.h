#ifndef SOCKET_H
#define SOCKET_H

#include "c.h"
#include <fcntl.h>

#define SOCKADDR2STR_MAX_BUFSIZE (NI_MAXHOST + NI_MAXSERV + 2)

static inline void
sockaddr2str(struct sockaddr *sa, socklen_t sa_len, char *buf)
{
    if (getnameinfo(sa, sa_len, buf, NI_MAXHOST, NULL, 0, 0) == -1)
        goto fail;

    strcat(buf, ":");

    if (getnameinfo(sa, sa_len, NULL, 0, buf + strlen(buf), NI_MAXSERV, 0) == -1)
        goto fail;

    return;

fail:
    snprintf(buf, SOCKADDR2STR_MAX_BUFSIZE, "???");
}

/* receives whole buffer of given size or returns -1 */
static inline ssize_t recv_buf(int sockfd, void *buf, size_t size)
{
    for (size_t recv_size = 0; recv_size < size; ) {
        ssize_t ret = recv(sockfd, (char *) buf + recv_size, size - recv_size, 0);

        if (ret == -1)
            return -1;

        recv_size += ret;
    }

    return size;
}

/* sends whole buffer of given size or returns -1 */
static inline ssize_t send_buf(int sockfd, const void *buf, size_t size)
{
    for (size_t sent_size = 0; sent_size < size; ) {
        ssize_t ret = send(sockfd, (char *) buf + sent_size, size - sent_size, 0);

        if (ret == -1)
            return -1;

        sent_size += ret;
    }

    return size;
}

static inline int set_nonblock(int fd)
{
    int flags = fcntl(fd, F_GETFL);
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

#endif /* SOCKET_H */


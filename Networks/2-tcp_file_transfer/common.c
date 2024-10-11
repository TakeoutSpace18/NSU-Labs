#include "common.h"

#include <sys/socket.h>
#include <errno.h>

Result_t
receive_buffer(int sockfd, void *buf, size_t size)
{
    for (size_t recv_size = 0; recv_size < size; ) {
        ssize_t ret = recv(sockfd, (char *) buf + recv_size, size - recv_size, 0);

        if (ret == -1 && errno == EINTR)
            return INTERRUPTED;

        if (ret == -1 && errno == EAGAIN)
            return TIMED_OUT;

        if (ret == -1)
            return ERROR;

        recv_size += ret;
    }

    return OK;
}

Result_t
send_buffer(int sockfd, const void *buf, size_t size)
{
    for (size_t sent_size = 0; sent_size < size; ) {
        ssize_t ret = send(sockfd, (char *) buf + sent_size, size - sent_size, 0);

        if (ret == -1 && errno == EINTR)
            return INTERRUPTED;

        if (ret == -1 && errno == EAGAIN)
            return TIMED_OUT;

        if (ret == -1)
            return ERROR;

        sent_size += ret;
    }

    return OK;
}


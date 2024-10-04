#ifndef TRANSFER_PROTOCOL_H
#define TRANSFER_PROTOCOL_H

#include <stdlib.h>
#include <stdint.h>
#include <sys/socket.h>

#include "error.h"

#define BEGIN_FILE_TRANSFER_MAGIC 0x1fab1fab

/* To initiate file transfer, client sends this struct with magic above */
typedef struct BeginFileTransferDTO
{
    uint64_t magic;
    size_t filename_size;
    size_t file_size;
}
BeginFileTransferDTO_t;

/* after that, server receives filename consisting
 * of *filename_size* UTF-8 chars
 * and the file itself, diveded into chunks */

#define FILE_CHUNK_SIZE (4 * 1024) /* 4kB */

/* Read buffer of given size from socket.
 * May return INTERRUPTED or TIMED_OUT
 * If returns ERROR, then errno is set */
Result_t receive_buffer(int sockfd, void *buf, size_t size);

/* Send buffer of given size to socket.
 * May return INTERRUPTED
 * If returns ERROR, then errno is set */
Result_t send_buffer(int sockfd, const void *buf, size_t size);

static inline Result_t
set_send_timeout(int sockfd, struct timeval timeout)
{
    if (setsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout))) {
        return ERROR;
    }

    return OK;
}

static inline Result_t
set_receive_timeout(int sockfd, struct timeval timeout)
{
    if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout))) {
        return ERROR;
    }

    return OK;
}

static inline Result_t
disable_send_timeout(int sockfd)
{
    return set_send_timeout(sockfd, (struct timeval) {0, 0});
}

static inline Result_t
disable_receive_timeout(int sockfd)
{
    return set_send_timeout(sockfd, (struct timeval) {0, 0});
}

#endif

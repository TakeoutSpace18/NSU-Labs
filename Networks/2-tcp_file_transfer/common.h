#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include "error.h"

/* To initiate file transfer, client sends this struct with magic above */
#define BEGIN_FILE_TRANSFER_MAGIC 0x1fab1fab
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

/* After receiving the file server responds with following DTO */
#define FILE_ACCEPTED_MAGIC 0x3fbc3fbc
typedef struct FileAcceptedDTO
{
    uint64_t magic;
}
FileAcceptedDTO_t;

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


#endif /* COMMON_H */

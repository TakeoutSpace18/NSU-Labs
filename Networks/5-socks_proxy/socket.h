#ifndef SOCKET_H
#define SOCKET_H

#include "c.h"

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

#endif /* SOCKET_H */


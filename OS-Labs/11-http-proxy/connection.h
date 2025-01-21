#ifndef CONNECTION_H
#define CONNECTION_H

#include "c.h"
#include "client_context.h"

typedef struct connection {
    int sockfd;
    fdwatcher_t *watcher;
} connection_t;

int connect_domain_name(connection_t *conn, const char *domain, uint16_t port);
void disconnect(connection_t *conn);

#endif /* !CONNECTION_H */

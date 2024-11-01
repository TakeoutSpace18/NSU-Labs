#ifndef SERVER_H
#define SERVER_H

#include <ev.h>

#include "coro.h"
#include "list.h"
#include "c.h"

typedef struct server server_t;
typedef struct client client_t;
typedef void (*client_routine_t)(client_t *c);

struct server {
    ev_io accept_watcher; /* mandatory to be the first field */

    /* coroutine that accepts client connections */
    coro_context context;
    struct ev_loop *loop;

    list_t clients;
    client_routine_t client_routine;
};

struct client {
    ev_io watcher; /* mandatory to be the first field */

    coro_context context;
    struct coro_stack stack;
    list_t link;

    char *description;

    server_t *server_p;
};

static inline int server_client_sockfd(client_t *c) {
    return c->watcher.fd;
}

void server_drop_client(client_t *c);

int server_create(server_t *s, uint16_t port, client_routine_t routine);

void server_run(server_t *s);

void server_yield(client_t *c);


#endif /* SERVER_H */

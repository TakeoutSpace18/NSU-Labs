#ifndef SERVER_H
#define SERVER_H

#include <ev.h>

#include "c.h"
#include "dynarray.h"
#include "client_context.h"

typedef struct server {
    client_routine_t client_routine;
    void *client_routine_arg;

    struct ev_loop *loop;
    ev_io accept_watcher;

    dynarray_t workers;
} server_t;

#define accept_w_2_server(p) container_of(p, server_t, accept_watcher)

int server_create(server_t *s, uint16_t port, client_routine_t routine,
                  void *client_routine_arg, size_t nr_workers);

void server_run(server_t *s);
void server_destroy(server_t *s);

#endif // !SERVER_H

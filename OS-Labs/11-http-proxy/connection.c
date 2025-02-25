#include "connection.h"

#include "c.h"
#include "client_context.h"
#include "coroutine.h"
#include "utils.h"

#include <ares.h>
#include <ev.h>

static int wait_for_connection(connection_t *conn)
{
    /* connection in progress, return control to other coroutines */
    client_fd_setevents(conn->watcher, EV_WRITE);
    for (;;) {
        client_yield();

        int err;
        socklen_t optlen = sizeof(err);
        getsockopt(conn->sockfd, SOL_SOCKET, SO_ERROR, &err, &optlen);

        if (err == EINPROGRESS) {
            continue;
        }
        else if (err == 0) {
            client_fd_setevents(conn->watcher, 0);
            return OK;
        }
        else {
            client_fd_setevents(conn->watcher, 0);
            return ERROR;
        }
    }
}

static int try_connect_node(connection_t *conn, struct ares_addrinfo_node *node)
{
    int type = SOCK_STREAM | SOCK_NONBLOCK;
    if ((conn->sockfd = socket(AF_INET, type, 0)) < 0) {
        coroutine_log_error("Failed to create host socket: %s", strerror(errno));
        return ERROR;
    }

    int cret = connect(conn->sockfd, node->ai_addr, node->ai_addrlen);
    conn->watcher = client_fdwatcher_create(conn->sockfd, 0);
    if (!conn->watcher) {
        close(conn->sockfd);
        return ERROR;
    }

    if (cret == 0) {
        /* connected successfully */
        return OK;
    }
    else if (cret == -1 && errno == EINPROGRESS) {
        /* connection in progress */
        return wait_for_connection(conn);
    }
    else {
        /* connection failed */
        client_fdwatcher_destroy(conn->watcher);
        close(conn->sockfd);
    }

    return ERROR;
}

int connect_domain_name(connection_t *conn, const char *domain, uint16_t port) {
    struct ares_addrinfo *result = NULL;

    struct ares_addrinfo_hints hints = { 0 };
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_flags = 0;

    char port_str[16 + 1];
    snprintf(port_str, 16, "%u", port);

    result = client_dns_resolve(domain, port_str, &hints);
    if (!result) {
        coroutine_log_error("Domain name resolution failed");
        return ERROR;
    }

    conn->sockfd = -1;
    struct ares_addrinfo_node *node;
    for (node = result->nodes; node != NULL; node = node->ai_next) {
        if (try_connect_node(conn, node) == OK) {
            break;
        }
    }

    if (node == NULL) {
        coroutine_log_error("Failed to connect to host %s", domain);
        ares_freeaddrinfo(result);
        return ERROR;
    }

    ares_freeaddrinfo(result);

    coroutine_log_info("Connected to %s:%u", domain, port);
    return OK;
}

void disconnect(connection_t *conn)
{
    client_fdwatcher_destroy(conn->watcher);
    proper_socket_close(conn->sockfd);
}

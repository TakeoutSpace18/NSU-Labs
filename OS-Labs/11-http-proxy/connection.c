#include "connection.h"

#include "c.h"
#include "client_context.h"
#include "coroutine.h"

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
        if ((conn->sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            coroutine_log_error("Failed to create host socket: %s", strerror(errno));
            ares_freeaddrinfo(result);
            return ERROR;
        }

        if (connect(conn->sockfd, node->ai_addr, node->ai_addrlen) == 0) {
            break;
        }

        close(conn->sockfd);
    }

    if (node == NULL) {
        coroutine_log_error("Failed to connect to host %s", domain);
        ares_freeaddrinfo(result);
        return ERROR;
    }

    ares_freeaddrinfo(result);

    conn->watcher = client_fdwatcher_create(conn->sockfd, 0);
    if (!conn->watcher) {
        close(conn->sockfd);
        return ERROR;
    }

    coroutine_log_info("Connected to %s:%u", domain, port);
    return OK;
}

void disconnect(connection_t *conn)
{
    client_fdwatcher_destroy(conn->watcher);
    close(conn->sockfd);
}

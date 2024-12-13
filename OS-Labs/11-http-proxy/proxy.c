#include <stdlib.h>
#include <string.h>
#define _GNU_SOURCE
#include "proxy.h"

#include <ares.h>

#include "c.h"
#include "client_context.h"
#include "http_request.h"

static int connect_to_domain_name(const char *domain, uint16_t port) {
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
        client_log_error("Domain name resolution failed");
        return ERROR;
    }

    int sfd = -1;
    struct ares_addrinfo_node *node;
    for (node = result->nodes; node != NULL; node = node->ai_next) {
        if ((sfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            client_log_error("Failed to create host socket: %s", strerror(errno));
            ares_freeaddrinfo(result);
            return ERROR;
        }

        if (connect(sfd, node->ai_addr, node->ai_addrlen) == 0) {
            break;
        }

        close(sfd);
    }

    if (node == NULL) {
        client_log_error("Failed to connect to host %s", domain);
        ares_freeaddrinfo(result);
        return ERROR;
    }

    ares_freeaddrinfo(result);
    return sfd;
}

static char *extract_domain_name(const char *target)
{
    const char *begin = target;
    const char *end = target + strlen(target);

    const char *protocol = strstr(target, "://");
    if (protocol) {
        begin = protocol + 3;
    }

    char *slash = strchr(begin, '/');

    if (slash) {
        end = slash;
    }

    char *domain = malloc(end - begin + 1);
    if (!domain) {
        client_log_error("out of memory");
        return NULL;
    }

    size_t domain_len = end - begin;
    memcpy(domain, begin, domain_len);
    domain[domain_len] = '\0';

    return domain;
}

void attribute_noreturn() proxy_main(void)
{
    fdwatcher_t client_watcher;

    client_watcher = client_fdwatcher();

    http_request_t request;
    if (http_request_read(&request, client_watcher) != OK) {
        client_drop();
    }

    // puts(request.raw);
    puts(request.method);
    puts(request.target);
    puts(request.version);
    fflush(stdout);

    if (strcmp(http_request_get_method(&request), "CONNECT") == 0) {
        client_log_info("CONNECT method is not supported");
        client_drop();
    }

    const char *domain = extract_domain_name(http_request_get_target(&request));
    if (!domain) {
        http_request_destroy(&request);
        client_drop();
    }


    int host_sfd = connect_to_domain_name(domain, 80);
    if (host_sfd < 0) {
        client_drop();
    }

    client_log_info("Connected!");

    client_drop();
    unreachable();
}

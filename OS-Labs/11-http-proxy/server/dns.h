#ifndef DNS_H
#define DNS_H

#include <ares.h>
#include <ev.h>

#include "c.h"

typedef struct client_context client_context_t;

typedef struct dns_resolver {
    struct ev_io io_watcher;
    struct ev_loop *loop;

    ares_channel channel;
} dns_resolver_t;

typedef struct dns_resolve_request {
    client_context_t *source_client;

    int status;
    int timeouts;
    struct ares_addrinfo *result;

    bool done;
} dns_resolve_request_t;

/* access holder structure from watcher pointer */
#define io_w_2_dns_resolver(p) container_of(p, dns_resolver_t, io_watcher)

int dns_init(void);
void dns_finalize(void);

int dns_resolver_create(dns_resolver_t *d, struct ev_loop *loop);
void dns_resolver_destroy(dns_resolver_t *d);

/* Makes async dns request using c-ares library
 * and switches coroutines internally.
 * RETURNS NULL on failure,
 *         ares_addrinfo list on success,
 *         which should be freed by ares_freeaddrinfo() */
struct ares_addrinfo *client_dns_resolve(const char *node, const char *service,
                                  const struct ares_addrinfo_hints *hints);

#endif /* !DNS_H */

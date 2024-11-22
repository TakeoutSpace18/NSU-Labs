#ifndef DNS_H
#define DNS_H

#include "c.h"

#include <ares.h>
#include <ev.h>

typedef struct client client_t;

typedef struct dns_resolver dns_resolver_t;
typedef struct dns_resolve_request dns_resolve_request_t;

struct dns_resolver {
    struct ev_io io_watcher;
    struct ev_loop *loop;

    ares_channel channel;
};

struct dns_resolve_request {
    client_t *source_client;

    int status;
    int timeouts;
    struct ares_addrinfo *result;

    bool done;
};

/* access holder structure from watcher pointer */
#define io_w_2_dns_resolver(p) container_of(p, dns_resolver_t, io_watcher)
#define timeout_w_2_dns_resolver(p) container_of(p, dns_resolver_t, timeout_watcher)

int dns_init(dns_resolver_t *d, struct ev_loop *loop);
void dns_finalize(dns_resolver_t *d);

/* Makes async dns request using c-ares library
 * and switches coroutines internally.
 * RETURNS NULL on failure,
 *         ares_addrinfo list on success,
 *         which should be freed by ares_freeaddrinfo() */
struct ares_addrinfo *client_dns_resolve(const char *node, const char *service,
                                  const struct ares_addrinfo_hints *hints);

#endif /* !DNS_H */

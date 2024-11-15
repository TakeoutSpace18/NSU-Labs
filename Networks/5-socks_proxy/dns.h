#ifndef DNS_H
#define DNS_H

#include <ares.h>
#include <ev.h>

typedef struct dns_data dns_data_t;

struct dns_data {
    struct ev_io io_watcher;
    struct ev_timer timeout_watcher;
    struct ev_loop *loop;

    ares_channel channel;
};

/* access holder structure from watcher pointer */
#define io_w_2_dns_data(p) container_of(p, dns_data_t, io_watcher)
#define timeout_w_2_dns_data(p) container_of(p, dns_data_t, timeout_watcher)

int dns_init(dns_data_t *d, struct ev_loop *loop);
void dns_finalize(struct dns_data *d);

int dns_resolve(uint32_t *ipv4, const char *name);
int dns_resolve_rev(char *name, uint32_t ipv4);

#endif /* !DNS_H */

#define _GNU_SOURCE
#include "dns.h"

#include <ares.h>
#include <ev.h>

#include "c.h"
#include "client_context.h"
#include "worker_thread.h"
#include "server.h"
#include "log.h"

int dns_init(void)
{
    int ret = ares_library_init(ARES_LIB_INIT_ALL);
    if (ret != ARES_SUCCESS) {
        log_error("Failed to init c-ares library: %s", ares_strerror(ret));
        return -1;
    }

    return OK;
}

void dns_finalize(void)
{
    ares_library_cleanup();
}

/* is always called from server context */
static void io_cb(EV_P_ ev_io *w, int revents) {
    dns_resolver_t *d = io_w_2_dns_resolver(w);

    ares_socket_t rfd, wfd;
    rfd = wfd = ARES_SOCKET_BAD;

    if (read_available(revents))
        rfd = w->fd;
    if (write_available(revents))
        wfd = w->fd;

    ares_process_fd(d->channel, rfd, wfd);
}

/* may be called from client or server context */
static void sock_state_change_cb(void *data_voidp, ares_socket_t sockfd,
                               int readable, int writeable) {
    
    dns_resolver_t *d = (dns_resolver_t *) data_voidp;

    if (ev_is_active(&d->io_watcher) &&  d->io_watcher.fd != sockfd) {
        log_warn("c-ares socket change requested for active io_watcher");
        return;
    }

    if (readable || writeable) {
        log_trace("c-ares sock_state_cb(): starting io_watcher");

        int events = (readable ? EV_READ : 0) | (writeable ? EV_WRITE : 0);
        ev_io_set(&d->io_watcher, sockfd, events);
        ev_io_start(d->loop, &d->io_watcher);
    }
    else {
        log_trace("c-ares sock_state_cb(): stopping io_watcher");

        ev_io_stop(d->loop, &d->io_watcher);
        ev_io_set(&d->io_watcher, -1, 0);
    }
}

int dns_resolver_create(dns_resolver_t *d, struct ev_loop *loop)
{
    memset(d, 0, sizeof(dns_resolver_t));
    d->loop = loop;

    ev_init(&d->io_watcher, io_cb);

    struct ares_options options;
    memset(&options, 0, sizeof(options));
    options.sock_state_cb_data = d;
    options.sock_state_cb = sock_state_change_cb;

    int ret = ares_init_options(&d->channel, &options, ARES_OPT_SOCK_STATE_CB);
    if (ret != ARES_SUCCESS) {
        log_error("Failed to init c-ares channel: %s", ares_strerror(ret));
        return -1;
    }

    return 0;
}

void dns_resolver_destroy(dns_resolver_t *d)
{
    ares_destroy(d->channel);
}


/* is called from server context */
static void
on_getaddrinfo_finish_cb(void *arg, int status, int timeouts,
                         struct ares_addrinfo *result)
{
    log_trace("Received ares_getaddrinfo result");
    dns_resolve_request_t *req = (dns_resolve_request_t *) arg;

    req->status = status;
    req->result = result;
    req->timeouts = timeouts;

    req->done = true;

    /* return control to client that issued the request */
    if (IN_CLIENT_CONTEXT) {
        client_switch_to_loop(req->source_client);
    }
    else {
        loop_switch_to_client(req->source_client);
    }
}

/* should be called from client context */
struct ares_addrinfo *client_dns_resolve(const char *node, const char *service,
                                  const struct ares_addrinfo_hints *hints)
{
    client_context_t *cc = RUNNING_CLIENT;
    dns_resolver_t *r = worker_thread_get_dns_resolver(cc->worker_p);

    dns_resolve_request_t req = { 0 };
    req.source_client = cc;
    req.done = false;

    client_log_trace("DNS Resolving %s:%s...", node, service);
    ares_getaddrinfo(r->channel, node, service, hints, on_getaddrinfo_finish_cb, &req);

    while (!req.done) {
        client_yield();
    }

    if (req.status != ARES_SUCCESS) {
        client_log_info("DNS %s:%s resolve failed: %s", node, service,
                        ares_strerror(req.status));
        return NULL;
    }
    else {
        client_log_info("DNS %s:%s resolve succeded", node, service);
        return req.result;
    }
}

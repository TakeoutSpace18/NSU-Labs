#include "dns.h"

#include <ares.h>

#include "c.h"
#include "log.h"

/* is always called from server context */
static void io_cb (EV_P_ ev_io *w, int revents) {
    dns_data_t *d = io_w_2_dns_data(w);

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
    
    dns_data_t *d = (dns_data_t *) data_voidp;

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

int dns_init(dns_data_t *d, struct ev_loop *loop)
{
    int ret;

    ret = ares_library_init(ARES_LIB_INIT_ALL);
    if (ret != ARES_SUCCESS) {
        log_error("Failed to init c-ares library: %s", ares_strerror(ret));
        return -1;
    }

    memset(d, 0, sizeof(dns_data_t));
    d->loop = loop;
    ev_init(&d->io_watcher, io_cb);
    ev_init(&d->timeout_watcher, io_cb);

    struct ares_options options;
    memset(&options, 0, sizeof(options));
    options.sock_state_cb_data = d;
    options.sock_state_cb = sock_state_change_cb;

    ret = ares_init_options(&d->channel, &options, ARES_OPT_SOCK_STATE_CB);
    if (ret != ARES_SUCCESS) {
        log_error("Failed to init c-ares channel: %s", ares_strerror(ret));
        return -1;
    }

    return 0;
}

void dns_finalize(struct dns_data *d)
{
    ares_destroy(d->channel);
    ares_library_cleanup();
}


/* is called from server context */
static void
on_getaddrinfo_finish_cb(void *arg, int status, int timeouts, struct hostent *host)
{
    /* switch to client context if needed */
}

/* may be called from both server and client contexts */
int dns_resolve(uint32_t *ipv4, const char *name)
{
    /* call ares_getaddrinfo() */
    /* switch to server context if in client */
}

int dns_resolve_rev(char *name, uint32_t ipv4);

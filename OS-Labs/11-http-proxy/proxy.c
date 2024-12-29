#define _GNU_SOURCE
#include "proxy.h"

#include <ev.h>
#include <ares.h>

#include "c.h"
#include "cache.h"
#include "client_context.h"
#include "connection.h"
#include "buffer.h"
#include "coroutine.h"
#include "http.h"

#define FORWARD_BUFFER_SIZE (64 * 1024)

static inline void out_of_memory(void) {
    coroutine_log_fatal("out of memory");
    abort();
}

static bool proxy_request_is_cacheable(http_request_t *r)
{
    return false;

    return memcmp(r->method, "GET", r->method_len) == 0
        || memcmp(r->method, "HEAD", r->method_len) == 0;
}

static int proxy_recv_request_header(buffer_t *buf, http_request_t *parsed,
                                     fdwatcher_t *source)
{
    int err;

    size_t chunk_size = 256;
    for (;;) {
        /* append memory chunk to buffer */
        err = buffer_ensure(buf, chunk_size);
        if (err == ENOMEM) {
            coroutine_log_fatal("out of memory");
            abort();
        }

        ssize_t rret = client_recv(source, buf->pos, buffer_unused(buf), 0);

        if (rret == 0) {
            coroutine_log_error("Failed to read request: socket closed");
            return ERROR;
        }
        else if (rret == -1) {
            coroutine_log_error("Failed to read request: %s", strerror(errno));
            return ERROR;
        }

        /* received some data, try to parse */
        assert(rret > 0);
        size_t last_len = buffer_used(buf);
        buffer_advance(buf, rret);

        int pret = http_parse_request(parsed, buf->start, buffer_used(buf),
                                      last_len);

        if (pret > 0) {
            /* request is complete, set it's final length */
            buffer_set_used(buf, pret);
            return OK;
        }
        else if (pret == -1) {
            coroutine_log_error("Failed to parse request");
            return ERROR;
        }

        /* request is incomplete, continue the loop */
        assert(pret == -2);
    }

    return OK;
}

static int proxy_recv_response_header(buffer_t *buf, size_t *header_len, 
                                      http_response_t *parsed,
                                      fdwatcher_t *source)
{
    int err;
    size_t chunk_size = 256;

    for (;;) {
        /* append memory chunk to buffer */
        err = buffer_ensure(buf, chunk_size);
        if (err == ENOMEM) {
            out_of_memory();
        }

        ssize_t rret = client_recv(source, buf->pos, buffer_unused(buf), 0);

        if (rret == 0) {
            coroutine_log_error("Failed to read response header: socket closed");
            return ERROR;
        }
        else if (rret == -1) {
            coroutine_log_error("Failed to read response header: %s",
                                strerror(errno));
            return ERROR;
        }

        /* received some data, try to parse */
        assert(rret > 0);
        size_t last_len = buffer_used(buf);
        buffer_advance(buf, rret);

        int pret = http_parse_response(parsed, buf->start, buffer_used(buf),
                                       last_len);

        if (pret > 0) {
            /* response header is complete, some data from body may be already read */
            *header_len = pret;
            return OK;
        }
        else if (pret == -1) {
            coroutine_log_error("Failed to parse response");
            return ERROR;
        }

        /* header is incomplete, continue the loop */
        assert(pret == -2);
    }

    return OK;
}


static ssize_t proxy_recv(fdwatcher_t *source, void *p, size_t len)
{
    ssize_t rret = client_recv(source, p, len, 0);

    if (rret == -1) {
        coroutine_log_error("Failed to receive data: %s", strerror(errno));
        return rret;
    }

    coroutine_log_trace("Received %zi bytes", rret);
    return rret;
}

static ssize_t proxy_recv_all(fdwatcher_t *source, void *p, size_t len)
{
    ssize_t rret = client_recv_all(source, p, len);

    if (rret == -1) {
        coroutine_log_error("Failed to receive data: %s", strerror(errno));
        return rret;
    }

    coroutine_log_trace("Received %zi bytes", rret);
    return rret;
}

static ssize_t proxy_send(fdwatcher_t *target, void *p, size_t len)
{
    ssize_t rret = client_send(target, p, len, 0);

    if (rret == -1) {
        coroutine_log_error("Failed to send data: %s", strerror(errno));
        return rret;
    }

    coroutine_log_trace("Sent %zi bytes", rret);
    return rret;
}

static ssize_t proxy_send_all(fdwatcher_t *target, void *p, size_t len)
{
    ssize_t rret = client_send_all(target, p, len);

    if (rret == -1) {
        coroutine_log_error("Failed to send data: %s", strerror(errno));
        return rret;
    }

    coroutine_log_trace("Sent %zi bytes", rret);
    return rret;
}

static int proxy_send_buffer(buffer_t *buf, fdwatcher_t *target)
{
    ssize_t sret = proxy_send_all(target, buf->start, buffer_used(buf));
    if (sret == 0) {
        coroutine_log_error("Failed to send buffer: socket was closed");
        return ERROR;
    }
    else if (sret < 0) {
        return ERROR;
    }

    return OK;
}

static int proxy_forward_chunked(buffer_t *buf, size_t header_len,
                                 fdwatcher_t *host, fdwatcher_t *client,
                                 cache_entry_t *cache_entry)
{
    coroutine_log_error("Chunked encoding is not supported");
    return ERROR;
}

static int proxy_forward_fixed_len(buffer_t *buf, size_t len,
                                 fdwatcher_t *host, fdwatcher_t *client,
                                 cache_entry_t *cache_entry)
{
    if (proxy_send_buffer(buf, client) != OK) {
        return ERROR;
    }

    if (cache_entry) {
        cache_entry_append_data(cache_entry, buf->start, buffer_used(buf));
    }

    buffer_reset(buf);
    if (buffer_ensure(buf, FORWARD_BUFFER_SIZE) == ENOMEM) {
        out_of_memory();
    }

    size_t total_size = 0;

    while (total_size < len) {
        ssize_t rret = proxy_recv(host, buf->start, FORWARD_BUFFER_SIZE);

        if (rret == 0) {
            coroutine_log_error("Failed to forward: socket closed");
            return ERROR;
        }
        else if (rret == -1) {
            return ERROR;
        }

        assert(rret > 0);

        ssize_t sret = proxy_send_all(client, buf->start, rret);
        if (sret == 0) {
            coroutine_log_error("Failed to forward: client closed socket");
            return ERROR;
        }
        else if (sret != rret) {
            return ERROR;
        }

        if (cache_entry) {
            cache_entry_append_data(cache_entry, buf->start, rret);
        }

        total_size += rret;
    }

    return OK;
}

static int proxy_forward_till_sock_close(buffer_t *buf, fdwatcher_t *host,
                                         fdwatcher_t *client,
                                         cache_entry_t *cache_entry)
{
    if (proxy_send_buffer(buf, client) != OK) {
        return ERROR;
    }

    if (cache_entry) {
        cache_entry_append_data(cache_entry, buf->start, buffer_used(buf));
    }

    buffer_reset(buf);
    if (buffer_ensure(buf, FORWARD_BUFFER_SIZE) == ENOMEM) {
        out_of_memory();
    }

    while (true) {
        ssize_t rret = proxy_recv(host, buf->start, FORWARD_BUFFER_SIZE);

        if (rret == 0) {
            return OK;
        }
        else if (rret == -1) {
            return ERROR;
        }

        assert(rret > 0);

        ssize_t sret = proxy_send_all(client, buf->start, rret);
        if (sret == 0) {
            coroutine_log_error("Failed to forward: client closed socket");
            return ERROR;
        }
        else if (sret != rret) {
            return ERROR;
        }

        if (cache_entry) {
            cache_entry_append_data(cache_entry, buf->start, FORWARD_BUFFER_SIZE);
        }
    }

    return OK;
}

static int proxy_send_response_from_cache(cache_entry_t *cache_entry,
                                          fdwatcher_t *client)
{
    /* not implemented yet */

    return ERROR;
}

static int proxy_send_response_from_host(cache_entry_t *cache_entry,
                                         fdwatcher_t *host, fdwatcher_t *client)
{
    int err;
    buffer_t buf;
    buffer_init(&buf);

    http_response_t header;
    size_t header_len;
    if (proxy_recv_response_header(&buf, &header_len, &header, host) != OK) {
        goto error;
    }

    http_response_print(&header);

    size_t content_length;
    if (http_response_is_chunked(&header)) {
        err = proxy_forward_chunked(&buf, header_len, host, client, cache_entry);
    }
    else if (http_get_content_length(header.headers, header.num_headers,
                                     &content_length)) {
        size_t len = content_length - (buffer_used(&buf) - header_len);
        err = proxy_forward_fixed_len(&buf, len, host, client, cache_entry);
    }
    else {
        err = proxy_forward_till_sock_close(&buf, host, client, cache_entry);
    }

    if (err != OK) {
        goto error;
    }

    buffer_free(&buf);
    return OK;

error:
    buffer_free(&buf);
    return ERROR;
}

void attribute_noreturn() proxy_main(void *arg)
{
    int err;

    cache_t     *cache = (cache_t *) arg;
    const char  *domain = NULL;
    bool         request_cached = false;

    buffer_t *buf = malloc(sizeof(*buf));
    if (!buf) {
        out_of_memory();
    }
    buffer_init(buf);

    http_request_t request;
    err = proxy_recv_request_header(buf, &request, client_fdwatcher());
    if (err != OK) {
        goto finish;
    }

    http_request_print(&request);

    if (memcmp(request.method, "CONNECT", request.method_len) == 0) {
        coroutine_log_info("CONNECT method is not supported");
        goto finish;
    }

    cache_entry_t *cache_entry = cache_get_entry(cache, buf);

    if (cache_entry) {
        /* request is present in cache */
        proxy_send_response_from_cache(cache_entry, client_fdwatcher());
    }
    else {
        if (proxy_request_is_cacheable(&request)) {
            cache_entry = cache_entry_create(cache);
            if (!cache_entry) {
                goto finish;
            }

            err = cache_add_entry(cache, buf, cache_entry);
            if (err != OK) {
                goto finish;
            }

            request_cached = true;
        }

        connection_t host;
        domain = http_request_get_host(&request);
        err = connect_domain_name(&host, domain, 80);
        if (err != OK) {
            goto finish;
        }

        /* send request to host */
        if (proxy_send_buffer(buf, host.watcher) != OK) {
            disconnect(&host);
            goto finish;
        }

        proxy_send_response_from_host(cache_entry, host.watcher, client_fdwatcher());
        disconnect(&host);
    }


finish:
    if (!request_cached) {
        buffer_free(buf);
        free(buf);
    }

    client_drop();

    unreachable();
}

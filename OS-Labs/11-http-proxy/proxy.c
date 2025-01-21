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
            /* Some part of request body may be also received to buf,
             * but proxying request bodies is not implemented in this version. */
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

static ssize_t proxy_send_all(fdwatcher_t *target, const void *p, size_t len)
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
    if (proxy_send_buffer(buf, client) != OK) {
        goto error;
    }

    if (cache_entry) {
        cache_entry_append_data(cache_entry, buf->start, buffer_used(buf));
    }

    http_chunk_t chunks[32];
    size_t n_parsed = header_len;

    while (true) {
        size_t num_chunks;
        ssize_t pret = http_parse_chunks(chunks, 32, &num_chunks,
                                         buf->start + n_parsed,
                                         buffer_used(buf) - n_parsed);

        if (pret == -1) {
            coroutine_log_error("Failed to forward: can't parse chunked encoding");
            goto error;
        }
        else {
            n_parsed += pret;
        }

        /* finish on zero chunk */
        if (num_chunks >= 1 && chunks[num_chunks - 1].len == 0) {
            break;
        }

        const size_t nread = 16384;
        if (buffer_ensure(buf, nread) == ENOMEM) {
            out_of_memory();
        }
        ssize_t rret = proxy_recv(host, buf->pos, buffer_unused(buf));

        if (rret == 0) {
            coroutine_log_error("Failed to forward: host closed socket");
            goto error;
        }
        else if (rret == -1) {
            goto error;
        }

        assert(rret > 0);

        ssize_t sret = proxy_send_all(client, buf->pos, rret);
        if (sret == 0) {
            coroutine_log_error("Failed to forward: client closed socket");
            goto error;
        }
        else if (sret != rret) {
            goto error;
        }

        if (cache_entry) {
            cache_entry_append_data(cache_entry, buf->pos, rret);
        }

        buffer_advance(buf, rret);
    }

    if (cache_entry) {
        cache_entry_finished(cache_entry);
    }
    return OK;

error:
    if (cache_entry) {
        cache_entry_finished(cache_entry);
    }
    return ERROR;
}

static int proxy_forward_fixed_len(buffer_t *buf, size_t len,
                                 fdwatcher_t *host, fdwatcher_t *client,
                                 cache_entry_t *cache_entry)
{
    if (proxy_send_buffer(buf, client) != OK) {
        goto error;
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
            coroutine_log_error("Failed to forward: host socket closed");
            goto error;
        }
        else if (rret == -1) {
            goto error;
        }

        assert(rret > 0);

        ssize_t sret = proxy_send_all(client, buf->start, rret);
        if (sret == 0) {
            coroutine_log_error("Failed to forward: client socket closed");
            goto error;
        }
        else if (sret != rret) {
            goto error;
        }

        if (cache_entry) {
            cache_entry_append_data(cache_entry, buf->start, rret);
        }

        total_size += rret;
    }

    if (cache_entry) {
        cache_entry_finished(cache_entry);
    }
    return OK;

error:
    if (cache_entry) {
        cache_entry_finished(cache_entry);
    }
    return ERROR;
}

static int proxy_forward_till_sock_close(buffer_t *buf, fdwatcher_t *host,
                                         fdwatcher_t *client,
                                         cache_entry_t *cache_entry)
{
    if (proxy_send_buffer(buf, client) != OK) {
        goto error;
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
            goto finish;
        }
        else if (rret == -1) {
            goto error;
        }

        assert(rret > 0);

        ssize_t sret = proxy_send_all(client, buf->start, rret);
        if (sret == 0) {
            coroutine_log_error("Failed to forward: client closed socket");
            goto error;
        }
        else if (sret != rret) {
            goto error;
        }

        if (cache_entry) {
            cache_entry_append_data(cache_entry, buf->start, rret);
        }
    }

finish:
    if (cache_entry) {
        cache_entry_finished(cache_entry);
    }
    return OK;

error:
    if (cache_entry) {
        cache_entry_finished(cache_entry);
    }
    return ERROR;
}

static int proxy_send_response_from_cache(cache_entry_t *cache_entry,
                                          fdwatcher_t *client)
{
    size_t sent_total = 0;
    for (;;) {
        char *data_start;
        size_t total_data_len;

        cache_entry_read_begin(cache_entry);
        cache_entry_get_data(cache_entry, &data_start, &total_data_len);

        const char *chunk = data_start + sent_total;
        ssize_t chunk_size = total_data_len - sent_total;

        if (chunk_size != 0) {
            client_fd_setevents(client, EV_WRITE);
            ssize_t sret = proxy_send_all(client, chunk, chunk_size);

            if (sret == 0) {
                coroutine_log_error("Failed to send response from cache: "
                                    "client socket closed");
                goto error;
            }
            else if (sret == -1) {
                goto error;
            }

            sent_total += sret;
            coroutine_log_trace("Sent %zi cached bytes", sret);
        }

        cache_entry_read_end(cache_entry);

        if (sent_total == total_data_len){
            /* wait for more data to appear in cache entry */
            client_fd_setevents(client, 0);
            if (cache_entry_wait(cache_entry, &total_data_len) != OK) {
                if (sent_total == total_data_len) {
                    break;
                }
            }
        }
    }
    
    client_fd_setevents(client, 0);
    return OK;

error:
    client_fd_setevents(client, 0);
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
    bool         request_added_to_cache = false;

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

    /* find request in cache or add new cache entry */
    cache_entry_t *cache_entry = NULL;
    bool is_in_cache = false;
    if (proxy_request_is_cacheable(&request)) {
        if ((cache_entry = cache_get_entry(cache, buf)) != NULL) {
            is_in_cache = true;
        }
        else {
            coroutine_log_debug("Adding cache entry");
            int err;
            cache_entry = cache_add_entry(cache, buf, &err);
            if (err == EEXIST) {
                is_in_cache = true;
            }
            else if (err == OK){
                request_added_to_cache = true;
            }
            else {
                goto finish;
            }
        }
    }

    if (is_in_cache) {
        /* request is present in cache */
        coroutine_log_debug("Request found in cache");
        err = proxy_send_response_from_cache(cache_entry, client_fdwatcher());
        cache_entry_put(cache_entry);

        if (err != OK) {
            goto finish;
        }
    }
    else {
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

        err = proxy_send_response_from_host(cache_entry, host.watcher, client_fdwatcher());
        if (err != OK && cache_entry != NULL) {
            cache_remove_entry(cache, buf);
            coroutine_log_debug("Removed entry from cache");
        }
        disconnect(&host);
    }


finish:
    if (!request_added_to_cache) {
        buffer_free(buf);
        free(buf);
    }

    client_drop();

    unreachable();
}

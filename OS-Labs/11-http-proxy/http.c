#include "http.h"

#include "buffer.h"
#include "c.h"
#include "client_context.h"
#include "coroutine.h"

static int read_header(buffer_t *buf, fdwatcher_t *source)
{
    size_t size = 512;
    buffer_ensure(buf, size);

    const char *delim = "\r\n\r\n";
    for (;;) {
        ssize_t ret = client_recv_until(source, buf->start, buffer_unused(buf),
                                        delim, strlen(delim));

        if (ret > 0) {
            buffer_advance(buf, ret);
            break;
        }

        if (ret == 0) {
            coroutine_log_error("Failed to read header: socket closed");
            return ERROR;
        }

        if (ret == -1) {
            coroutine_log_error("Failed to read header: %s", strerror(errno));
            return ERROR;
        }

        if (ret == NODATA) {
            size *= 2;
            buffer_ensure(buf, size);
        }
    }

    return OK;
}

static int read_till_socket_close(buffer_t *buf, fdwatcher_t *source) {
    size_t size = 512;
    for (;;) {
        if (buffer_unused(buf) <= 0) {
            size *= 2;
            buffer_ensure(buf, size);
        }

        ssize_t ret = client_recv(source, buf->pos, buffer_unused(buf), 0);

        if (ret > 0) {
            buffer_advance(buf, ret);
        }
        else if (ret == 0) {
            return OK;
        }
        else if (ret == -1) {
            coroutine_log_error("Failed to read till socket close: %s", strerror(errno));
            return ERROR;
        }
    }
}

int http_request_read(http_request_t *request, fdwatcher_t *source)
{
    memset(request, 0, sizeof(*request));

    int err = read_header(&request->raw, source);
    if (err != OK) {
        http_request_destroy(request);
        return ERROR;
    }

    /* to treat header as a string */
    buffer_add(&request->raw, "\0", 1);

    /* Parse first line of the request: 
     * <method> <target> <version> */
    char *method_begin = request->raw.start;
    char *method_end = strchr(request->raw.start, ' ');
    memset(request->method, 0, sizeof(request->method));
    memcpy(request->method, method_begin, method_end - method_begin);

    char *target_begin = method_end + 1;
    char *target_end = strchr(target_begin, ' ');
    request->target = calloc(1, target_end - target_begin + 1);
    if (!request->target) {
        coroutine_log_error("out of memory");
        http_request_destroy(request);
        return ERROR;
    }
    memcpy(request->target, target_begin, target_end - target_begin);

    char *version_begin = target_end + 1;
    char *version_end = strstr(version_begin, "\r\n");
    memset(request->version, 0, sizeof(request->version));
    memcpy(request->version, version_begin, version_end - version_begin);

    return OK;
}

void http_request_destroy(http_request_t *request)
{
    buffer_free(&request->raw);
    free(request->target);
}

int http_request_send(const http_request_t *request, fdwatcher_t *target)
{
    ssize_t err = client_send_buf(target, request->raw.start, buffer_used(&request->raw));
    if (err == 0) {
        coroutine_log_error("Failed to send request: socket closed");
        return ERROR;
    }
    if (err == -1) {
        coroutine_log_error("Falied to send request: %s", strerror(errno));
        return ERROR;
    }

    return OK;
}

int http_response_read(http_response_t *response, fdwatcher_t *source)
{
    int err;

    err = read_header(&response->raw, source);
    if (err != OK) {
        http_response_destroy(response);
        return ERROR;
    }

    err = read_till_socket_close(&response->raw, source);
    if (err != OK) {
        http_response_destroy(response);
        return ERROR;
    }

    /* treat response as a string */
    buffer_add(&response->raw, "\0", 1);

    return OK;
}

int http_response_send(const http_response_t *response, fdwatcher_t *target)
{
    ssize_t err = client_send_buf(target, response->raw.start, buffer_used(&response->raw));
    if (err == 0) {
        coroutine_log_error("Failed to send response: socket closed");
        return ERROR;
    }
    if (err == -1) {
        coroutine_log_error("Falied to send response: %s", strerror(errno));
        return ERROR;
    }

    return OK;
}

void http_response_destroy(http_response_t *response)
{
    buffer_free(&response->raw);
}


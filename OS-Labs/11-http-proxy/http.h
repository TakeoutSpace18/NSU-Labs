#ifndef HTTP_H
#define HTTP_H

#include "c.h"
#include "client_context.h"
#include "buffer.h"

typedef struct http_request {
    buffer_t raw;
    char method[16];
    char version[16];
    char *target;
} http_request_t;

int http_request_read(http_request_t *request, fdwatcher_t *source);
int http_request_send(const http_request_t *request, fdwatcher_t *target);
void http_request_destroy(http_request_t *request);

static inline char *http_request_get_method(http_request_t *request)
{
    return request->method;
}

static inline char *http_request_get_target(http_request_t *request)
{
    return request->target;
}

static inline char *http_request_get_get_version(http_request_t *request)
{
    return request->version;
}

typedef struct http_response {
    buffer_t raw;
} http_response_t;

int http_response_read(http_response_t *response, fdwatcher_t *source);
int http_response_send(const http_response_t *response, fdwatcher_t *target);
void http_response_destroy(http_response_t *response);

#endif /* HTTP_H */

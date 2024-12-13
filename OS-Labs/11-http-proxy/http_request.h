#ifndef HTTP_REQUEST
#define HTTP_REQUEST

#include "c.h"
#include "client_context.h"

typedef struct http_request http_request_t;

struct http_request {
    char *raw;
    char method[16];
    char version[16];
    char *target;
};

int http_request_read(http_request_t *request, fdwatcher_t source);
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

#endif /* HTTP_REQUEST */

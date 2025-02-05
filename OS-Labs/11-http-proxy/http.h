#ifndef HTTP_H
#define HTTP_H

#include "c.h"
#include "picohttpparser.h"

#define MAX_HEADERS 100

typedef struct http_request {
    const char *method;
    size_t method_len;
    const char *path;
    size_t path_len;
    int minor_version;
    struct phr_header headers[MAX_HEADERS];
    size_t num_headers;
} http_request_t;

typedef struct http_response {
    int minor_version;
    int status;
    const char *msg;
    size_t msg_len;
    struct phr_header headers[MAX_HEADERS];
    size_t num_headers;
} http_response_t;

typedef struct http_chunk {
    size_t len;
    const char *data;
} http_chunk_t;

/* returns number of bytes consumed if successful, -2 if request is partial,
 * -1 if failed */
int http_parse_request(http_request_t *request, const char *buf_start,
                       size_t len, size_t last_len);

/* returns number of bytes consumed if successful, -2 if response is partial,
 * -1 if failed */
int http_parse_response(http_response_t *response, const char *buf_start,
                        size_t len, size_t last_len);

/* returns number of bytes consumed if successful, -1 if failed */
int http_parse_chunks(http_chunk_t *chunks, size_t max_chunks,
                      size_t *num_chunks, const char *buf, size_t len);

struct phr_header *http_find_header(struct phr_header *headers,
                                    size_t num_headers, const char *name);

bool http_response_is_chunked(http_response_t *parsed);
  
bool http_get_content_length(struct phr_header *headers, size_t num_headers,
                             size_t *value);

/* returns malloced string */
const char *http_request_get_host(http_request_t *parsed);

void http_request_print(http_request_t *request);
void http_response_print(http_response_t *request);

#endif /* !HTTP_H */

#include "http.h"

#include "coroutine.h"
#include "utils.h"

int http_parse_request(http_request_t *r, const char *buf_start,
                       size_t len, size_t last_len)
{

    r->num_headers = sizeof(r->headers) / sizeof(r->headers[0]);
    return phr_parse_request(buf_start, len,
                             &r->method, &r->method_len,
                             &r->path, &r->path_len,
                             &r->minor_version,
                             r->headers, &r->num_headers,
                             last_len);
}

int http_parse_response(http_response_t *r, const char *buf_start,
                        size_t len, size_t last_len)
{
    r->num_headers = sizeof(r->headers) / sizeof(r->headers[0]);
    return phr_parse_response(buf_start, len,
                              &r->minor_version, &r->status,
                              &r->msg, &r->msg_len,
                              r->headers, &r->num_headers,
                              last_len);
}

int http_parse_chunks(http_chunk_t *chunks, size_t max_chunks,
                      size_t *num_chunks, const char *buf, size_t len)
{
    *num_chunks = 0;
    size_t bytes_consumed = 0;
    const char *pos = buf;
    while (true) {
        if (*num_chunks == max_chunks) {
            break;
        }

        const char *crlf = strstr(pos, "\r\n");
        if (crlf == NULL) {
            break;
        }
        chunks[*num_chunks].len = strtoull_n(pos, crlf - pos, 16);
        chunks[*num_chunks].data = crlf + 2;

        if (crlf + 2 + chunks[*num_chunks].len + 2 > buf + len) {
            break;
        }

        bytes_consumed += crlf - pos + 2 + chunks[*num_chunks].len + 2;
        pos = chunks[*num_chunks].data + chunks[*num_chunks].len + 2;
        *num_chunks += 1;
    }

    return bytes_consumed;
}

void http_request_print(http_request_t *d)
{
    printf("%.*s %.*s HTTP/1.%d\n",
           (int)d->method_len, d->method,
           (int)d->path_len, d->path,
           d->minor_version);

    for (size_t i = 0; i < d->num_headers; i++) {
        printf("%.*s: %.*s\n",
               (int)d->headers[i].name_len, d->headers[i].name,
               (int)d->headers[i].value_len, d->headers[i].value);
    }

    printf("\n");
}

void http_response_print(http_response_t *d)
{
    printf("HTTP/1.%d %d %.*s\n", 
           d->minor_version, 
           d->status, 
           (int)d->msg_len, 
           d->msg);

    for (size_t i = 0; i < d->num_headers; i++) {
        printf("%.*s: %.*s\n", 
               (int)d->headers[i].name_len, d->headers[i].name,
               (int)d->headers[i].value_len, d->headers[i].value);
    }

    printf("\n");
}

struct phr_header *http_find_header(struct phr_header *headers,
                                    size_t num_headers, const char *name) {
    for (size_t i = 0; i < num_headers; ++i) {
        if (memcmp(headers[i].name, name, headers[i].name_len) == 0) {
            return &headers[i];
        }
    }

    return NULL;
}

 bool http_response_is_chunked(http_response_t *response)
{
    struct phr_header *hdr = http_find_header(response->headers,
                                         response->num_headers,
                                         "Transfer-Encoding");
    if (!hdr) {
        return false;
    }

    return memcmp(hdr->value, "chunked", hdr->value_len) == 0;
}

bool http_get_content_length(struct phr_header *headers, size_t num_headers,
                             size_t *value)
{
    struct phr_header *hdr = http_find_header(headers, num_headers,
                                              "Content-Length");
    if (!hdr) {
        return false;
    }

    *value = strtoull_n(hdr->value, hdr->value_len, 10);
    return true;
}

const char *http_request_get_host(http_request_t *request)
{
    struct phr_header *host_hdr = http_find_header(request->headers,
                                              request->num_headers, "Host");

    /* Host header is mandatory in HTTP 1.1 */
    assert(host_hdr);

    const char *dup = strndup(host_hdr->value, host_hdr->value_len);
    if (!dup) {
        coroutine_log_fatal("out of memory");
        abort();
    }

    return dup;
}

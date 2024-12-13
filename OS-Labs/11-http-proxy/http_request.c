#include "http_request.h"
#include "c.h"
#include "client_context.h"
#include <string.h>

static inline bool is_full_request(char *str)
{
    return strstr(str, "\r\n\r\n") != NULL;
}

static int read_raw_request(char **dest, fdwatcher_t source)
{
    size_t max_size = 512;
    size_t recv_size = 0;
    char *buf = calloc(1, max_size);
    if (!buf) {
        goto out_of_mem;
    }

    client_fd_setevents(source, EV_READ);
    while (!is_full_request(buf)) {
        size_t sz = max_size - recv_size - 1; /* -1 for null terminator */
        ssize_t ret = client_recv(source, buf + recv_size, sz, 0);
        if (ret == 0) {
            return ERROR;
        }
        if (ret < 0) {
            client_log_error("client_recv() failed: %s", strerror(errno));
            return ERROR;
        }

        recv_size += ret;

        if (recv_size == max_size - 1) {
            max_size *= 2;
            char *new_buf = realloc(buf, max_size);
            if (!new_buf) {
                goto out_of_mem;
            }
            memset(new_buf + recv_size, 0, max_size - recv_size);
            buf = new_buf;
        }

    }

    *dest = buf;
    return OK;

out_of_mem:
    client_log_error("out of memory");
    free(buf);

    return ERROR;
}

int http_request_read(http_request_t *request, fdwatcher_t source)
{
    int err = read_raw_request(&request->raw, source);
    if (err != OK) {
        return err;
    }

    /* Parse first line of the request: 
     * <method> <target> <version> */
    char *method_begin = request->raw;
    char *method_end = strchr(request->raw, ' ');
    memset(request->method, 0, sizeof(request->method));
    memcpy(request->method, method_begin, method_end - method_begin);

    char *target_begin = method_end + 1;
    char *target_end = strchr(target_begin, ' ');
    request->target = calloc(1, target_end - target_begin + 1);
    if (!request->target) {
        client_log_error("out of memory");
        free(request->raw);
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
    free(request->raw);
    free(request->target);
}

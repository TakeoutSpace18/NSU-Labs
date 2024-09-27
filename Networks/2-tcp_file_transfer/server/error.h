#ifndef ERROR_H
#define ERROR_H

typedef enum {
    OK,
    ERROR,
    CLIENTS_LIMIT_REACHED,
    TIMED_OUT,
    INTERRUPTED
} Result_t;

#define ERR_DESCR_BUFSIZE 512
extern char err_descr[];

#define set_errdescr(format, ...) \
    snprintf(err_descr, ERR_DESCR_BUFSIZE, format, ##__VA__ARGS__)


#endif

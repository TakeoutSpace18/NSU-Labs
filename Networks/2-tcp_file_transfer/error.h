#ifndef ERROR_H
#define ERROR_H

typedef enum {
    OK,
    ERROR,
    CLIENTS_LIMIT_REACHED,
    TIMED_OUT,
    INTERRUPTED
} Result_t;

#endif

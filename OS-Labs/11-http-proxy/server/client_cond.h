#ifndef CLIENT_COND_H
#define CLIENT_COND_H

#include "dynarray.h"

typedef struct client_cond {
    dynarray_t waiters;
    pthread_mutex_t lock;
} client_cond_t;

void client_cond_create(client_cond_t *cond);
void client_cond_destroy(client_cond_t *cond);

void client_cond_wait(client_cond_t *cond);
void client_cond_signal(client_cond_t *cond);

#endif /* !CLIENT_COND_H */

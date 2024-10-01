#ifndef CLIENT_THREAD_H
#define CLIENT_THREAD_H

#include <pthread.h>

#include "error.h"

#define MAX_ALLOWED_CLIENTS 16

typedef struct Client
{
    pthread_t thread;
    int sockfd;
}
ClientData_t;

typedef struct ActiveClients
{
    ClientData_t *list[MAX_ALLOWED_CLIENTS];
    size_t count;
    pthread_mutex_t mutex;
}
ActiveClients_t;

void clients_list_init(ActiveClients_t *active_clients);
Result_t create_client_thread(ActiveClients_t *clients, int sockfd);
Result_t shutdown_active_clients(void);

#endif /* CLIENT_THREAD_H */

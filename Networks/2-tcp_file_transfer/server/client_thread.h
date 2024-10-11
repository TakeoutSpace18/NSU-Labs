#ifndef CLIENT_THREAD_H
#define CLIENT_THREAD_H

#include <pthread.h>

#include "error.h"
#include "common.h"

#define MAX_ALLOWED_CLIENTS 16
#define SPD_UPDATE_MSECS 100
#define REPORT_INTERVAL_MSECS 2000

typedef struct ActiveClients ActiveClients_t;

typedef struct Client
{
    pthread_t thread;
    int sockfd;
    char description[SOCKADDR2STR_MAX_BUFSIZE];
    ActiveClients_t *active_clients_p;
}
ClientData_t;

typedef struct ActiveClients
{
    ClientData_t *list[MAX_ALLOWED_CLIENTS];
    size_t count;
    pthread_mutex_t mutex;
}
ActiveClients_t;

typedef struct FileData
{
    FILE *f;
    char *pathname;
}
FileData_t;

extern const char *storage_dir; /* defined in receiver_server.c */

void clients_list_init(ActiveClients_t *active_clients);
Result_t create_client_thread(ActiveClients_t *clients, int sockfd,
                              const char *description);
Result_t shutdown_active_clients(ActiveClients_t *clients);

#endif /* CLIENT_THREAD_H */

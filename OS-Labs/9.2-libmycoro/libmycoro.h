#ifndef LIBMYTHREAD_H
#define LIBMYTHREAD_H

#define MYCORO_ERROR -1
#define MYCORO_OK 0

typedef unsigned long long mycoro_t;
typedef void (*start_routine_t)(void *);

int mycoro_init(mycoro_t *main);

int mycoro_create(mycoro_t *coro, start_routine_t func, void *arg);
void mycoro_destroy(mycoro_t coro);

void mycoro_switch(mycoro_t from, mycoro_t to);

extern _Thread_local char __mycoro_error_descr[512];

static inline const char *mycoro_strerror(void)
{
    return __mycoro_error_descr;
}

#endif /* LIBMYTHREAD_H */


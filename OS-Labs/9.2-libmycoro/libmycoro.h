#ifndef LIBMYCORO_H
#define LIBMYCORO_H

#define MYCORO_ERROR -1
#define MYCORO_OK 0

typedef unsigned long long mycoro_t;
typedef void *(*start_routine_t)(void *);

/* should be called before any other functions */
int mycoro_init(void);

int mycoro_create(mycoro_t *coro, start_routine_t func, void *arg);

void mycoro_yield(void);

void mycoro_join(mycoro_t coro, void **ret);

extern _Thread_local char __mycoro_error_descr[512];

static inline const char *mycoro_strerror(void)
{
    return __mycoro_error_descr;
}

#endif /* LIBMYCORO_H */


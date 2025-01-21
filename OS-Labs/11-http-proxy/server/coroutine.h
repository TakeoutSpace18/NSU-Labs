#ifndef COROUTINE_H
#define COROUTINE_H

#include <coro.h>
#include <threads.h>

#include "c.h"
#include "log.h"

typedef void (*coroutine_func_t)(void *);

typedef struct coroutine {
    struct coro_context ctx;
    struct coro_stack stack;
    char *name;
    void *data; /* user data */
} coroutine_t;

extern thread_local coroutine_t *__current_coroutine;

int coroutine_create(coroutine_t *c, coroutine_func_t func, void *arg);
void coroutine_create_initial(coroutine_t *c);
void coroutine_destroy(coroutine_t *c);

void coroutine_set_name(coroutine_t *c, const char *name);

static inline coroutine_t *coroutine_get_current(void)
{
    return __current_coroutine;
}

static inline void coroutine_switch(coroutine_t *prev, coroutine_t *next)
{
    log_trace("context switch: [%s] --> [%s]",
              prev->name ? prev->name : "???",
              next->name ? next->name : "???");

    __current_coroutine = next;
    coro_transfer(&prev->ctx, &next->ctx);
}

__attribute__ ((format (printf, 4, 5)))
void coroutine_log(int level, const char *file, int line, const char *fmt, ...);

#define coroutine_log_trace(...) coroutine_log(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define coroutine_log_debug(...) coroutine_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define coroutine_log_info(...)  coroutine_log(LOG_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define coroutine_log_warn(...)  coroutine_log(LOG_WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define coroutine_log_error(...) coroutine_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define coroutine_log_fatal(...) coroutine_log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

#endif /* !COROUTINE_H */

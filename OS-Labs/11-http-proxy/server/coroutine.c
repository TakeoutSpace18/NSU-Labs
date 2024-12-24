#include "coroutine.h"

thread_local coroutine_t *__current_coroutine;

int coroutine_create(coroutine_t *c, coroutine_func_t func, void *arg)
{
    memset(c, 0, sizeof(coroutine_t));

    if (!coro_stack_alloc(&c->stack, 0)) {
        log_error("Failed to allocate coroutine stack");
        return ERROR;
    }

    coro_create(&c->ctx, func, arg, c->stack.sptr, c->stack.ssze);

    return OK;
}

void coroutine_create_initial(coroutine_t *c)
{
    memset(c, 0, sizeof(*c));
    coro_create(&c->ctx, NULL, NULL, NULL, 0);
    __current_coroutine = c;
}

void coroutine_destroy(coroutine_t *c)
{
    coro_stack_free(&c->stack);
    free(c->name);
}

void coroutine_set_name(coroutine_t *c, const char *name)
{
    if (c->name) {
        free(c->name);
    }
    c->name = strdup(name);
}

void coroutine_log(int level, const char *file, int line, const char *fmt, ...)
{
    coroutine_t *current = __current_coroutine;

    static char extended_fmt[512] = { 0 };
    snprintf(extended_fmt, 512, "[%s] %s", current->name ? current->name : "???", fmt);

    va_list ap;
    va_start(ap, fmt);
    vlog_log(level, file, line, extended_fmt, ap);
    va_end(ap);
}


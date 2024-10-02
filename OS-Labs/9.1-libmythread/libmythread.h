#ifndef LIBMYTHREAD_H
#define LIBMYTHREAD_H

typedef unsigned long long MyThread_t;

typedef enum MyThreadResult {
    OK,
    ERROR
} MtResult_t;

MtResult_t MyThreadCreate(MyThread_t *thread, void *(*start_routine)(void *),
                          void *arg);

MyThread_t MyThreadSelf(void);

MtResult_t MyThreadJoin(MyThread_t *thread);
MtResult_t MyThreadDetach(MyThread_t *thread);

MtResult_t MyThreadCancel(MyThread_t *thread);
void MyThreadTestCancel(void);

extern _Thread_local char __MyThread_error_descr[512];

static inline const char *MyThreadErrorDescr(void)
{
    return __MyThread_error_descr;
}

#endif /* LIBMYTHREAD_H */


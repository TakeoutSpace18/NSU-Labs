#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <time.h>

void get_mono_time(struct timespec *t);
int64_t msecs_elapsed(const struct timespec *s, const struct timespec *e);
int64_t msecs_elapsed_till_now(const struct timespec *s);

#endif /* TIMER_H */

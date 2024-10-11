#include "timer.h"

#include <assert.h>
#include <errno.h>
#include <string.h>

#include "log.h"

int64_t
msecs_elapsed(const struct timespec *s, const struct timespec *e)
{
	int64_t sec, nsec;

	sec = e->tv_sec - s->tv_sec;
	nsec = e->tv_nsec - s->tv_nsec;
	if (nsec < 0) {
		sec--;
		nsec += 1000ULL * 1000 * 1000;
	}
	assert(0 <= nsec && nsec < 1000ULL * 1000 * 1000);

	return sec * 1000 + nsec / (1000 * 1000);
}


int64_t
msecs_elapsed_till_now(const struct timespec *s)
{
    struct timespec now;
    get_mono_time(&now);

    return msecs_elapsed(s, &now);
}

void
get_mono_time(struct timespec *t)
{
    if (clock_gettime(CLOCK_MONOTONIC, t) == -1) {
        log_error("clock_gettime(): %s", strerror(errno));
        assert(0);
    }
}



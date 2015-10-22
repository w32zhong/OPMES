#include <stdio.h>
#include <stdint.h>
#include "common.h"

struct cp_timer {
	struct timespec begin, end;
};

void cp_delay(time_t sec, long ms, long us)
{
    struct timespec interval;
    interval.tv_sec = sec;
    interval.tv_nsec = us * 1000 + ms * 1000000;
    clock_nanosleep(CLOCK_MONOTONIC, 0, &interval, NULL);
}

void *cp_timer_new()
{
	return cp_malloc(sizeof(struct cp_timer));
}

void cp_timer_free(void *timer)
{
	cp_free(timer);
}

void cp_timer_reset(void *timer)
{
	P_CAST(t, struct cp_timer, timer);
	clock_gettime(CLOCK_MONOTONIC, &t->begin);
}

float cp_timer_get(void *timer)
{
	float dt;
	P_CAST(t, struct cp_timer, timer);

	clock_gettime(CLOCK_MONOTONIC, &t->end);
	dt = (float)(t->end.tv_sec - t->begin.tv_sec) + 
	     (float)(t->end.tv_nsec - t->begin.tv_nsec) * 1e-9;
	return dt;
}

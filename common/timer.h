#include <time.h>

void *cp_timer_new(void);

void cp_timer_free(void*);

void cp_delay(time_t, long, long);

void cp_timer_reset(void *);

float cp_timer_get(void *);

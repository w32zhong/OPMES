#include "common.h"

int main()
{
	float dt, dt2;
	void *timer = cp_timer_new();
	char input[1024];

	cp_timer_reset(timer);
	cp_delay(1, 200, 3000);
	dt = cp_timer_get(timer);

	printf("time: %f s\n", dt);

	cp_timer_reset(timer);
	cp_delay(0, 200, 8000);
	dt = cp_timer_get(timer);

	cp_delay(0, 790, 2000);
	dt2 = cp_timer_get(timer);
	
	printf("time: %f s\n", dt);
	printf("time: %f s\n", dt2);

	printf("please input...\n");
	cp_timer_reset(timer);
	scanf("%s", input);
	dt = cp_timer_get(timer);
	printf("time: %f s\n", dt);

	cp_timer_free(timer);
	return 0;
}

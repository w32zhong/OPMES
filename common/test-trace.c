#include "common.h"
#include <stdlib.h>
#include <inttypes.h>

int main()
{
	void *ptr = NULL;
	trace_init("test-trace.log");

	trace(TEST, "Hello world!\n", 0);
	trace(TEST, "%" PRIu64 " is a big number\n", 
	      999999999999999);

	ptr = cp_malloc(32);
	cp_free(ptr);
	ptr = cp_malloc(32);
	free(ptr);

	trace_unfree();
	trace_uninit();

	return 0;
}

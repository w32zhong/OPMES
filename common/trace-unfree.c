#include <stdlib.h>
#include <string.h>
#include "common.h"

static int32_t unfree_cnt = 0;

void *cp_malloc(size_t sz)
{
	unfree_cnt ++;
	return malloc(sz);
}

void *cp_calloc(size_t nmemb, size_t sz)
{
	unfree_cnt ++;
	return calloc(nmemb, sz);
}

char *cp_strdup(const char* str)
{
	unfree_cnt ++;
	return strdup(str);
}

void cp_free(void* ptr)
{
	unfree_cnt --;
	free(ptr);
}

int32_t trace_unfree()
{
	if (unfree_cnt) 
		trace(UNFREE, "unfree memory count: %d.\n", unfree_cnt);
	return unfree_cnt;
}

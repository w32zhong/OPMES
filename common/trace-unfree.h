#include <stddef.h>

void *cp_malloc(size_t);

void *cp_calloc(size_t, size_t);

char *cp_strdup(const char*);

void cp_free(void*);

int32_t trace_unfree();

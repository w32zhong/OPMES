#include <stdio.h>
#include <stdarg.h>
#include "common.h"

#if TRACE_ENABLE_LOG
static FILE *log_fh = NULL; 
#endif

FILE *trace_log_fh()
{
	return log_fh;
}

void trace_init(char *fname)
{
#if TRACE_ENABLE && TRACE_ENABLE_LOG
	log_fh = fopen(fname, "a");
#endif
}

void _tr_err(char *fmt, ...) 
{
	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
}

void _tr_log(char *fmt, ...) 
{
	va_list args;
	if (log_fh == NULL) {
		printf("log used without init.\n");
		return;
	}
	va_start(args, fmt);
	vfprintf(log_fh, fmt, args);
	fflush(log_fh);
	va_end(args);
}

void trace_uninit()
{
#if TRACE_ENABLE && TRACE_ENABLE_LOG
	if (log_fh)
		fclose(log_fh);
#endif
}

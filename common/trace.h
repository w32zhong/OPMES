#if TRACE_ENABLE
#if (0 == TRACE_ENABLE_LOG)
	#define trace(_name, _fmt, ...) \
	do { \
		if (TRACE_ENABLE_STDERR) \
			if (TRACE_STDERR_ ## _name || TRACE_STDERR_ALL) \
				_tr_err(#_name "\t" _fmt, __VA_ARGS__); \
	} while(0)
#else
	#define trace(_name, _fmt, ...) \
	do { \
		if (TRACE_ENABLE_STDERR) \
			if (TRACE_STDERR_ ## _name || TRACE_STDERR_ALL) \
				_tr_err(#_name "\t" _fmt, __VA_ARGS__); \
		if (TRACE_ENABLE_LOG) \
			if (TRACE_LOG_ ## _name || TRACE_LOG_ALL) { \
				_tr_log(#_name "\t" _fmt, __VA_ARGS__); \
			} \
	} while(0)
#endif
#else
	#define trace(_name, _fmt, _args)
#endif

#include <stdio.h>

FILE *trace_log_fh(void);

void trace_init(char*);

void _tr_log(char*, ...);

void _tr_err(char*, ...);

void trace_uninit();

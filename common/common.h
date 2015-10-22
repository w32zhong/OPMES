/* common type */
#include <stdint.h>
#include <inttypes.h>

#define CP_ID     uint32_t
#define CP_ID_SZ  sizeof(CP_ID)
#define CP_SCORE  int32_t
#define CP_NBRW   uint32_t

#ifndef NULL
#define NULL ((void*)0)
#endif

#ifndef BOOL
#define BOOL uint8_t
#endif

#ifndef uchar 
#define uchar uint8_t
#endif

/* max and min inline function, avoid double-evaluation of macro */
static __inline int max(int a, int b)
{
	return (a>b)?a:b;
}

static __inline int min(int a, int b)
{
	return (a<b)?a:b;
}

/* handy ASCII/VT100 color control sequences */
#define C_RST     "\033[0m"
#define C_RED     "\033[1m\033[31m"      /* Bold Red */
#define C_GREEN   "\033[1m\033[32m"      /* Bold Green */
#define C_BLUE    "\033[1m\033[34m"      /* Bold Blue */
#define C_MAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define C_CYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define C_GRAY    "\033[1m\033[30m"      /* Bold Gray */

/* included files */
#include "config.h"
#include "list.h"
#include "tree.h"
#include "trace.h"
#include "trace-unfree.h"
#include "timer.h"

/* POSIX related declarations */
#include <stdio.h>
int fileno(FILE *);

char *strdup(const char *s);

/* fatal exit */
#include <stdlib.h>
#define CP_FATAL \
	do { trace(FATAL, "%s @ line %d.\n", __FILE__, __LINE__); \
	exit(1); } while (0)

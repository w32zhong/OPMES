#include "search.h"
#include <unistd.h> /* access() */

#define BRWBLK_UNIT_SZ sizeof(struct posting_item)
#define BRWBLK_RD_BUF_SZ (BRWBLK_RD_NUM * BRWBLK_UNIT_SZ) 

uint64_t n_posmerge_scanned_items = 0;

char (*pos_merge_arg(char *pos_fname, 
                     char (*dirs)[MAX_SUBPATH_DIR_NAME],
                     uint32_t n))
[MAX_SUBPATH_DIR_NAME]
{
	uint32_t i;
	char (*ret)[MAX_SUBPATH_DIR_NAME];

	ret = cp_calloc(n, MAX_SUBPATH_DIR_NAME);

	for (i = 0; i < n; i++)
		sprintf(ret[i], "%s/%s", dirs[i], pos_fname);

	return ret;
}

void posmerge_print_rd_num()
{
	printf("BRWBLK_RD_NUM = %d\n", BRWBLK_RD_NUM);
}

static void 
posmerge_print_buf(struct posting_item* buff, FILE *fh,
                   uint32_t idx, uint32_t end) 
{
	uint32_t j;
	struct posting_item *item;
	for (j = 0; j < BRWBLK_RD_NUM; j++) {
		item = buff + j;
		if (j == idx)
			fprintf(fh, "[%d]", item->id); /* current pos */
		else if (j >= end)
			fprintf(fh, "<%d>", item->id); /* old numbers */
		else
			fprintf(fh, "(%d)", item->id); /* loaded data */
	}
	fprintf(fh, "\n");
}

static size_t rebuf(void *buf, FILE *fh,
                    uint32_t *idx, uint32_t *end)
{
	size_t nread;
	nread = fread(buf, BRWBLK_UNIT_SZ, BRWBLK_RD_NUM, fh);
	*idx = 0;
	*end = nread;
	n_posmerge_scanned_items += BRWBLK_RD_NUM;

	return nread;
}

/* return whether possible to be at the position of target ID */
static BOOL 
jump(CP_ID target, struct posting_item *buf, FILE *fh,
     uint32_t *idx, uint32_t *end)
{
	uint32_t idx_far;
	do {
		idx_far = *end - 1;
		
		if (*end == 0)
			return 0;
		else if (buf[idx_far].id >= target)
			break;
		else if (*end != BRWBLK_RD_NUM)
			return 0;

		rebuf(buf, fh, idx, end);
	} while (1);

	/* go ahead and try to find the target ID */
	while (*idx < *end && buf[*idx].id < target)
		(*idx) ++;

	return 1;
}

BOOL posmerge(char (*path)[MAX_SUBPATH_DIR_NAME], uint32_t n,
              pm_callbk_fun callbk_fun, void *callbk_arg)
{
	struct    posting_item (*buf)[BRWBLK_RD_NUM];
	uint32_t *idx, *end;
	BOOL      diff, ret = 0, go_on = 1;
	uint32_t  i;
#if DEBUG_POS_MERGE
	uint32_t  j;
#endif
	FILE    **fh;
	CP_ID     id, max;

	/* open posting files in each directory */
	fh = cp_calloc(n, sizeof(FILE*));

	for (i = 0; i < n; i++) {
		fh[i] = fopen(path[i], "r");
		if (fh[i] == NULL) {
			trace(FATAL, "open error: %s\n", path[i]);
			goto exit_1;
		}
	}
	
	/* allocate progress variables */
	buf = cp_malloc(n * BRWBLK_RD_BUF_SZ);
	idx = cp_malloc(n * sizeof(uint32_t));
	end = cp_malloc(n * sizeof(uint32_t));

	/* initial read */
	for (i = 0; i < n; i++)
		rebuf(&buf[i], fh[i], &idx[i], &end[i]);

	do {
		/* look at the current pointers, see if 
		 * they are of different ID and find out
		 * their max */
		max = buf[0][idx[0]].id;
		diff = 0;
		for (i = 1; i < n; i++) {
			id = buf[i][idx[i]].id;
			if (id != max) {
				if (id > max) 
					max = id;
				diff = 1;
			}
		}

#if DEBUG_POS_MERGE
		printf("max=%d, diff=%d\n", max, diff);
		for (j = 0; j < n; j++)
			posmerge_print_buf(buf[j], stdout, idx[j], end[j]);
		sleep(1);
#endif

		if (diff) {
			/* different ID, go ahead for pipes 
			 * that are lower IDs */
			for (i = 0; i < n; i++) {
				id = buf[i][idx[i]].id;
				if (id != max) {
					if (0 == jump(max, buf[i], fh[i], 
					             &idx[i], &end[i])) {
#if DEBUG_POS_MERGE
		printf(C_RED "jump to exit." C_RST "\n");
#endif
						goto exit_0;
					}
				}
			}

#if DEBUG_POS_MERGE
		printf(C_RED "after jump:" C_RST "\n");
		for (j = 0; j < n; j++)
			posmerge_print_buf(buf[j], stdout, idx[j], end[j]);
#endif

		} else {
			/* identical IDs, which is max. */
#if DEBUG_POS_MERGE
			printf(C_RED "before calling." C_RST "\n");
#endif

			/* same ID, consider it as matched */
			for (i = 0; i < n; i++) {
				/* now, go through all the identical IDs */
				while (buf[i][idx[i]].id == max) {
					(*callbk_fun)(i, max, &buf[i][idx[i]].brw, 
							callbk_arg);
#if DEBUG_POS_MERGE
					printf(C_BLUE "[%d] @ pipe %d calls" C_RST "\n", 
							buf[i][idx[i]].id, i);
					printf("identical ID = %d.\n", max);
					for (j = 0; j < n; j++)
						posmerge_print_buf(buf[j], stdout, idx[j], end[j]);
#endif

					idx[i] ++;
					if (idx[i] == end[i]) {
						if (end[i] == BRWBLK_RD_NUM) {
							rebuf(&buf[i], fh[i], &idx[i], &end[i]);
#if DEBUG_POS_MERGE
							printf("rebuf...\n");
							for (j = 0; j < n; j++)
								posmerge_print_buf(buf[j], stdout, idx[j], end[j]);
#endif
						} else {
							go_on = 0;
							break;
						}
					}
				}
			}

#if DEBUG_POS_MERGE
			printf(C_RED "end calling." C_RST "\n");
#endif
			if (1 == (*callbk_fun)(0, max, NULL, callbk_arg)) {
#if DEBUG_POS_MERGE
				printf("max ranking items (%d) reached.\n", 
				       MAX_RANKING_ITEMS);
#endif
				ret = 1;
				break;
			}
		}
	} while (go_on);

exit_0:
	/* free progress variables */
	cp_free(end);
	cp_free(idx);
	cp_free(buf);

exit_1:
	/* close posting files */
	for (i = 0; i < n; i++) {
		if (fh[i] != NULL)
			fclose(fh[i]);
	}

	cp_free(fh);

	return ret;
}

BOOL posmerge_possible(const char *headname, 
                       char (*dirs)[MAX_SUBPATH_DIR_NAME], uint32_t n)
{
	char               (*paths)[MAX_SUBPATH_DIR_NAME];
	CP_ID                maxmin, minmax;
	struct posting_head  hd;
	BOOL                 ret = 1;
	FILE               **fh;
	uint32_t             i;
	
	paths = pos_merge_arg((char*)headname, dirs, n);

	/* check head file availability */
	for (i = 0; i < n; i++)
		if (access(paths[i], F_OK) == -1
			/* file does not exist */)
			break;
	
	if (i == n /* every search directory has head file */) {
		/* prepare to read head.bin */
		fh = cp_calloc(n, sizeof(FILE*));

		maxmin = 0;
		minmax = MAX_CP_ID;
		for (i = 0; i < n; i++) {
			fh[i] = fopen(paths[i], "r");
			if (fh[i] == NULL) {
				trace(FATAL, "unexpected open error: %s\n", 
				      paths[i]);
				goto exit;
			}

			fread(&hd, sizeof(struct posting_head), 1, fh[i]);
			if (hd.max < minmax)
				minmax = hd.max;
			if (hd.min > maxmin)
				maxmin = hd.min;
		}

		/* return zero if not possible to merge */
		if (minmax < maxmin) {
#if DEBUG_HEADPRUNE_PRINT
			fprintf(stderr, "not possible posmerge dirs:\n");
			dirs_print(paths, n, stderr);
#endif
			ret = 0;
		}
exit:
		/* close posting files */
		for (i = 0; i < n; i++)
			if (fh[i] != NULL)
				fclose(fh[i]);
		cp_free(fh);
	}

	cp_free(paths);
	return ret;
}

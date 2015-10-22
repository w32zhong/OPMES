#include <stdio.h>
#include <time.h>

#if DEBUG_POS_MERGE
#include <unistd.h> /* for sleep() */
#endif

void posmerge_print_rd_num();

typedef BOOL (*pm_callbk_fun)(uint32_t, CP_ID, 
                              struct brw*, void*);

BOOL posmerge(char (*)[MAX_SUBPATH_DIR_NAME], uint32_t,
              pm_callbk_fun, void*);

char (*pos_merge_arg(char *, 
                     char (*)[MAX_SUBPATH_DIR_NAME], 
                     uint32_t))[MAX_SUBPATH_DIR_NAME];

extern uint64_t n_posmerge_scanned_items;

BOOL posmerge_possible(const char *, 
                       char (*)[MAX_SUBPATH_DIR_NAME], uint32_t);

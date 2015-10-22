int dir_find(DIR*, const char*);
BOOL dir_exists(DIR*, const char*);

char (*dir_merge_arg(char *, struct rlv_stack *, uint32_t))
[MAX_SUBPATH_DIR_NAME];

void dirs_print(char (*)[MAX_SUBPATH_DIR_NAME], uint32_t, FILE*);

typedef BOOL (*dm_callbk_fun)(char (*)[MAX_SUBPATH_DIR_NAME], 
                           uint32_t, uint32_t, void *);

BOOL dir_merge(char (*)[MAX_SUBPATH_DIR_NAME], uint32_t, uint32_t,
               dm_callbk_fun, void *);

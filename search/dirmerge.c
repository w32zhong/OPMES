#include "search.h"

BOOL dir_exists(DIR *parent_dir, const char *dname)
{
	struct dirent *dent;
	rewinddir(parent_dir);

	while (1) {
		dent = readdir(parent_dir);
		if (!dent)
			break;

		if (dent->d_type & DT_DIR) {
			if (strcmp(dname, "..") == 0 ||
			    strcmp(dname, ".") == 0)
				continue;

			if (0 == strcmp(dname, dent->d_name)) {
				return 1;
			}
		}
	}

	return 0;
}
	
char (*dir_merge_arg(char *prefix, struct rlv_stack *rlv_array, 
                     uint32_t rlv_array_len))[MAX_SUBPATH_DIR_NAME]
{
	uint32_t i;
	char (*ret)[MAX_SUBPATH_DIR_NAME];

	ret = cp_calloc(rlv_array_len, MAX_SUBPATH_DIR_NAME);

	for (i = 0; i < rlv_array_len; i++)
		sprintf(ret[i], "%s/%s", prefix, rlv_array[i].dir);

	return ret;
}

void dirs_print(char (*dirs)[MAX_SUBPATH_DIR_NAME], uint32_t n,
                FILE *fh)
{
	uint32_t i;
	for (i = 0; i < n; i++)
		fprintf(fh, "%s\n", dirs[i]);
}

BOOL dir_merge(char (*dirs)[MAX_SUBPATH_DIR_NAME], uint32_t n, 
               uint32_t depth, dm_callbk_fun callbk_fun, 
               void *callbk_arg)
{
	char         (*deeper_dirs)[MAX_SUBPATH_DIR_NAME];
	DIR          **dh = cp_calloc(n, sizeof(DIR*)); 
	BOOL           ret = 0;
	const char    *dname;
	struct dirent *dent;
	uint32_t       i;

	/* directories in argument array should exists. */
	for (i = 0; i < n; i++) {
		dh[i] = opendir(dirs[i]);

		if (NULL == dh[i]) {
			trace(MERGE_SEARCH, "dir open failed: %s\n", dirs[i]);
			goto exit;
		}
	}

	/* call the callback function */
	if (1 == (*callbk_fun)(dirs, n, depth, callbk_arg)) {
		trace(MERGE_SEARCH, "dirmerege stops.\n", NULL);
		ret = 1;
		goto exit;
	}

	/* now search the merge directories in this depth */
	while (1) {
		dent = readdir(dh[0]);
		if (!dent)
			break;
		
		dname = dent->d_name;
		if (dent->d_type & DT_DIR) {
			if (strcmp(dname, "..") == 0 ||
			    strcmp(dname, ".") == 0)
				continue;
			    
			/* see if this dirname also exists in 
			 * another directory */
			for (i = 1; i < n; i++) {
				if (!dir_exists(dh[i], dname)) 
					break;
			}

			/* no break, means every directory has
			 * this dirname */
			if (i == n) {
				/* let's search recursively */
				deeper_dirs = cp_calloc(n, MAX_SUBPATH_DIR_NAME);

				for (i = 0; i < n; i++)
					sprintf(deeper_dirs[i], "%s/%s", 
					        dirs[i], dname);

				if (0 != dir_merge(deeper_dirs, n, depth + 1, 
				                   callbk_fun, callbk_arg)) {
					/* all dir_merge should stop in this case. */
					cp_free(deeper_dirs);
					ret = 1;
					break;
				}

				cp_free(deeper_dirs);
			}
		}
	}

exit:
	for (i = 0; i < n; i++)
		if (NULL != dh[i])
			closedir(dh[i]);

	cp_free(dh);

	return ret;
}

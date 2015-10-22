#include "search.h"
#include <getopt.h>    /* getopt() */

uint32_t g_cnt = 0;

static BOOL test_callbk(char (*dirs)[MAX_SUBPATH_DIR_NAME], 
                        uint32_t n, uint32_t depth, void *arg)
{
	P_CAST(use_default, BOOL, arg);

	if (*use_default) {
		printf("test @ depth = %d\n", depth);
		dirs_print(dirs, n, stdout);
		printf("\n");
	} else {
		g_cnt ++;
		dirs_print(dirs, n, stdout);
		printf("\n");
	}

	return 0; /* go on searching */
}

int main(int argc, char** argv)
{
	uint32_t dirs, i;
	BOOL use_default = 0;
	char (*dirname)[MAX_SUBPATH_DIR_NAME]; 
	char (*full_dirs)[MAX_SUBPATH_DIR_NAME];
	struct rlv_stack *rlv_array;
	int opt;
	char *col_path = NULL;

	trace_init("test-dirmerge.log");

	while ((opt = getopt(argc, argv, "hp")) != -1) {
		switch (opt) {
		case 'h':
			printf("DESCRIPTION:\n");
			printf("test directory merge.\n");
			printf("\n");
			printf("SYNOPSIS:\n");
			printf("%s -h | -p <path0> <path1> ..."
			       "\n",
			       argv[0]);
			printf("\n");
			printf("EXAMPLE:\n");
			printf("%s -p 'VAR/TIMES/ADD' 'ONE/ADD'\n", argv[0]);
			printf("\n");
			goto exit;

		case 'p':
			dirs = argc - 2;
			dirname = cp_malloc(MAX_SUBPATH_DIR_NAME * dirs);
			for (i = 2; i < argc; i++)
				strcpy(dirname[i - 2], argv[i]);

			col_path = "./col";
			goto skip_default;

		default:
			break;
		}
	}

	dirs = 3;
	dirname = cp_malloc(MAX_SUBPATH_DIR_NAME * dirs);
	strcpy(dirname[0], "./dir0");
	strcpy(dirname[1], "./dir1");
	strcpy(dirname[2], "./dir2");
	col_path = "./test-dir";
	use_default = 1;

skip_default:
	if (NULL == col_path) {
		printf("col_path unset.\n");
		goto exit;
	}

	rlv_array = cp_malloc(dirs * sizeof(struct rlv_stack));

	printf("search from paths in %s:\n", col_path);
	for (i = 0; i < dirs; i++) {
		printf("[%s]\n", dirname[i]);
		rlv_array[i].dir = dirname[i];
	}
	printf("\n");

	full_dirs = dir_merge_arg(col_path, rlv_array, dirs);
	dir_merge(full_dirs, dirs, 0, &test_callbk, &use_default);

	if (!use_default)
		printf("total merged paths: %u\n", g_cnt);

	cp_free(full_dirs);
	cp_free(rlv_array);
	cp_free(dirname);
	
exit:
	trace_unfree();
	trace_uninit();
	return 0;
}

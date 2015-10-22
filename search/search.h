#include <stdio.h>
#include <string.h>

#include "index.h"
#include "score.h"
#include "minheap.h"
#include "rank.h"

#include <dirent.h>
#include <string.h>
#include "dirmerge.h"
#include "posmerge.h"

void query_paths_name_sort(struct list_it*);

void query_paths_set_idx(struct list_it*);

struct merge_node {
	struct tex_tr   *nd;
	struct list_node ln;
	tree_depth_t     depth;
};

struct list_it merge_nodes(struct tex_tr*);

LIST_DECL_FREE_FUN(merge_nodes_free);

void merge_nodes_print(struct list_it*, FILE*);

enum {
	CP_SE_RET_GOOD,
	CP_SE_RET_COL_UNSET,
	CP_SE_RET_COL_MISSING,
	CP_SE_RET_DB_MISSING,
	CP_SE_RET_PARSER_FAILS,
	CP_SE_RET_EX_LIMIT
};

struct se_options {
	char       *col_path;
	uint32_t    n_ranking_items;
	BOOL        en_merges_nd_se;
	BOOL        en_headprune;
	BOOL        en_thorough_search;
};

struct rank cp_search(const char*, struct se_options*, int*);

void search_print_options(struct se_options*, FILE*);
void search_options_set_default(struct se_options*);

extern uint32_t n_search_merge_nodes;
extern uint64_t n_search_scanned_items;

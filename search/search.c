#include <inttypes.h>  /* PRIu64 */ 
#include <unistd.h>    /* access() */
#include "search.h"

struct merge_search_arg {
	struct searcher    *se;
	struct score_tr    *st;
	struct score_cache *cache;
	struct rank         rank;
	uint32_t            se_depth;  /* search depth */
	uint32_t            rot_depth; /* tex tree rot depth */
	BOOL                en_headprune;
	BOOL                en_thorough_search;
};

struct merge_node_search_arg {
	struct merge_search_arg  *msa;
	char     *query; 
	char     *col_path; 
	BOOL      ret_err;
	BOOL      en_merges_nd_se; /* enable merge node search */
};

struct _get_name_cnt_arg {
	uint32_t       cnt;
	enum symbol_id symbol_id;
};

uint32_t n_search_merge_nodes;
uint64_t n_search_scanned_items;

static const char default_col_path[] = DEFAULT_COL_PATH; 

static LIST_IT_CALLBK(_get_name_cnt)
{
	LIST_OBJ(struct query_path, p, ln);
	P_CAST(gnca, struct _get_name_cnt_arg, pa_extra);

	if (gnca->symbol_id == p->brw.symbol_id)
		gnca->cnt += p->brw.fan[0];

	LIST_GO_OVER;
}

static LIST_IT_CALLBK(_set_idx)
{
	LIST_OBJ(struct query_path, p, ln);
	P_CAST(i, uint32_t, pa_extra);

	p->idx = (*i) ++;

	LIST_GO_OVER;
}

static LIST_IT_CALLBK(_set_name_cnt)
{
	struct list_it li;
	struct _get_name_cnt_arg gnca;
	LIST_OBJ(struct query_path, p, ln);

	/* go through entire list */
	li = list_get_it(pa_head->now);
	gnca.cnt = 0;
	gnca.symbol_id = p->brw.symbol_id;
	list_foreach(&li, &_get_name_cnt, &gnca);

	p->_same_nm_cnt = gnca.cnt;

	LIST_GO_OVER;
}

static LIST_CMP_CALLBK(_name_sort_cmp)
{
	struct query_path *p0;
	struct query_path *p1;
	
	p0 = MEMBER_2_STRUCT(pa_node0, struct query_path, ln);
	p1 = MEMBER_2_STRUCT(pa_node1, struct query_path, ln);
	
	if (p0->_same_nm_cnt == p1->_same_nm_cnt)
		/* alphabet order */
		return p0->brw.symbol_id < p1->brw.symbol_id;
	else
		return p0->_same_nm_cnt > p1->_same_nm_cnt;
}

void query_paths_name_sort(struct list_it *li)
{
	struct list_sort_arg lsa;
	list_foreach(li, &_set_name_cnt, NULL);

	lsa.cmp = &_name_sort_cmp;
	list_sort(li, &lsa);
}

void query_paths_set_idx(struct list_it *li)
{
	uint32_t i = 0;
	list_foreach(li, &_set_idx, &i);
}

static TREE_IT_CALLBK(search_merge_nodes)
{
	TREE_OBJ(struct tex_tr, p, tnd);
	P_CAST(ret_li, struct list_it, pa_extra);
	struct merge_node *mn;
	BOOL insert = 0;

	if (p->tnd.father == NULL) {
		/* tex tree root may only have one child,
		 * but we still need to add it into ret_li. */
		insert = 1;

	} else if (p->tnd.sons.now && 
	           p->tnd.sons.now != p->tnd.sons.last
	          /* if it has son and at least two sons */) {
		insert = 1;
	}

	if (insert) {
		mn = cp_malloc(sizeof(struct merge_node));
		LIST_NODE_CONS(mn->ln);
		mn->nd = p;
		mn->depth = pa_depth;

		list_insert_one_at_tail(&mn->ln, ret_li, NULL, NULL);
	}

	LIST_GO_OVER;
}

struct list_it merge_nodes(struct tex_tr *root)
{
	struct list_it ret_li = LIST_NULL;
	tree_foreach(&root->tnd, &tree_pre_order_DFS, 
	             &search_merge_nodes, 0, &ret_li);

	return ret_li;
}

static LIST_IT_CALLBK(_merge_nodes_print)
{
	LIST_OBJ(struct merge_node, mn, ln);
	P_CAST(fh, FILE, pa_extra);
	
	if (pa_now->now == pa_head->last)
		fprintf(fh, "#%d.\n", mn->nd->node_id);
	else
		fprintf(fh, "#%d, ", mn->nd->node_id);
	
	LIST_GO_OVER;
}

void merge_nodes_print(struct list_it *li, FILE *fh)
{
	list_foreach(li, &_merge_nodes_print, fh);
}

LIST_DEF_FREE_FUN(merge_nodes_free, struct merge_node, ln,
                  cp_free(p));

static 
BOOL merge_pos_callbk(uint32_t pipe, CP_ID id, 
                      struct brw *brw, void* args)
{
	CP_SCORE score;
	P_CAST(msa, struct merge_search_arg, args);

	if (brw != NULL /* regular call */) {
#if DEBUG_SCORE_BRW
		printf(C_CYAN "ID=%d @ %s " C_RST "\n", id, 
		       msa->se->rlv_array[pipe].dir);
		printf("{ ");
		print_brw(brw, stdout);
		printf(" }");
		printf("\n");
#endif
	
#if !CONFIG_NO_SCORE_PROCESS
		score_push(msa->st, msa->se, msa->cache, pipe, brw, 
		           msa->se_depth);
#endif
	} else /* ending call */ {
#if !CONFIG_NO_SCORE_PROCESS
		score = score_calc(msa->st, msa->se, msa->cache, id);
#else
		score = 1;
#endif

		if (score != 0)
			rank_filter(&msa->rank, id, score, msa->rot_depth);

#if !CONFIG_NO_SCORE_PROCESS
		score_tr_clear(msa->st);
		searcher_clear(msa->se);
#endif
	}

	return 0;
}

static 
BOOL merge_dir_callbk(char (*dirs)[MAX_SUBPATH_DIR_NAME], 
                      uint32_t n, uint32_t depth, void *arg)
{
	char   (*paths)[MAX_SUBPATH_DIR_NAME];
	uint32_t i;
	BOOL     res = 0;
	P_CAST(msa, struct merge_search_arg, arg);

	if (!msa->en_thorough_search && 
	    n_posmerge_scanned_items > MAX_SEARCH_ITEMS) {
		fprintf(stderr, "search items exceeds maximum.\n");
		return 1; /* exist search */
	}

	if (msa->en_headprune && 
	    !posmerge_possible("head.bin", dirs, n))
		goto skip;

	paths = pos_merge_arg("posting.bin", dirs, n);

	/* check posting file availability */
	for (i = 0; i < n; i++)
		if (access(paths[i], F_OK) == -1
			/* file does not exist */)
			break;

	if (i == n) {
		/* every search directory has posting file */
		msa->se_depth = depth;
		res = posmerge(paths, n, merge_pos_callbk, arg);
	}

	cp_free(paths);

skip:
	return res;
}

BOOL merge_search(char *col_path, struct merge_search_arg *msa)
{
	BOOL ret;
	char (*dirs)[MAX_SUBPATH_DIR_NAME];

	/* reset scanned items counter */
	n_posmerge_scanned_items = 0;

	dirs = dir_merge_arg(col_path, msa->se->rlv_array, 
	                     msa->se->rlv_array_len); 
	ret = dir_merge(dirs, msa->se->rlv_array_len, 0,
	                &merge_dir_callbk, msa);
	cp_free(dirs);

	return ret;
}

static LIST_IT_CALLBK(merge_node_search)
{
	int subpath_err;
	struct list_it li_subpath;
	LIST_OBJ(struct merge_node, mn, ln);
	P_CAST(mnsa, struct merge_node_search_arg, pa_extra);
	BOOL list_break;
	void *timer;

	/* timer set */
	timer = cp_timer_new();
	cp_timer_reset(timer);

	/* NULL to indicate exit code whether to free the resource */
	li_subpath.now = NULL;
	mnsa->msa->se->li_query_path.now = NULL;
	list_break = 0;
	
	/* print current merge node */
	fprintf(stderr, "query formula: %s\n", mnsa->query);
	tex_tr_print(parser_root, stderr);

	fprintf(stderr, C_RED "current merge node: #%d (rot depth=%d)." 
	        C_RST "\n", mn->nd->node_id, mn->depth);

	/* get subpaths from current merge point node */
	li_subpath = tex_tr_subpaths(mn->nd, &subpath_err);

	if (subpath_err) {
		fprintf(stderr, "subpaths generation error.\n");
		list_break = 1;
		mnsa->ret_err = 1;
		goto exit;
	}
	
	/* transfer subpath into searcher and free subpaths */
	searcher_cons(mnsa->msa->se, &li_subpath);
	subpaths_free(&li_subpath);

	/* sort query path and set index in order */
	query_paths_name_sort(&mnsa->msa->se->li_query_path);
	query_paths_set_idx(&mnsa->msa->se->li_query_path);

	fprintf(stderr, "sorted query paths:\n");
	query_paths_print(&mnsa->msa->se->li_query_path, stderr);

	/* setup score tree */
	score_tr_setup(mnsa->msa->st, mnsa->msa->se->n_qpath);

	/* begin to search */
	fprintf(stderr, "begin to search...\n");

	mnsa->msa->rot_depth = mn->depth;
	if (0 == merge_search(mnsa->col_path, mnsa->msa))
		fprintf(stderr, "searched thoroughly.\n");

	fprintf(stderr, "%" PRIu64 " items scanned.\n", 
			n_posmerge_scanned_items);

	/* update global variables */
	n_search_merge_nodes ++;
	n_search_scanned_items += n_posmerge_scanned_items;
	
	/* print status of rank list */
	fprintf(stderr, "rank list: %d/%d.\n", 
	        mnsa->msa->rank.n_items,
	        mnsa->msa->rank.n_capacity);

	/* test if we go on to the next merge node */
	if (mnsa->msa->rank.n_items >= STOP_MERG_ND_RK_ITEMS ||
	    /* go on only if merge node search is enabled */
	    !mnsa->en_merges_nd_se) {
		fprintf(stderr, "stop going to the next merge node.\n");
		list_break = 1;
	}
	
exit:
	/* subpath list free */
	if (li_subpath.now)
		subpaths_free(&li_subpath);

	/* searcher reset */
	if (mnsa->msa->se->li_query_path.now)
		searcher_empty(mnsa->msa->se);

	/* report time cost and free timer */
	trace(SEARCH_TIME, "merge node #%d time cost: %f sec.\n", 
	      mn->nd->node_id, cp_timer_get(timer));
	cp_timer_free(timer);

	/* print trailing linefeed */
	fprintf(stderr, "\n");

	if (list_break)
		return LIST_RET_BREAK;
	else
		LIST_GO_OVER;
}

void search_options_set_default(struct se_options *opt)
{
	opt->col_path           = DEFAULT_COL_PATH;
	opt->n_ranking_items    = DEFAULT_RANKING_ITEMS;
	opt->en_merges_nd_se    = DEFAULT_EN_MERGES_ND_SE;
	opt->en_headprune       = DEFAULT_EN_HEADPRUNE;
	opt->en_thorough_search = DEFAULT_EN_THOROUGH_SE;
}

void search_print_options(struct se_options *opt, FILE *fh)
{
	fprintf(fh, "         collection path: %s\n", 
	        (NULL == opt->col_path) ? "NULL" : opt->col_path);
	fprintf(fh, "           ranking items: %u\n", 
	        opt->n_ranking_items);
	fprintf(fh, "enable merge node search: %d\n", 
	        opt->en_merges_nd_se);
	fprintf(fh, " enable head.bin pruning: %d\n", 
	        opt->en_headprune);
	fprintf(fh, "  enable thorough search: %d\n", 
	        opt->en_thorough_search);
}

struct rank cp_search(const char *query, 
                      struct se_options *se_opt, 
                      int *ret_code)
{
	struct merge_node_search_arg mnsa;
	struct list_it li_merge_nodes;
	struct merge_search_arg msa;
	void                   *timer;

	/* set initial return code */
	*ret_code = CP_SE_RET_GOOD;

	/* timer set */
	timer = cp_timer_new();
	cp_timer_reset(timer);

	/* rank list should be set-up first to make sure it is always 
	 * good to use for user after cp_search() returns. */
	/* ranking set init */
	if (!rank_init(&msa.rank, se_opt->n_ranking_items)) {
		fprintf(stderr, "key-value DB is not found.\n");
		rank_free(&msa.rank);
	
		*ret_code = CP_SE_RET_DB_MISSING;
		goto exit;
	}

	/* check if collection path is set */
	if (NULL == se_opt->col_path) {
		fprintf(stderr, "collection directory is unset.\n");
		*ret_code = CP_SE_RET_COL_UNSET;
		goto exit;
	}

	/* check if collection path exists */
	if (access(se_opt->col_path, F_OK) == -1) {
		fprintf(stderr, "collection directory is not found.\n");
		*ret_code = CP_SE_RET_COL_MISSING;
		goto exit;
	}

	/* set parser_root to NULL initially */
	/* or "go to exit" might free a random address. */
	parser_root = NULL;

	/* parse formula string */
	if (!cp_parse((char*)query)) {
		*ret_code = CP_SE_RET_PARSER_FAILS;
		goto exit;
	}
	
	if (parser_err_flg) {
		fprintf(stderr, "parser: %s\n", parser_err_msg);

		parser_err_flg = 0;
		*ret_code = CP_SE_RET_PARSER_FAILS;
		goto exit;
	}

	if (NULL == parser_root) {
		parser_err_flg = 0;
		*ret_code = CP_SE_RET_PARSER_FAILS;
		goto exit;
	}

	/* update tex tree and generate subpath list */
	if (0 == tex_tr_update(parser_root)) {
		fprintf(stderr, "too many tex tree leaves.\n");
		*ret_code = CP_SE_RET_EX_LIMIT;
		goto exit;
	}

	/* generate merge nodes list */
	li_merge_nodes = merge_nodes(parser_root);

	/* searcher allocation */
	msa.se = searcher_new();
	/* score cache allocation */
	msa.cache = cp_malloc(sizeof(struct score_cache));
	/* score tree allocation */
	msa.st = score_tr_new();

	/* other search arguments */
	mnsa.msa = &msa;
	mnsa.ret_err = 0;
	mnsa.query = (char*)query;
	mnsa.col_path = se_opt->col_path;
	mnsa.en_merges_nd_se = se_opt->en_merges_nd_se;
	msa.en_thorough_search = se_opt->en_thorough_search;
	msa.en_headprune = se_opt->en_headprune;

	/* reset global info counters */
	n_search_merge_nodes = 0;
	n_search_scanned_items = 0;

	/* search every merge node */
	list_foreach(&li_merge_nodes, &merge_node_search, &mnsa);

	/* update DB records and rank results */
	rank_sort(&msa.rank);

	/* return limit error if occurred */
	if (mnsa.ret_err)
		*ret_code = CP_SE_RET_EX_LIMIT;
	
	/* free allocated search stuff */
	cp_free(msa.cache);
	cp_free(msa.st);
	cp_free(msa.se);

	/* free merge nodes list */
	merge_nodes_free(&li_merge_nodes);

	/* report API total time cost */
	trace(SEARCH_TIME, "cp_search() total time cost: %f sec.\n", 
	      cp_timer_get(timer));
exit:
	/* releases textree */
	if (parser_root) {
		tex_tr_release(parser_root);
		parser_root = NULL;
	}
	
	/* timer free */
	if (timer)
		cp_timer_free(timer);

	return msa.rank;
}

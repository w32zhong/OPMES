#include "search.h"

int main()
{
	struct list_it li_subpath = LIST_NULL;
	struct subpath *p;
	struct searcher *se;
	struct score_cache *cache;
	struct score_tr *st; 

	struct brw brw;
	brw.pin[1] = 0;
	brw.fan[0] = 1;
	brw.fan[1] = 0;
	
	printf("size=%ld\n", sizeof(struct score_tr) + 
	       sizeof(struct searcher) + sizeof(struct score_cache));
	trace_init("test-score.log");

	p = cp_malloc(sizeof(struct subpath));
	strcpy(p->dir, "a dir");
	p->brw.symbol_id = S_Alpha;
	p->brw.pin[0] = 2;
	p->brw.fan[0] = 1;
	p->brw.pin[1] = 0;
	p->brw.fan[1] = 0;
	LIST_NODE_CONS(p->ln);
	list_insert_one_at_tail(&p->ln, &li_subpath, NULL, NULL);

	p = cp_malloc(sizeof(struct subpath));
	strcpy(p->dir, "b dir");
	p->brw.symbol_id = S_Alpha;
	p->brw.pin[0] = 1;
	p->brw.fan[0] = 1;
	p->brw.pin[1] = 0;
	p->brw.fan[1] = 0;
	LIST_NODE_CONS(p->ln);
	list_insert_one_at_tail(&p->ln, &li_subpath, NULL, NULL);

	p = cp_malloc(sizeof(struct subpath));
	strcpy(p->dir, "a dir");
	p->brw.symbol_id = S_Beta;
	p->brw.pin[0] = 3;
	p->brw.fan[0] = 1;
	p->brw.pin[1] = 0;
	p->brw.fan[1] = 0;
	LIST_NODE_CONS(p->ln);
	list_insert_one_at_tail(&p->ln, &li_subpath, NULL, NULL);

	//subpaths_print(&li_subpath, stdout);

	/* transfer subpath into searcher */
	se = searcher_new();
	
	searcher_cons(se, &li_subpath);
	subpaths_free(&li_subpath);
	
	/* score tree construction */
	st = score_tr_new();
	score_tr_setup(st, se->n_qpath);

	cache = cp_malloc(sizeof(struct score_cache));

	{ /* search loop */
		brw.symbol_id = S_alpha;
		brw.pin[0] = 12;
		score_push(st, se, cache, 0, &brw, 0);

		brw.symbol_id = S_beta;
		brw.pin[0] = 21;
		score_push(st, se, cache, 0, &brw, 0);

		brw.symbol_id = S_alpha;
		brw.pin[0] = 22;
		score_push(st, se, cache, 1, &brw, 0);

		printf(C_MAGENTA "before calc:\n" C_RST);
		score_tr_print(st, stdout);
		searcher_print(se, stdout);

		score_calc(st, se, cache, 0);
		printf(C_CYAN "score = %d\n" C_RST, st->score);

		score_tr_clear(st);
		searcher_clear(se);

		printf(C_MAGENTA "after clear:\n" C_RST);
		score_tr_print(st, stdout);
		searcher_print(se, stdout);

/* redundance */
		brw.symbol_id = S_alpha;
		brw.pin[0] = 1;
		score_push(st, se, cache, 0, &brw, 0);

		brw.symbol_id = S_beta;
		brw.pin[0] = 2;
		score_push(st, se, cache, 1, &brw, 0);

		brw.symbol_id = S_alpha;
		brw.pin[0] = 3;
		score_push(st, se, cache, 0, &brw, 0);

		brw.symbol_id = S_beta;
		brw.pin[0] = 4;
		score_push(st, se, cache, 0, &brw, 0);

		brw.symbol_id = S_alpha;
		brw.pin[0] = 5;
		score_push(st, se, cache, 1, &brw, 0);

		printf(C_MAGENTA "before calc:\n" C_RST);
		score_tr_print(st, stdout);
		searcher_print(se, stdout);

		score_calc(st, se, cache, 0);
		printf(C_CYAN "score = %d\n" C_RST, st->score);

		score_tr_clear(st);
		searcher_clear(se);

		printf(C_MAGENTA "after clear:\n" C_RST);
		score_tr_print(st, stdout);
		searcher_print(se, stdout);
	}

	cp_free(cache);
	cp_free(st);

	searcher_empty(se);
	cp_free(se);

	trace_unfree();
	trace_uninit();
	return 0;
}

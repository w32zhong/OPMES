#include "search.h"

static void *db_formula = NULL;
static void *db_textree = NULL;

void _rank_heap_print(void* ele, uint32_t i, uint32_t depth)
{
	uint32_t j;
	P_CAST(rk_item, struct rank_item, ele);

	for (j = 0; j < depth; j++)
		printf("  ");

	printf("%-4d ", rk_item->score);
}

void rank_heap_print_arr(struct rank *rank)
{
	heap_print_arr(&rank->heap, &_rank_heap_print);
}

void rank_heap_print_tr(struct rank *rank)
{
	heap_print_tr(&rank->heap, &_rank_heap_print);
}

static BOOL less_than_stage0(void* ele0, void* ele1)
{
	P_CAST(i0, struct rank_item, ele0);
	P_CAST(i1, struct rank_item, ele1);

	/* ranked by a tuple of two values */
	if (i0->rot_depth != i1->rot_depth)
		return i0->rot_depth > i1->rot_depth;
	else
		return i0->score < i1->score;
}

static BOOL less_than_stage1(void* ele0, void* ele1)
{
	P_CAST(i0, struct rank_item, ele0);
	P_CAST(i1, struct rank_item, ele1);

	/* ranked by a tuple of three values */
	if (i0->rot_depth != i1->rot_depth)
		return i0->rot_depth > i1->rot_depth;
	else if (i0->score != i1->score)
		return i0->score < i1->score;
	else
		return i0->frml_rec.n_brw > i1->frml_rec.n_brw;
}

BOOL rank_init(struct rank* rank, uint32_t n_capacity)
{
	/* set structure members */
	rank->heap = heap_create(n_capacity);
	heap_set_callbk(&rank->heap, &less_than_stage0);

	LIST_CONS(rank->keep);

	rank->n_items = 0;
	rank->n_capacity = n_capacity;
	
	/* open key-value database files */
	if (db_formula == NULL)
		db_formula = db_init(CONFIG_FORMULA_DB_NAME, DB_OMOD_RD);

	if (db_textree == NULL)
		db_textree = db_init(CONFIG_TEXTREE_DB_NAME, DB_OMOD_RD);

	if (db_formula == NULL)
	/* db_formula is mandatory. */
		return 0;
	else
		return 1;
}

LIST_DEF_FREE_FUN(rank_li_free, struct rank_item, ln, 
	if (p->frml_rec.frml_str)
		cp_free(p->frml_rec.frml_str); 
	if (p->frml_rec.url_str)
		cp_free(p->frml_rec.url_str); 
	cp_free(p));
 
void rank_free(struct rank *rank)
{
	/* release structure members */
	heap_destory(&rank->heap);
	rank_li_free(&rank->keep);
	rank->n_items = 0;
	rank->n_capacity = 0;
	
	/* release database */
	if (db_formula)
		db_release(db_formula);

	if (db_textree)
		db_release(db_textree);
}

static 
struct rank_item *rank_li_push(struct list_it *keep, CP_ID id, 
                               CP_SCORE score, uint32_t rot_depth)
{
	struct rank_item *rk_item;
	rk_item = cp_malloc(sizeof(struct rank_item));

	LIST_NODE_CONS(rk_item->ln);
	rk_item->id        = id;
	rk_item->score     = score;
	rk_item->rot_depth = rot_depth;

	list_insert_one_at_tail(&rk_item->ln, keep, NULL, NULL);
	return rk_item;
}

void rank_filter(struct rank *rank, CP_ID id,
                 CP_SCORE score, uint32_t rot_depth)
{
	struct rank_item *save;

	if (!heap_full(&rank->heap)) {
		/* insert until it is full */
		save = rank_li_push(&rank->keep, id, score, rot_depth);
		minheap_insert(&rank->heap, save);
		
		rank->n_items ++; /* increment ranking items */
	} else {
		/* rank is full now, replace the lowest score. */
		save = (struct rank_item*)heap_top(&rank->heap);

		if (save->score < score
		    /* do not compare rot_depth in this case, 
			 * because rot_depth of the heap top must 
			 * be the same when rank is full. */) {
			list_detach_one(&save->ln, &rank->keep, NULL, NULL);
			cp_free(save);

			save = rank_li_push(&rank->keep, id, score, rot_depth);
			minheap_replace(&rank->heap, 0, save);
		}
	}
}

void rank_item_print(struct rank_item *rk_item, FILE *fh)
{
	fprintf(fh, "ranked by tuple "
	            "(rot depth=%d, score=%d, #brw=%d).\n",
	        rk_item->rot_depth, rk_item->score,
	        rk_item->frml_rec.n_brw);
	
	fprintf(fh, "formula (ID = %d):\n", rk_item->id);

	if (rk_item->frml_rec.frml_str)
		fprintf(fh, "%s\n", rk_item->frml_rec.frml_str);

	if (rk_item->frml_rec.url_str)
		fprintf(fh, "url: %s\n", rk_item->frml_rec.url_str);
	
	if (db_textree)
		probe_print_frml_textree(db_textree, rk_item->id);
}

void rank_print(struct rank *rank, FILE *fh)
{
	uint32_t i;
	struct rank_item *rk_item;
	struct heap *h = &rank->heap;

	fprintf(stderr, C_GREEN "ranking list (%d items):" C_RST "\n", 
	       rank->n_items);

	for (i = 0; i < rank->n_items; i++) {
		rk_item = (struct rank_item*)h->array[i];
		if (rk_item) {
			fprintf(fh, C_GREEN "#%d " C_RST, i + 1);
			rank_item_print(rk_item, fh);
		}
	}
}

static BOOL set_frml_rec(struct heap *h)
{
	uint32_t i;
	BOOL ret = 0;
	struct rank_item *rk_item;

	for (i = 0; i < h->end; i++) {
		rk_item = (struct rank_item*)h->array[i];
	
		ret = db_get_formula_record(db_formula, rk_item->id, 
	                                &rk_item->frml_rec);
		if (0 != ret) {
			trace(RANK_ERR, "formula record not found.\n", NULL);
			break;
		}
	}

	return ret;
}

void rank_sort(struct rank* rank)
{
	void *timer;
#if DEBUG_HEAP_WHEN_SORT 
	fprintf(stderr, "heap before sort:\n");
	rank_heap_print_arr(rank);
	rank_heap_print_tr(rank);
#endif
	/* timer set */
	timer = cp_timer_new();
	cp_timer_reset(timer);

	/* complete heap nodes' ranking info (i.e. #brw) */
	set_frml_rec(&rank->heap);

	/* use new ranking function to consider #brw */
	heap_set_callbk(&rank->heap, &less_than_stage1);
	
	/* report time cost */
	trace(SEARCH_TIME, "get formula records from DB: %f sec.\n", 
	      cp_timer_get(timer));

	/* timer set again */
	cp_timer_reset(timer);

	/* final sort */
	heap_sort_desc(&rank->heap);
	
	/* time cost report and free timer */
	trace(SEARCH_TIME, "final sort time cost: %f sec.\n", 
	      cp_timer_get(timer));
	cp_timer_free(timer);
	
#if DEBUG_HEAP_WHEN_SORT 
	fprintf(stderr, "heap after sort:\n");
	rank_heap_print_arr(rank);
	rank_heap_print_tr(rank);
#endif
}

static __inline 
uint32_t div_ceil(uint32_t a, uint32_t b)
{
	uint32_t ret = a / b;
	if (a % b)
		return ret + 1;
	else
		return ret;
}

struct rank_wind rank_window_calc(struct rank *rank,
                                  uint32_t page_start, 
                                  uint32_t res_per_page,
                                  uint32_t *out_tot_page)
{
	struct rank_wind win = {rank, 0, 0};
	*out_tot_page = 0;

	if (res_per_page == 0) 
		return win;

	*out_tot_page = div_ceil(rank->n_items, res_per_page);

	if (page_start >= *out_tot_page)
		return win;
	
	win.from = page_start * res_per_page;
	win.to   = (page_start + 1) * res_per_page;

	if (win.to > rank->n_items)
		win.to = rank->n_items;

	return win;
}

BOOL rank_window_foreach(struct rank_wind *win, 
                         rank_item_callbk fun, void *arg)
{
	uint32_t i, cnt = 0;
	struct heap *h = &win->rank->heap;

	for (i = win->from; i < win->to; i++) {
		(*fun)((struct rank_item*)h->array[i], cnt, arg);
		cnt ++;
	}

	return cnt;
}

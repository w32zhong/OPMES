#include <stdio.h>
#include <string.h>

#include "common.h"
#include "parser.h"
#include "trans.h"
#include "score.h"

LIST_DEF_FREE_FUN(query_paths_free, struct query_path, ln,
                  cp_free(p));

static void cpy_series(uint8_t *dest, uint8_t *src)
{
	uint32_t i = 0;
	while (src[i] != 0) {
		dest[i] = src[i];
		i++;
	}
}

static LIST_IT_CALLBK(_search_cons)
{
	LIST_OBJ(struct subpath, p, ln);
	P_CAST(se, struct searcher, pa_extra);
	uint32_t i;
	struct rlv_stack *found;
	struct rlv_stack_ref_entry *ref_ent;

	struct query_path *qp = cp_malloc(sizeof(struct query_path));

	/* copy directory name */
	strcpy(qp->dir, p->dir);

	/* copy branch word */
	qp->brw.symbol_id = p->brw.symbol_id;
	cpy_series(qp->brw.pin, p->brw.pin);
	cpy_series(qp->brw.fan, p->brw.fan);

	/* insert path name into directory set */
	found = NULL;
	for (i = 0; i < se->rlv_array_len; i++) {
		if (0 == strcmp(se->rlv_array[i].dir, qp->dir)) {
			found = se->rlv_array + i;
			break;
		}
	}

	if (!found) {
		se->rlv_array[i].dir = qp->dir;
		se->rlv_stack_items[i] = 0;
		LIST_CONS(se->rlv_array[i].li_ref);
		se->rlv_array_len ++;

		qp->stk = se->rlv_array + i;
	} else {
		qp->stk = found;
	}

	qp->rlv_stack_idx = i;

	/* add reference for stack */
	ref_ent = cp_malloc(sizeof(struct rlv_stack_ref_entry));
	ref_ent->qp = qp;

	LIST_NODE_CONS(ref_ent->ln);
	list_insert_one_at_tail(&ref_ent->ln, &qp->stk->li_ref, 
	                        NULL, NULL);

	/* insert into query path list */
	LIST_NODE_CONS(qp->ln);
	list_insert_one_at_tail(&qp->ln, &se->li_query_path,
	                        NULL, NULL);

	qp->idx = se->n_qpath ++;
	LIST_GO_OVER;
}

struct searcher *searcher_new()
{
	struct searcher *se;

	se = cp_malloc(sizeof(struct searcher));

	LIST_CONS(se->li_query_path);
	se->rlv_array_len = 0;
	se->n_qpath = 0;
	
	return se;
}

void searcher_cons(struct searcher *se, struct list_it *li_subpath)
{
	se->rlv_array_len = 0;
	se->n_qpath = 0;

	list_foreach(li_subpath, &_search_cons, se);
}

LIST_DEF_FREE_FUN(li_ref_free, struct rlv_stack_ref_entry, ln, 
                  cp_free(p));

void searcher_empty(struct searcher *se)
{
	uint32_t i;
	query_paths_free(&se->li_query_path);

	for (i = 0; i < se->rlv_array_len; i++)
		li_ref_free(&se->rlv_array[i].li_ref);
	
	se->rlv_array_len = 0;
}

static void print_query_path(struct query_path *qp, FILE *fh)
{
	fprintf(fh, "%dth: ", qp->idx);

	fprintf(fh, "{ ");
	print_brw(&qp->brw, fh);
	fprintf(fh, " }");
}

static LIST_IT_CALLBK(_print_query_path)
{
	LIST_OBJ(struct query_path, qp, ln);
	P_CAST(fh, FILE, pa_extra);

	print_query_path(qp, fh);
	if (qp->stk)
		fprintf(fh, " -> %s", qp->stk->dir);
	else
		fprintf(fh, " -> nil");
	
	fprintf(fh, " (%dth)\n", qp->rlv_stack_idx);

	LIST_GO_OVER;
}

static LIST_IT_CALLBK(_print_query_path_brw_only)
{
	LIST_OBJ(struct query_path, qp, ln);
	P_CAST(fh, FILE, pa_extra);

	print_query_path(qp, fh);
	if (qp->stk)
		fprintf(fh, " -> %s", qp->stk->dir);
	else
		fprintf(fh, " -> nil");
	fprintf(fh, "\n");

	LIST_GO_OVER;
}

static LIST_IT_CALLBK(_print_ref_entry)
{
	LIST_OBJ(struct rlv_stack_ref_entry, ent, ln);
	P_CAST(fh, FILE, pa_extra);

	print_query_path(ent->qp, fh);
	fprintf(fh, "\n");

	LIST_GO_OVER;
}

void query_paths_print(struct list_it *li_query_path, FILE *fh)
{
	list_foreach(li_query_path, &_print_query_path_brw_only, fh);
}

void searcher_print(struct searcher *se, FILE *fh)
{
	uint32_t i, j, n_items;
	struct rlv_stack *stk;
	
	fprintf(fh, "query path list (n_qpath=%d):\n", se->n_qpath);
	list_foreach(&se->li_query_path, &_print_query_path, fh);
	
	fprintf(fh, "relevance stacks:\n");
	for (i = 0; i < se->rlv_array_len; i++) {
		stk = se->rlv_array + i;
		n_items = se->rlv_stack_items[i];
		fprintf(fh, "<%s> ", stk->dir);

		fprintf(fh, "[");
		for (j = 0; j < n_items; j++) {
			fprintf(fh, "#%d (%dth var)", stk->ck_ptr[j]->id,
			        stk->var_idx[j]);

			if (j != n_items - 1)
				fprintf(fh, ", ");
		}
		fprintf(fh, "]");
		fprintf(fh, " refs:\n");

		list_foreach(&stk->li_ref, &_print_ref_entry, fh);
	}
}

void searcher_clear(struct searcher *se)
{
	memset(se->rlv_stack_items, 0, se->rlv_array_len * 
	       sizeof(uint32_t));
}

void score_tr_clear(struct score_tr *st)
{
	memset(st->check_board, 0, MAX_TEX_TR_VARS * st->n_qpath *
	       sizeof(struct check_unit));

	st->n_var = 0;
	st->score = 0;
}

void score_tr_setup(struct score_tr *st, uint32_t n_qpath)
{
	st->n_qpath = n_qpath;
}

struct score_tr *score_tr_new()
{
	struct score_tr *st;
	st = cp_calloc(1, sizeof(struct score_tr));
	/* 
	 * st-> 
	 *  n_var, n_qpath, score, max, who
	 *  and champ_board should be zero.
	 */

	return st;
}

void score_tr_insert(struct score_tr *st, struct brw *brw, 
                     struct check_unit **ck, uint32_t *var_idx)
{
	uint32_t i;
	struct score_var *var;

	for (i = 0; i < st->n_var; i++)
		if (st->var[i].symbol_id == brw->symbol_id)
			break;

	var = st->var + i;
	*var_idx = i;

	if (i == st->n_var) {
		st->var_score[i] = 0;
		var->n_ck_unit = 0;
		var->symbol_id = brw->symbol_id;

		st->n_var ++;
	}

	*ck = &st->check_board[var->n_ck_unit ++][i];
	(*ck)->ck_mark = CK_UNMARKED;
	(*ck)->id = brw->pin[0];
}

static void print_ck_mark(enum check_mark ck_mark, FILE *fh)
{
	switch (ck_mark) {
	case CK_UNMARKED:
		fprintf(fh, "unmarked");
		break;
	case CK_MARKED:
		fprintf(fh, "marked");
		break;
	case CK_CROSSED:
		fprintf(fh, "crossed");
		break;
	}
}

struct cache_put_arg {
	struct score_cache *ca;
	struct brw         *brw;
	uint32_t            depth;
};

static BOOL fan_constrain(uint8_t *fan_q, uint8_t *fan_d)
{
	uint32_t i = 1;
	while (fan_q[i] != 0 && fan_d[i] != 0) {
		if (fan_q[i] > fan_d[i])
			return 1;
		i++;
	}

	return 0;
}

static CP_SCORE score(struct brw *brw_q, struct brw *brw_d, 
                      uint32_t depth)
{
	CP_SCORE s; /* base score */

	if (fan_constrain(brw_q->fan, brw_d->fan))
		return 0;

	if (brw_q->symbol_id == brw_d->symbol_id)
		s = 10;
	else
		s = 9;
	
	s /= 1 + depth;

	s = s * min(brw_q->fan[0], brw_d->fan[0]);
	return s;
}

static LIST_IT_CALLBK(_cache_put)
{
	LIST_OBJ(struct rlv_stack_ref_entry, ref, ln);
	P_CAST(cpa, struct cache_put_arg, pa_extra);

	struct brw *b0 = &ref->qp->brw, *b1 = cpa->brw;

	cpa->ca->s[ref->qp->idx][b1->pin[0]] = 
	score(b0, b1, cpa->depth);
	
	LIST_GO_OVER;
}

void score_cache_put(struct score_cache *ca, struct rlv_stack *stk,
                     struct brw *brw, uint32_t cur_depth)
{
	struct cache_put_arg cpa = {ca, brw, cur_depth};
	list_foreach(&stk->li_ref, &_cache_put, &cpa);
}

void score_tr_print(struct score_tr *st, FILE *fh)
{
	uint32_t i, j;
	struct score_var *v;
	struct check_unit *ck;
	struct champ_unit *chmp;
	fprintf(fh, "root: max=%d, score=%d", st->max, st->score);
	if (st->who)
		fprintf(fh, ", who->%s", trans_symbol(st->who->symbol_id));
	fprintf(fh, ".\n");

	for (i = 0; i < st->n_var; i++) {
		v = st->var + i;
		fprintf(fh, "\t%dth var: %s (score=%d)\n", i, 
		        trans_symbol(v->symbol_id), st->var_score[i]);

		fprintf(fh, "\tcheck units:\n");
		for (j = 0; j < v->n_ck_unit; j++) {
			ck = &st->check_board[j][i];
			fprintf(fh, "\t\t"); print_ck_mark(ck->ck_mark, fh);
			fprintf(fh, ", brwID=%d\n", ck->id);
		}

		fprintf(fh, "\tchamp units:\n");
		for (j = 0; j < st->n_qpath; j++) {
			chmp = &st->champ_board[j][i];
			fprintf(fh, "\t\t"); 
			fprintf(fh, "max=%d", chmp->max);
			if (chmp->who)
				fprintf(fh, ", who->%d", chmp->who->id);
			fprintf(fh, "\n");
		}
	}
}

static BOOL next_stage(struct query_path *cur_qp, 
                       struct list_it *pa_head, 
                       struct list_it *pa_now)
{
	struct query_path *next_qp;

	if (pa_now->now == pa_head->last) {
		return 1;
	} else {
		next_qp = MEMBER_2_STRUCT(pa_now->now->next, 
				struct query_path, ln);
		if (cur_qp->brw.symbol_id != next_qp->brw.symbol_id)
			return 1;
	}

	return 0;
}

struct score_main_arg {
	struct score_tr    *st;
	struct score_cache *cache;
	struct searcher    *se; /* for printing */
	uint32_t            stage_idx;
};

static LIST_IT_CALLBK(_score_main)
{
	LIST_OBJ(struct query_path, qp, ln);
	P_CAST(sma, struct score_main_arg, pa_extra);
	struct champ_unit *champs, *champ;
	enum   check_mark  new_mark;
	struct check_unit *ck;
	struct score_tr   *st;
	struct score_var  *v;
	CP_SCORE score, *vs;
	uint32_t i, j, n_items;
	BOOL test, match;

	champs = sma->st->champ_board[qp->idx];
	st = sma->st;
	n_items = sma->se->rlv_stack_items[qp->rlv_stack_idx];

	match = 0;
	for (i = 0; i < n_items; i++) {
		ck = qp->stk->ck_ptr[i];
		j = qp->stk->var_idx[i];
		champ = champs + j;

		if (ck->ck_mark == CK_UNMARKED) {
			score = sma->cache->s[qp->idx][ck->id];
			match = 1;

			if (score > champ->max) {
				if (champ->who)
					champ->who->ck_mark = CK_UNMARKED;
				champ->max = score;
				champ->who = ck;
				ck->ck_mark = CK_MARKED;
			}
		}
	}

#if (DEBUG_SCORING)
	/* print current evaluating query brw */
	printf(C_MAGENTA);
	printf("for ");
	print_query_path(qp, stdout);
	printf("\n");
	
	printf("(relavant: ");
	if (qp->stk) {
		printf("%s ", qp->stk->dir);
		printf("[");
		for (i = 0; i < n_items; i++) {
			ck = qp->stk->ck_ptr[i];
			printf("#%d", ck->id);

			if (i != n_items - 1) 
				printf(", ");
		}
		printf("]");
	}
	printf(")\n" C_RST);
#endif

	if (match == 0) {
		st->score = 0;
		st->max = 0;
		st->who = NULL;
		memset(st->champ_board, 0, 
		       MAX_TEX_TR_VARS * st->n_qpath *
		       sizeof(struct champ_unit));
#if (DEBUG_SCORING)
		printf(C_RED "Exhausted all candidates." C_RST "\n");
#endif
		return LIST_RET_BREAK;
	}

	for (i = 0; i < st->n_var; i++) {
		v = st->var + i;
		vs = st->var_score + i;
		*vs += champs[i].max;

		if (*vs > st->max) {
			st->max = *vs;
			st->who = v;
		}
	}

#if (DEBUG_SCORING)
	/* print current tree */
	score_tr_print(sma->st, stdout);
#endif

	if (next_stage(qp, pa_head, pa_now)) {
		for (i = 0; i < st->n_var; i++) {
			test = (st->who == st->var + i);
			new_mark = test ? CK_CROSSED : CK_UNMARKED;

			for (j = sma->stage_idx; j <= qp->idx; j++) {
				champ = &st->champ_board[j][i];
				if (champ->who) {
					champ->who->ck_mark = new_mark;
					champ->who = NULL;
					champ->max = 0; /* debug */
				}
			}
		}

		memset(st->var_score, 0, st->n_var * sizeof(CP_SCORE));

		st->score += st->max;
		st->max = 0;
		st->who = NULL;

		sma->stage_idx = qp->idx;

#if (DEBUG_SCORING)
		printf(C_MAGENTA "end of this stage (variable %s):\n" C_RST,
		       trans_symbol(qp->brw.symbol_id));
		score_tr_print(sma->st, stdout);
#endif
	}

	LIST_GO_OVER;
}

CP_SCORE score_calc(struct score_tr* st, struct searcher *se,
                    struct score_cache *cache, 
                    CP_ID id /* debug only */)
{
	struct score_main_arg sma = {st, cache, se, 0};

#if (DEBUG_SCORING)
		printf(C_CYAN "docID = %d, calc begin...\n" C_RST, id);
		score_tr_print(st, stdout);
		searcher_print(se, stdout);
#endif

	list_foreach(&se->li_query_path, &_score_main, &sma);

#if (DEBUG_SCORING)
		printf(C_RED "score = %d" C_RST "\n", st->score);
#endif

	return st->score;
}

void score_push(struct score_tr *st, struct searcher *se,
                struct score_cache *cache, uint32_t call_idx, 
                struct brw *brw, uint32_t cur_depth)
{
	struct check_unit  *ck;
	uint32_t idx, *n_items;
	struct rlv_stack  *stk;

	/* insert into score tree */
	score_tr_insert(st, brw, &ck, &idx);

	/* get stack from calling index */
	stk = se->rlv_array + call_idx; 
	n_items = se->rlv_stack_items + call_idx;

	/* push score tree link */
	stk->ck_ptr[*n_items] = ck;
	stk->var_idx[*n_items] = idx;
	(*n_items) ++;

	/* put score into cache */
	score_cache_put(cache, stk, brw, cur_depth);
}

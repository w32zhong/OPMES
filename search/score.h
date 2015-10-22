/* score tree */
enum check_mark {
	CK_UNMARKED,
	CK_MARKED,
	CK_CROSSED,
};

struct check_unit {
	enum check_mark ck_mark;
	uint8_t         id;
};

struct champ_unit {
	CP_SCORE           max;
	struct check_unit *who;
};

struct score_var {
	uint32_t n_ck_unit;
	uint16_t symbol_id;
};

struct score_tr {
	uint32_t n_var, n_qpath /* number of query path */;

	/* max score */
	CP_SCORE           score, max;
	struct score_var  *who;

	/* tree structure */
	struct score_var  var[MAX_TEX_TR_VARS];
	CP_SCORE          var_score[MAX_TEX_TR_VARS];

	struct champ_unit
	champ_board[MAX_TEX_TR_LEAVES][MAX_TEX_TR_VARS];

	struct check_unit
	check_board[MAX_TEX_TR_LEAVES][MAX_TEX_TR_VARS];
};

/* score cache */
struct score_cache {
	CP_SCORE s[MAX_TEX_TR_LEAVES][MAX_TEX_TR_LEAVES];
};

/* searcher */
struct rlv_stack;

struct query_path {
	char              dir[MAX_SUBPATH_DIR_NAME];
	struct brw        brw;
	struct list_node  ln;
	uint32_t          idx; /* index in sorted query list */
	struct rlv_stack *stk; /* relevance stack */
	uint32_t          rlv_stack_idx; 
	uint32_t          _same_nm_cnt; /* for sorting */
};

struct rlv_stack_ref_entry {
	struct query_path *qp;
	struct list_node   ln;
};

struct rlv_stack {
	char              *dir;
	struct list_it     li_ref; /* reference */

	struct check_unit *ck_ptr[MAX_TEX_TR_LEAVES]; 
	uint32_t           var_idx[MAX_TEX_TR_LEAVES];
};

struct searcher {
	uint32_t          n_qpath;
	struct list_it    li_query_path;
	uint32_t          rlv_array_len;
	uint32_t          rlv_stack_items[MAX_TEX_TR_LEAVES];
	struct rlv_stack  rlv_array[MAX_TEX_TR_LEAVES];
};

/* searcher */
struct searcher *searcher_new();

void searcher_cons(struct searcher*, struct list_it*);

void searcher_print(struct searcher*, FILE*);

void searcher_clear(struct searcher*);

void searcher_empty(struct searcher*);

/* score tree */
struct score_tr *score_tr_new();

void score_tr_setup(struct score_tr*, uint32_t);

void score_tr_insert(struct score_tr*, struct brw*, 
                     struct check_unit **, uint32_t*);

void score_tr_print(struct score_tr*, FILE*);

void score_tr_clear(struct score_tr*);

/* score cache */
void score_cache_put(struct score_cache*, struct rlv_stack*, 
                     struct brw*, uint32_t);

/* score */
void score_push(struct score_tr*, struct searcher*, 
                struct score_cache*, uint32_t, 
                struct brw*, uint32_t);

CP_SCORE score_calc(struct score_tr*, struct searcher*, 
                    struct score_cache*, CP_ID);

/* utilities */
void query_paths_print(struct list_it*, FILE*);

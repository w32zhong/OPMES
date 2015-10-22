struct rank {
	struct heap      heap;
	struct list_it   keep;
	uint32_t         n_items;
	uint32_t         n_capacity;
};

struct rank_item {
	struct list_node ln;
	CP_ID            id;
	CP_SCORE         score;
	uint32_t         rot_depth;
	struct frml_rec  frml_rec;
};

BOOL rank_init(struct rank*, uint32_t);
void rank_free(struct rank*);
void rank_filter(struct rank*, CP_ID, CP_SCORE, uint32_t);
void rank_item_print(struct rank_item*, FILE*);
void rank_print(struct rank*, FILE*);
void rank_sort(struct rank*);
void rank_heap_print_tr(struct rank*);
void rank_heap_print_arr(struct rank*);

/* ranking window */
struct rank_wind {
	struct rank *rank;
	uint32_t from, to;
};

struct rank_wind
rank_window_calc(struct rank*, uint32_t, uint32_t, uint32_t*);

typedef void (*rank_item_callbk)(struct rank_item*,
                                 uint32_t, void*);
BOOL 
rank_window_foreach(struct rank_wind*, rank_item_callbk, void*);

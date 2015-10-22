#include <getopt.h>    /* getopt() */
#include "search.h"

static void print_page_item(struct rank_item *rk_item, 
                            uint32_t cnt, void *arg) 
{
	P_CAST(fh, FILE, arg);
	fprintf(fh, C_GREEN "(%d) " C_RST, cnt + 1);
	rank_item_print(rk_item, fh);
}

static
uint32_t print_page(struct rank *rank, uint32_t page,
                    uint32_t res_per_page, FILE* fh)
{
	struct rank_wind  win;
	uint32_t          tot_page;

	win = rank_window_calc(rank, page, res_per_page, &tot_page);

	fprintf(fh, "==========");
	fprintf(fh, C_BLUE "result page %d/%d:" C_RST, 
	        page + 1, tot_page);
	fprintf(fh, "==========\n");

	if (0 == rank_window_foreach(&win, &print_page_item, fh))
		fprintf(stderr, "no results at page %d.\n", page);

	return tot_page;
}

static void print_all_pages(struct rank *rank, 
                            uint32_t res_per_page, FILE* fh)
{
	uint32_t total, page = 0;
	do {
		total = print_page(rank, page, res_per_page, fh);
		page ++;
	} while (page < total);
}

int main(int argc, char* argv[])
{
	void             *timer = cp_timer_new();
	char             *query = NULL;
	struct rank       rank;
	int               opt, se_ret;
	struct se_options se_opt;
	char              col_path[MAX_COL_PATH_NAME_LEN];
	uint32_t          res_per_page = DEFAULT_RES_PER_PAGE;
	uint32_t          res_start_page = 0;
	BOOL              print_rank_li = 0;
	BOOL              print_all_in_res_page = 0;

	/* init trace/log */
	trace_init("search.log");

	/* start program stopwatch */
	cp_timer_reset(timer);
	
	/* set parser_root to NULL initially */
	/* or "go to exit" might free a random address. */
	parser_root = NULL;

	/* set default search options */
	search_options_set_default(&se_opt);	
	
	/* handle program arguments */
	while ((opt = getopt(argc, argv, "htnmabq:c:r:s:p:")) != -1) {
		switch (opt) {
		case 'h':
			printf("DESCRIPTION:\n");
			printf("search query formula in a collection.\n");
			printf("\n");
			printf("SYNOPSIS:\n");
			printf("%s -h | "
			       "-q <query formula> "
			       "[-c <collection path prefix>] "
			       "[-t (toggle thorough search)] "
			       "[-n (toggle merge nodes search)] "
			       "[-m (toggle head.bin pruning)] "
			       "[-r <number of ranking items>] "
			       "[-a (print rank list)] "
			       "[-b (print all in result pages)] "
			       "[-s <results starting page>] "
			       "[-p <number of results per page>] "
			       "\n",
			       argv[0]);
			printf("\n");
			printf("EXAMPLE:\n");
			printf("%s -q '\\frac a b' \n", 
			       argv[0]);
			printf("\n");
			goto exit;

		case 'q':
			query = cp_strdup(optarg);
			break;
		
		case 'c':
			strcpy(col_path, optarg);
			se_opt.col_path = col_path;
			break;

		case 'r':
			{
				uint32_t tmp;
				sscanf(optarg, "%u", &tmp);
				if (tmp == 0) tmp = 1;
				se_opt.n_ranking_items = tmp;
				break;
			}
		
		case 's':
			{
				uint32_t tmp;
				sscanf(optarg, "%u", &tmp);
				res_start_page = tmp;
				break;
			}
		
		case 'p':
			{
				uint32_t tmp;
				sscanf(optarg, "%u", &tmp);
				res_per_page = tmp;
				printf("set to %u result(s) per page.\n", tmp);
				break;
			}
		
		case 't':
			se_opt.en_thorough_search = !se_opt.en_thorough_search;
			break;

		case 'n':
			se_opt.en_merges_nd_se = !se_opt.en_merges_nd_se;
			break;
		
		case 'm':
			se_opt.en_headprune = !se_opt.en_headprune;
			break;
		
		case 'a':
			print_rank_li = 1;
			break;
		
		case 'b':
			print_all_in_res_page = 1;
			break;

		default:
			fprintf(stderr, "bad argument(s).\n");
			goto exit;
		}
	}

	/* check validity of required program arguments */
	if (NULL == query) {
		fprintf(stderr, "please input a query.\n");
		goto exit;
	}
	
	/* print search options */
	fprintf(stderr, "search options:\n");
	search_print_options(&se_opt, stderr);
	fprintf(stderr, "\n");

	/* search for query in collection */
	rank = cp_search(query, &se_opt, &se_ret);

	/* print search report */
	fprintf(stderr, "%" PRIu64 " items scanned in %d time(s).\n", 
			n_search_scanned_items, n_search_merge_nodes);

	/* handle search results */
	switch (se_ret) {
	case CP_SE_RET_GOOD:
		if (print_rank_li)
			rank_print(&rank, stderr);
		else
			if (print_all_in_res_page)
				print_all_pages(&rank, res_per_page, stderr);
			else
				print_page(&rank, res_start_page, 
				           res_per_page, stderr);
		break;
	case CP_SE_RET_PARSER_FAILS:
		fprintf(stderr, "LaTeX sytax error.\n");
		break;
	case CP_SE_RET_COL_UNSET:
		fprintf(stderr, "collection path unset.\n");
		break;
	case CP_SE_RET_DB_MISSING:
		fprintf(stderr, "database missing.\n");
		break;
	case CP_SE_RET_COL_MISSING:
		fprintf(stderr, "collection missing.\n");
		break;
	case CP_SE_RET_EX_LIMIT:
		fprintf(stderr, "value(s) exceeding limits.\n");
		break;
	default:
		fprintf(stderr, "unexpected default.\n");
		break;
	}

	/* free ranking list */
	rank_free(&rank);

exit:
	/* free query string */
	if (query)
		cp_free(query);

	/* report time cost and free stopwatch */
	fprintf(stderr, "program runing time: %f sec.\n", 
	        cp_timer_get(timer));
	cp_timer_free(timer);

	/* uninit trace/log */
	trace_unfree();
	trace_uninit();
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <curl/curl.h>
#include "search.h"

#define STR(_num) # _num
#define STR_FMT(_num) "%" STR(_num) "s"

static void cat(char *file)
{
	char path[1024];
	size_t len = 0;
	char *line = NULL;
	FILE *f;
	
	sprintf(path, "cat/%s", file);
	f = fopen(path, "r");

	if (f == NULL)
		return;

	while (getline(&line, &len, f) != -1)
		printf("%s", line);

	free(line);
	fclose(f);
}

static void replace_plus(char *str)
{
	while (str[0] != '\0') {
		if (str[0] == '+')
			str[0] = ' ';
		str ++;
	}
}

static char *q_unescape(char *str)
{
	CURL *curl;
	char *ret;

	curl = curl_easy_init();
	ret = curl_easy_unescape(curl, str, 0, NULL);
	curl_easy_cleanup(curl);

	return ret;
}

static void query_subst(char *query)
{
	uint32_t i;
	query[0] = ' ';
	query[1] = ' ';

	for (i = 0; i < strlen(query); i++) {
		if (query[i] == '\n')
			query[i] = ' ';
	}
}

void echo_hidden_form(char *name, char *query, uint32_t page)
{
	printf("<form action=\"/cgi/search.cgi?s=%u\" "
	       "method=\"post\" name=\"%s\">"
	       "<input type=\"hidden\" name=\"q\" value=\"%s\">"
	       "</form>", page, name, query);
}

void echo_li(char *doc_tex, char *url, char *info)
{
	if (doc_tex && url && info)
	printf("<li><table border=\"0\" class=\"rank_item\">"
	       "<tr><td><a target=\"_blank\" href=\"%s\">"
	       "<h3>%s</h3></a>"
	       "<span style=\"color:green;\">%s</span>"
	       "</td><td>[dmath]%s[/dmath]"
	       "<span style=\"color:white;\">"
	       "%s</span></td></tr></table></li>",
	       url, doc_tex, url, doc_tex, info);
}

static void echo_page_item(struct rank_item *rk_item, 
                           uint32_t cnt, void *arg) 
{
	char info[1024];
	sprintf(info, "rot depth=%d, score=%d, #brw=%d, ID=%d.",
	        rk_item->rot_depth, rk_item->score,
	        rk_item->frml_rec.n_brw, rk_item->id);
	
	echo_li(rk_item->frml_rec.frml_str, rk_item->frml_rec.url_str, 
	        info);
}

static
uint32_t echo_page(struct rank *rank, uint32_t page,
                   uint32_t res_per_page)
{
	struct rank_wind  win;
	uint32_t          tot_page;

	win = rank_window_calc(rank, page, res_per_page, &tot_page);
	rank_window_foreach(&win, &echo_page_item, NULL);

	return tot_page;
}

static void trace_user()
{
	char *env_ip, *env_agent;
	env_ip = getenv("REMOTE_ADDR");	
	env_agent = getenv("HTTP_USER_AGENT");	

	if (env_ip && env_agent)
		trace(WEB, "IP [%s] visted, agent:\n %s\n", 
		      env_ip, env_agent);
}

int main()
{
	char      stdin_input[MAX_WEB_QUERY_STR_LEN + 1];
	char     *env_input;
	char     *query;
	void     *timer;
	uint32_t  start_page;
	uint32_t  total_pages = 0;
	
	struct rank       rank;
	struct se_options se_opt;
	int               se_ret;
	char             *se_err;

	/* CGI initial print */
	printf("Content-type: text/html\n\n");

	/* create trace/log */
	trace_init("web.log");

	/* get POST content from stdin */
	scanf(STR_FMT(MAX_WEB_QUERY_STR_LEN), stdin_input);
	replace_plus(stdin_input);

	/* get GET content from environment variable */ 
	/* e.g. `export QUERY_STRING=s=123' */
	env_input = getenv("QUERY_STRING");
	if (env_input == NULL) {
		trace(WEB, "No env variable: QUERY_STRING.\n", NULL);
		goto exit;
	}
	
	/* unescape to get query */
	query = q_unescape(stdin_input);
	if (query == NULL) {
		trace(WEB, "curl_easy_unescape() fails.\n", NULL);
		goto exit;
	}
	
	/* strip `s=' to get query string */
	query_subst(query);

	/* extract page number from GET content */
	sscanf(env_input, "s=%u", &start_page);

	/* echo HTML */
	cat("head.cat");
	printf("[imath]\\text{query} \\; = \\; "
	       "\\left\\{%s\\right\\}[/imath]", query);
	cat("neck.cat");

	/* set search options */
	search_options_set_default(&se_opt);

	/* print what we have now */
	trace(WEB, "================%c", '\n');
	trace_user();
	trace(WEB, "query: %s\n", query);
	trace(WEB, "start page: %u\n", start_page);
	trace(WEB, "search options:\n", NULL);
	if (trace_log_fh())
		search_print_options(&se_opt, trace_log_fh());

	/* start search stopwatch */
	timer = cp_timer_new();
	cp_timer_reset(timer);

	/* search for query in collection */
	rank = cp_search(query, &se_opt, &se_ret);
	
	/* handle search results */
	se_err = NULL;
	switch (se_ret) {
	case CP_SE_RET_GOOD:
		break;
	case CP_SE_RET_PARSER_FAILS:
		se_err = "LaTeX sytax error.";
		break;
	case CP_SE_RET_COL_UNSET:
		se_err = "collection path unset.";
		break;
	case CP_SE_RET_DB_MISSING:
		se_err = "database missing.";
		break;
	case CP_SE_RET_COL_MISSING:
		se_err = "collection missing.";
		break;
	case CP_SE_RET_EX_LIMIT:
		se_err = "value(s) exceeding limits.";
		break;
	default:
		se_err = "unexpected default.\n";
		break;
	}

	/* echo results */
	if (se_err) {
		printf("<li><h2>%s</h2></li>\n", se_err);
	} else {
		total_pages = echo_page(&rank, start_page, 
		                        DEFAULT_RES_PER_PAGE);
		if (total_pages == 0)
			printf("<li><h2>Nothing found. T_T</h2></li>\n");
	}

	/* echo search report */
	printf("<span style=\"color: #aaaaaa\">" "%" PRIu64
	       " items scanned in %u time(s), take"
	       " roughly %f sec. </span>", 
	       n_search_scanned_items, n_search_merge_nodes,
	       cp_timer_get(timer));

	/* free timer */
	cp_timer_free(timer);

	/* echo HTML */
	cat("ass.cat");

	/* echo navigation links */
	if (total_pages != 0) {
		if (start_page != 0)
			cat("prev.cat");

		printf("<td>page %u / %u</td>\n", 
		       start_page + 1, total_pages);

		if (start_page + 1 < total_pages)
			cat("next.cat");

		echo_hidden_form("prev", query, start_page - 1);
		echo_hidden_form("next", query, start_page + 1);
	}

	/* echo HTML */
	cat("tail.cat");

	/* delete ranking list */
	rank_free(&rank);

exit:
	/* free query string */
	if (query)
		curl_free(query);

	/* delete trace/log */
	trace_unfree();
	trace_uninit();
	return 0;
}

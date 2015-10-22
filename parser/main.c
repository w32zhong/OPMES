#include "common.h"
#include "parser.h"

#include <readline/readline.h>
#include <readline/history.h>

int main(int argc,char *argv[])
{   
	uint32_t       fail_cnt = 0, succ_cnt = 0;
	struct list_it li_subpath;
	char          *line;
	int            subpath_err;

	trace_init("parser.log");	

	/* disable auto-complete */
	rl_bind_key('\t', rl_abort);

	while (1) {
		line = readline(C_CYAN "edit: " C_RST);

		if (line == NULL)
			break;

		/* user can use UP and DOWN key to
		 * search through the history. */
		add_history(line);

		if (!cp_parse(line)) 
			goto next_line;

		if (parser_err_flg) {
			fail_cnt ++;
			parser_err_flg = 0; /* clear flag */
			trace(YYERR, "%s\n", parser_err_msg);

			if (parser_root) {
				/* not supposed to get here */
				trace(YYERR, "unfree root %p.\n", parser_root);
				break;
			}
			
		} else {
			succ_cnt ++;

			if (parser_root) {
				tex_tr_update(parser_root);
				tex_tr_print(parser_root, stdout);
				printf("\n");

				li_subpath = tex_tr_subpaths(parser_root, 
				                             &subpath_err);

				if (subpath_err) {
					printf("subpath generation error.\n");
				} else {
					printf("subpaths:\n");
					subpaths_print(&li_subpath, stdout);
				}

				subpaths_free(&li_subpath);
				tex_tr_release(parser_root);
				parser_root = NULL;
			}
		}

next_line:
		free(line);
		printf("\n");
		trace_unfree();
	}

	trace_uninit();
	printf("\n");
	printf("%d passed, %d errors.\n", succ_cnt, fail_cnt);
	return 0;
}

#include <stdio.h>
#include <string.h>
#include "common.h"
#include "parser.h"
#include "trans.h"

char *mk_scan_buf(char *str, size_t *out_sz)
{
	char *buf; 
	*out_sz = strlen(str) + 3;
	buf = cp_malloc(*out_sz);
	sprintf(buf, "%s\n_", str);
	buf[*out_sz - 2] = '\0';

	return buf;
}

BOOL cp_parse(char *str)
{
	YY_BUFFER_STATE state_buf;
	char *scan_buf;
	size_t scan_buf_sz;
	BOOL ret = 1;

	scan_buf = mk_scan_buf(str, &scan_buf_sz);
	state_buf = yy_scan_buffer(scan_buf, scan_buf_sz);

	yyparse();

	if (parser_root && parser_root->token_id == T_NIL
		/* e.g. a comment */) {
		tex_tr_release(parser_root);
		parser_root = NULL;
		ret = 0;
		
		parser_err_flg = 0; /* clear flag */
	}

	yy_delete_buffer(state_buf);
	cp_free(scan_buf);

	return ret;
}

static void print_series(FILE *fh, uint8_t *s)
{
	uint32_t i = 0;
	while (s[i] != 0) {
		fprintf(fh, "%d", s[i]);
		i++;

		if (s[i] != 0)
			fprintf(fh, ", ");
	}
}

void print_brw(struct brw *brw, FILE *fh)
{
	fprintf(fh, "sym: %s; ", trans_symbol(brw->symbol_id));
	fprintf(fh, "pins: ");
	print_series(fh, brw->pin);
	fprintf(fh, "; n & fans: "); 
	/* fan[0] is group number */
	print_series(fh, brw->fan);
	fprintf(fh, ".");
}

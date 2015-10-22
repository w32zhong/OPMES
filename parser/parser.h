#include "enum-symbol.h"
#include "enum-token.h"
#include "tex-tr.h"
#include "brw.h"

/* parser related */
extern int            yyparse();
extern int            yyerror(const char *);

extern struct tex_tr *parser_root;
extern int            parser_err_flg;
extern char           parser_err_msg[MAX_PARSER_ERR_STR];

/* yacc/bison related */
struct yy_buffer_state;
typedef struct yy_buffer_state *YY_BUFFER_STATE;
typedef size_t yy_size_t;

YY_BUFFER_STATE yy_scan_buffer(char *, yy_size_t);

void yy_delete_buffer(YY_BUFFER_STATE);

/* utilities */
char *mk_scan_buf(char*, size_t*);
BOOL  cp_parse(char*);
void  print_brw(struct brw*, FILE*);

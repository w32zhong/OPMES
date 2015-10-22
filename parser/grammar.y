%{
#include <stdio.h>
#include <string.h>
#include "common.h"
#include "enum-symbol.h"
#include "enum-token.h"
#include "tex-tr.h"

#define TR_CONS(_name, _f, _s0, _s1) \
	_f = tex_tr_alloc(S_ ## _name, T_ ## _name); \
	tex_tr_attatch(_f, _s0); \
	tex_tr_attatch(_f, _s1); \
	parser_root = _f;

#define TR_ATTACH(_ret, _f, _s0, _s1) \
	tex_tr_attatch(_f, _s0); \
	tex_tr_attatch(_f, _s1); \
	_ret = parser_root = _f;

extern int            yyerror(const char *);
extern int            yylex(void);
extern struct tex_tr *parser_root;
extern int            parser_err_flg;
extern char           parser_err_msg[MAX_PARSER_ERR_STR];

%}

%union { 
	struct tex_tr *t;
}

%error-verbose

%token <t> VAR
%token <t> NUM
%token <t> ZERO
%token <t> ONE
%token _TIMES 
%token _DIV 
%token _FRAC 
%token <t> FRAC__
%token _COMBIN
%token <t> COMBIN__
%token _SQRT 
%token _ROOT _OF
%token _OVER
%token _ABOVE
%token _VERT
%token _REL
%token _SEP_CLASS 
%token <t> SUM_CLASS 
%token <t> FUN_CLASS 
%token _X_ARROW 
%token _STACKREL 
%token _BUILDREL 
%token _SET_REL 
%token _CHOOSE
%token _MODULAR
%token _BEGIN_MAT _END_MAT
%token _TAB 
%token _PRIME_VAR
%token _PRIME_SUP
%token _DOTS 
%token _PARTIAL 
%token _PI 
%token _INFTY 
%token _EMPTY 
%token _ANGLE 
%token _PERP 
%token _CIRC 
%token _PERCENT 
%token _VECT

%type <t> rel
%type <t> tex
%type <t> abv_tex
%type <t> mat_tex
%type <t> term
%type <t> factor
%type <t> pack 
%type <t> atom
%type <t> s_atom
%type <t> script

%destructor { 
	if ($$ != NULL) {
		trace(YYDEBUG, "destructor:\n", NULL);

#if (TRACE_STDERR_YYDEBUG)
		tex_tr_print($$, stdout);
#endif

#if (TRACE_LOG_YYDEBUG)
		if (cp_log_fh())
			tex_tr_print($$, cp_log_fh());
#endif
		tex_tr_release($$);
		$$ = NULL;
	}
} <t> 

 /* associativity rules is order-sensitive */
%right _OVER 
%right _CHOOSE

%right _TAB

%right _SEP_CLASS 
%right _REL 

%right _X_ARROW 
%right _STACKREL 
%right _BUILDREL 
%right _SET_REL 

%right _MODULAR 
%right _ABOVE 

%left NULL_REDUCE
%left  '+' '-' 
%nonassoc '!' 
%nonassoc _PRIME_SUP
%right '^' '_' 
%left _TIMES _DIV
%nonassoc '{' '}' 
%nonassoc '[' ']' 
%nonassoc '(' ')' 
%nonassoc _LEFT_CEIL _RIGHT_CEIL 
%nonassoc _LEFT_FLOOR _RIGHT_FLOOR 

 /* begin grammar rules */
%%
doc : tex '\n' ;

rel 
: atom {
	parser_root = $$ = $1;
}
| _REL {
	TR_CONS(EQ_CLASS, $$, NULL, NULL);
}
;

tex 
 /* a NULL reduce precedence */
: %prec NULL_REDUCE {
	TR_CONS(NIL, $$, NULL, NULL);
}
| term {
	parser_root = $$ = $1;
}
| tex '+' term {
	TR_CONS(ADD, $$, $1, $3);
}
| tex '+' {
	TR_CONS(ADD, $$, $1, NULL);
}
| tex '-' term {
	struct tex_tr *neg;
	TR_CONS(NEG, neg, $3, NULL);
	TR_CONS(ADD, $$, $1, neg);
}
| tex '-' {
	parser_root = $$ = $1;
}
| tex _REL tex {
	TR_CONS(EQ_CLASS, $$, $1, $3);
}
| tex _SEP_CLASS tex {
	TR_CONS(SEP_CLASS, $$, $1, $3);
}
| tex _ABOVE tex {
	TR_CONS(FRAC, $$, $1, $3);
}
| tex _OVER tex {
	TR_CONS(FRAC, $$, $1, $3);
}
| tex _MODULAR tex {
	TR_CONS(MODULAR, $$, $1, $3);
}
 /* below tex rules will not appear in matrix */
| tex _CHOOSE tex {
	TR_CONS(CHOOSE, $$, $1, $3);
}
 /* and some stack above rules */
| tex _STACKREL atom rel tex {
	TR_ATTACH($$, $4, $1, $5);
	tex_tr_release($3);
}
| tex _BUILDREL abv_tex _OVER rel tex {
	TR_ATTACH($$, $5, $1, $6);
	tex_tr_release($3);
}
| tex _SET_REL atom rel tex {
	TR_ATTACH($$, $4, $1, $5);
	tex_tr_release($3);
}
| tex _X_ARROW atom tex {
	struct tex_tr *x_arrow;
	TR_CONS(EQ_CLASS, x_arrow, NULL, NULL);
	TR_ATTACH($$, x_arrow, $1, $4);
	tex_tr_release($3);
}
| tex _X_ARROW '[' tex ']' atom tex {
	struct tex_tr *x_arrow;
	TR_CONS(EQ_CLASS, x_arrow, NULL, NULL);
	TR_ATTACH($$, x_arrow, $1, $7);
	tex_tr_release($4);
	tex_tr_release($6);
}
;

abv_tex 
 /* a NULL reduce precedence */
: %prec NULL_REDUCE {
	TR_CONS(NIL, $$, NULL, NULL);
}
| term {
	parser_root = $$ = $1;
}
| abv_tex '+' term {
	TR_CONS(ADD, $$, $1, $3);
}
| abv_tex '+' {
	TR_CONS(ADD, $$, $1, NULL);
}
| abv_tex '-' term {
	struct tex_tr *neg;
	TR_CONS(NEG, neg, $3, NULL);
	TR_CONS(ADD, $$, $1, neg);
}
| abv_tex '-' {
	parser_root = $$ = $1;
}
| abv_tex _REL abv_tex {
	TR_CONS(EQ_CLASS, $$, $1, $3);
}
| abv_tex _SEP_CLASS abv_tex {
	TR_CONS(SEP_CLASS, $$, $1, $3);
}
;

mat_tex 
 /* a NULL reduce precedence */
: %prec NULL_REDUCE {
	TR_CONS(NIL, $$, NULL, NULL);
}
| term {
	parser_root = $$ = $1;
}
| mat_tex '+' term {
	TR_CONS(ADD, $$, $1, $3);
}
| mat_tex '+' {
	TR_CONS(ADD, $$, $1, NULL);
}
| mat_tex '-' term {
	struct tex_tr *neg;
	TR_CONS(NEG, neg, $3, NULL);
	TR_CONS(ADD, $$, $1, neg);
}
| mat_tex '-' {
	parser_root = $$ = $1;
}
| mat_tex _REL mat_tex {
	TR_CONS(EQ_CLASS, $$, $1, $3);
}
| mat_tex _SEP_CLASS mat_tex {
	TR_CONS(SEP_CLASS, $$, $1, $3);
}
| mat_tex _ABOVE mat_tex {
	TR_CONS(FRAC, $$, $1, $3);
}
| mat_tex _OVER mat_tex {
	TR_CONS(FRAC, $$, $1, $3);
}
| mat_tex _MODULAR mat_tex {
	TR_CONS(MODULAR, $$, $1, $3);
}
| mat_tex _TAB mat_tex {
	TR_CONS(TAB, $$, $1, $3);
}
;

term 
: factor {
	parser_root = $$ = $1;
}
| term factor {
	TR_CONS(TIMES, $$, $1, $2);
}
| term _TIMES factor {
	TR_CONS(TIMES, $$, $1, $3);
}
| term _DIV factor {
	TR_CONS(FRAC, $$, $1, $3);
}
;

factor 
: pack {
	parser_root = $$ = $1;
}
| factor '!' {
	TR_CONS(FACT, $$, $1, NULL);
}
 /* 
  * legal prime/script combination:
  * a''^2_2 \\
  * a''^2   \\
  * a''_2 
  */
| factor _PRIME_SUP {
	struct tex_tr *prime;
	TR_CONS(PRIME, prime, NULL, NULL);
	TR_CONS(HANGER, $$, $1, prime);
}
| factor script {
	TR_ATTACH($$, $2, $1, NULL);
}
| factor _PRIME_SUP script {
	struct tex_tr *prime;
	TR_CONS(PRIME, prime, NULL, NULL);
	TR_ATTACH($$, $3, $1, prime);
}
;

pack 
: atom {
	parser_root = $$ = $1;
}
| '(' tex ')' {
	parser_root = $$ = $2;
}
 /* tree range rules */
| '(' tex ']' {
	parser_root = $$ = $2;
}
| '[' tex ')' {
	parser_root = $$ = $2;
}
| '[' tex ']' {
	parser_root = $$ = $2;
}
 /* ceil mat and floor :P */
| _LEFT_CEIL tex _RIGHT_CEIL {
	TR_CONS(CEIL, $$, $2, NULL);
}
| _BEGIN_MAT mat_tex _END_MAT {
	parser_root = $$ = $2;
}
| _LEFT_FLOOR tex _RIGHT_FLOOR {
	TR_CONS(FLOOR, $$, $2, NULL);
}
;
 
atom
: NUM {
	parser_root = $$ = $1;
} 
| ZERO {
	parser_root = $$ = $1;
} 
| ONE {
	parser_root = $$ = $1;
} 
| VAR {
	parser_root = $$ = $1;
}
| FUN_CLASS {
	parser_root = $$ = $1;
}
| SUM_CLASS {
	parser_root = $$ = $1;
}
| '{' tex '}' {
	parser_root = $$ = $2;
}
| FRAC__ {
	parser_root = $$ = $1;
}
| _FRAC atom atom {
	TR_CONS(FRAC, $$, $2, $3);
}
| COMBIN__ {
	parser_root = $$ = $1;
}
| _COMBIN atom atom {
	TR_CONS(CHOOSE, $$, $2, $3);
}
| _VECT atom {
	TR_CONS(VECT, $$, $2, NULL);
}
| _SQRT atom {
	TR_CONS(SQRT, $$, $2, NULL);
}
| _SQRT '[' tex ']' atom {
	TR_CONS(SQRT, $$, $5, $3);
}
| _ROOT atom _OF atom {
	TR_CONS(SQRT, $$, $4, $2);
}
| '*' {
	TR_CONS(STAR, $$, NULL, NULL);
}
| _PRIME_VAR {
	TR_CONS(PRIME, $$, NULL, NULL);
}
| _DOTS {
	TR_CONS(DOTS, $$, NULL, NULL);
}
| _PARTIAL {
	TR_CONS(PARTIAL, $$, NULL, NULL);
}
| _PI {
	TR_CONS(PI, $$, NULL, NULL);
}
| _INFTY {
	TR_CONS(INFTY, $$, NULL, NULL);
}
| _EMPTY {
	TR_CONS(EMPTY, $$, NULL, NULL);
}
| _ANGLE {
	TR_CONS(ANGLE, $$, NULL, NULL);
}
| _PERP {
	TR_CONS(PERP, $$, NULL, NULL);
}
| _CIRC {
	TR_CONS(CIRC, $$, NULL, NULL);
}
| _PERCENT {
	TR_CONS(PERCENT, $$, NULL, NULL);
}
| _VERT {
	TR_CONS(VERT, $$, NULL, NULL);
}
;

s_atom
: atom {
	parser_root = $$ = $1;
} 
| '+' {
	TR_CONS(POS, $$, NULL, NULL);
}
| '-' {
	TR_CONS(NEG, $$, NULL, NULL);
}
| _TIMES {
	TR_CONS(STAR, $$, NULL, NULL);
}
| '{' _TIMES '}' {
	TR_CONS(STAR, $$, NULL, NULL);
}
;

script
: '_' s_atom {
	struct tex_tr *sub;
	TR_CONS(SUB_SCRIPT, sub, $2, NULL);
	TR_CONS(HANGER, $$, sub, NULL);
}
| '^' s_atom {
	struct tex_tr *sup;
	TR_CONS(SUP_SCRIPT, sup, $2, NULL);
	TR_CONS(HANGER, $$, sup, NULL);
}
| '_' s_atom '^' s_atom {
	struct tex_tr *sub, *sup;
	TR_CONS(SUB_SCRIPT, sub, $2, NULL);
	TR_CONS(SUP_SCRIPT, sup, $4, NULL);
	TR_CONS(HANGER, $$, sub, sup);
}
| '^' s_atom '_' s_atom {
	struct tex_tr *sub, *sup;
	TR_CONS(SUB_SCRIPT, sub, $4, NULL);
	TR_CONS(SUP_SCRIPT, sup, $2, NULL);
	TR_CONS(HANGER, $$, sub, sup);
}
;

%%
struct tex_tr *parser_root = NULL;
int            parser_err_flg = 0;
char           parser_err_msg[MAX_PARSER_ERR_STR];

int yyerror(const char *msg)
{
	strcpy(parser_err_msg, msg);
	/* set root to NULL to avoid double free */
	parser_root = NULL; 
	parser_err_flg = 1;

	return 0;
}

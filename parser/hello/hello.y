%{
#include <stdio.h>
#include <string.h>
#include "common.h"

extern int yylex(void);
extern int yyerror(const char *);
%}

%error-verbose
%union { char *str; }

%token FRAC 
%token LF 
%type  <str> atom
%token <str> VAR NUM FRAC__

%destructor { printf("free: %s\n", $$); cp_free($$);} <str> 
%%
 /* overall structure */
doc
:
| doc line
;

 /* line structure */
line
: frac LF {}
| LF {}
;
 
 /* atom is an united state :P */
atom
: NUM {
	$$ = $1;
} 
| VAR {
	$$ = $1;
}
| '{' atom '}' {
	$$ = cp_malloc(32); 
	sprintf($$, "<%s>", $2);
	cp_free($2);
}
;

 /* frac structure */
frac 
: FRAC atom atom {
	printf("%s/%s\n", $2, $3); 
	cp_free($2);
	cp_free($3);
}
| FRAC__ {
	char a= $1[strlen($1) - 2], b = $1[strlen($1) - 1];
	/* hack it to handle something like `\frac 12' */
	printf("hack `%s'...\n", $1);
	printf("%c/%c\n", a, b); 
	cp_free($1);
}
;
%%

int yyerror(const char *msg)
{
	trace(YYERR, "%s\n", msg);
	return 0;
}

int main(int argc,char *argv[])
{   
	trace_init("parser-hello.log");	
	yyparse();

	trace_unfree();
	trace_uninit();

	printf("bye!\n");
	return 0;
}

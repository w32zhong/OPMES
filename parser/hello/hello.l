%{
#include "y.tab.h"
#include <stdio.h>
#include "common.h"
%}

%%

[\t ]	              { };
"{"|"}"               { return yytext[0]; }
\n                    { return LF; }
\\frac[ ]*[0-9][0-9]  { yylval.str = cp_strdup(yytext); return FRAC__; }
\\frac                { return FRAC; }
[0-9]+                { yylval.str = cp_strdup(yytext); return NUM; }
[a-z]                 { yylval.str = cp_strdup(yytext); return VAR; }
\\[a-z]+              { }
.                     { printf("%s escaped.\n", yytext); }
%%

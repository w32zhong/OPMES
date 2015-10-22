%{
#include "y.tab.h"
#include "common.h"
#include "enum-symbol.h"
#include "enum-token.h"
#include "tex-tr.h"

#define RET_TOK(_symbol, _token) \
	yylval.t = tex_tr_alloc(S_ ## _symbol, T_ ## _token); \
	return _token;

#define LVAL (&yylval.t)

#define RET_HACK(_name, _ret) \
	return ret_hack(S_ ## _name, T_ ## _name, _ret, \
	                yytext, &yylval.t);

int ret_var(char*, struct tex_tr **, char);
int ret_num(char*, struct tex_tr **);
int ret_float(char*, struct tex_tr **);
int ret_hack(int, int, int, char*, struct tex_tr **);

extern unsigned int ign_stack;
extern unsigned int mat_stack;

int bgn_mat();
%}

 /* some start conditions */
 
 /* for dimension */
%x dim
 /* to be ignored */
%x ign
 /* comment */
%x cmt
 /* matrix (table) related */
%s mat

%%
 /* invisible and space */

\n                                      { return yytext[0]; }
"\\!"            { /* omit short space, before factorial */ } 
"\\;"            { /* omit short space, before semicolon */ } 
"\\,"                { /* omit short space, before comma */ } 
[\t ]                                    { /* omit space */ }

 /* comment */
"%"                                           { BEGIN(cmt); }
<cmt>\n                      { BEGIN(INITIAL); return '\n'; }
<cmt>.                                       { /* ignore */ }

 /* super-script and sub-script */

"_"|"^"                                 { return yytext[0]; }

 /* add and minus */

"+"|"-"                                 { return yytext[0]; }
\\oplus                                       { return '+'; }
\\uplus                                       { return '+'; }
\\dotplus                                     { return '+'; }
\\pm                                          { return '+'; }
\\mp                                          { return '+'; }
"±"                                           { return '+'; }
\\neg                                         { return '-'; }
\\ominus                                      { return '-'; }
\\setminus                                    { return '-'; }
\\backslash                                   { return '-'; }
\\smallsetminus                               { return '-'; }
\\lnot                                        { return '-'; } 
\\barwedge                                    { return '-'; } 

 /* star and times */

"*"                                           { return '*'; }
\\times                                    { return _TIMES; }
\\otimes                                   { return _TIMES; }
\\ltimes                                   { return _TIMES; }
\\rtimes                                   { return _TIMES; }
\\cdot                                     { return _TIMES; }
\\odot                                     { return _TIMES; }
\\ast                                      { return _TIMES; }

 /* factorial */

"!"                                           { return '!'; }

 /* fraction related */

 /* over */
\\over                                      { return _OVER; }
 /* above */
\\above                        { BEGIN(dim); return _ABOVE; }
 /* div */
\/                                           { return _DIV; }
\\div                                        { return _DIV; }
\\divideontimes                              { return _DIV; }
 /* frac */
\\frac                                      { return _FRAC; }
\\dfrac                                     { return _FRAC; }
\\cfrac                                     { return _FRAC; }
\\tfrac                                     { return _FRAC; }
\\frac[ ]*[0-9][0-9]              { RET_HACK(FRAC, FRAC__); }
\\dfrac[ ]*[0-9][0-9]             { RET_HACK(FRAC, FRAC__); } 
\\cfrac[ ]*[0-9][0-9]             { RET_HACK(FRAC, FRAC__); } 
\\tfrac[ ]*[0-9][0-9]             { RET_HACK(FRAC, FRAC__); } 

 /* left right pair */

 /* left */
"("|"\\{"                                     { return '('; }
\\left[ ]*"."                                 { return '('; }
\\left[ ]*"("                                 { return '('; }
\\left[ ]*\\lgroup                            { return '('; }
\\left[ ]*"["                                 { return '('; }
\\left[ ]*"\\{"                               { return '('; }
\\left[ ]*\\langle                            { return '('; }
\\left[ ]*"<"                                 { return '('; }
\\left[ ]*"/"                                 { return '('; }
\\left[ ]*"\\"                                { return '('; }
\\left[ ]*\\backslash                         { return '('; }
\\left[ ]*\\lmoustache                        { return '('; }
\\left[ ]*\\[Uu]parrow                        { return '('; }
\\left[ ]*\\[Dd]ownarrow                      { return '('; }
\\left[ ]*\\[Uu]pdownarrow                    { return '('; }
\\left                                        { return '('; }
 /* right */
")"|"\\}"                                     { return ')'; }
\\right[ ]*"."                                { return ')'; }
\\right[ ]*")"                                { return ')'; }
\\right[ ]*\\rgroup                           { return ')'; }
\\right[ ]*"]"                                { return ')'; }
\\right[ ]*"\\}"                              { return ')'; }
\\right[ ]*\\rangle                           { return ')'; }
\\right[ ]*">"                                { return ')'; }
\\right[ ]*"/"                                { return ')'; }
\\right[ ]*"\\"                               { return ')'; }
\\right[ ]*\\backslash                        { return ')'; }
\\right[ ]*\\rmoustache                       { return ')'; }
\\right[ ]*\\[Uu]parrow                       { return ')'; }
\\right[ ]*\\[Dd]ownarrow                     { return ')'; }
\\right[ ]*\\[Uu]pdownarrow                   { return ')'; }
\\right                                       { return ')'; }
 /* ceil and floor */
\\left[ ]*\\lceil                     { return _LEFT_CEIL;  }
\\left[ ]*\\lfloor                    { return _LEFT_FLOOR; }
\\right[ ]*\\rceil                   { return _RIGHT_CEIL;  }
\\right[ ]*\\rfloor                  { return _RIGHT_FLOOR; }
 /* shorter ceil and floor */
\\lceil                                { return _LEFT_CEIL; }
\\lfloor                              { return _LEFT_FLOOR; }
\\rceil                               { return _RIGHT_CEIL; }
\\rfloor                             { return _RIGHT_FLOOR; }
 /* shorter pair */
\\lgroup                                      { return '('; }
\\langle                                      { return '('; }
\\lmoustache                                  { return '('; }
\\lbrace                                      { return '('; }
\\lbrack                                      { return '('; }
\\rgroup                                      { return ')'; }
\\rangle                                      { return ')'; }
\\rmoustache                                  { return ')'; }
\\rbrace                                      { return ')'; }
\\rbrack                                      { return ')'; }
 /* non-discriminative pairs are crazy mass */
"|"                                         { return _VERT; }
"\\|"                                       { return _VERT; }
\\vert                                      { return _VERT; }
\\Vert                                      { return _VERT; }
\\[Aa]rrowvert                              { return _VERT; }
\\bracevert                                 { return _VERT; }
\\r[vV]ert                                  { return _VERT; }
\\l[vV]ert                                  { return _VERT; }
\\mid                                       { return _VERT; }

 /* table/matrix related */

\\begin\{matrix\}           { BEGIN(mat); return bgn_mat(); }
\\begin\{vmatrix\}          { BEGIN(mat); return bgn_mat(); }
\\begin\{Vmatrix\}          { BEGIN(mat); return bgn_mat(); }
\\begin\{bmatrix\}          { BEGIN(mat); return bgn_mat(); }
\\begin\{Bmatrix\}          { BEGIN(mat); return bgn_mat(); }
\\begin\{pmatrix\}          { BEGIN(mat); return bgn_mat(); }
\\begin\{smallmatrix\}      { BEGIN(mat); return bgn_mat(); }
\\begin\{cases\}            { BEGIN(mat); return bgn_mat(); }

\\end\{matrix\}          { BEGIN(INITIAL); return _END_MAT; } 
\\end\{vmatrix\}         { BEGIN(INITIAL); return _END_MAT; } 
\\end\{Vmatrix\}         { BEGIN(INITIAL); return _END_MAT; } 
\\end\{bmatrix\}         { BEGIN(INITIAL); return _END_MAT; } 
\\end\{Bmatrix\}         { BEGIN(INITIAL); return _END_MAT; } 
\\end\{pmatrix\}         { BEGIN(INITIAL); return _END_MAT; } 
\\end\{smallmatrix\}     { BEGIN(INITIAL); return _END_MAT; } 
\\end\{cases\}           { BEGIN(INITIAL); return _END_MAT; } 

\\array\{                   { BEGIN(mat); return bgn_mat(); }

<mat>"{"                  { mat_stack ++; return yytext[0]; }

<mat>\\\\                                    { return _TAB; }
<mat>\\cr                                    { return _TAB; }
<mat>\\newline                               { return _TAB; }
<mat>&                                       { return _TAB; }

<mat>"}" { 
	mat_stack --; 
	if (!mat_stack) { 
		BEGIN(INITIAL);
		return _END_MAT; 
	} else {
		return yytext[0]; 
	}
}

 /* commands to be ignored */

\\color\{                       { BEGIN(ign); ign_stack ++; } 
\\mbox\{                        { BEGIN(ign); ign_stack ++; }
\\hbox\{                        { BEGIN(ign); ign_stack ++; }
\\label\{                       { BEGIN(ign); ign_stack ++; }
\\tag\{                         { BEGIN(ign); ign_stack ++; }
\\text\{                        { BEGIN(ign); ign_stack ++; }
\\leftroot\{                    { BEGIN(ign); ign_stack ++; }
\\uproot\{                      { BEGIN(ign); ign_stack ++; }

<ign>\{                         { ign_stack ++; }
<ign>\}     { ign_stack --; if (!ign_stack) BEGIN(INITIAL); }
<ign>.                                                     {}

 /* dimension related states */

\\hspace                                      { BEGIN(dim); }
\\hskip                                       { BEGIN(dim); }
\\mspace                                      { BEGIN(dim); }
\\mskip                                       { BEGIN(dim); }
\\mkern                                       { BEGIN(dim); }
\\kern                                        { BEGIN(dim); }

<dim>em\}?                                { BEGIN(INITIAL); }
<dim>ex\}?                                { BEGIN(INITIAL); }
<dim>pt\}?                                { BEGIN(INITIAL); }
<dim>pc\}?                                { BEGIN(INITIAL); }
<dim>mu\}?                                { BEGIN(INITIAL); }
<dim>cm\}?                                { BEGIN(INITIAL); }
<dim>mm\}?                                { BEGIN(INITIAL); }
<dim>in\}?                                { BEGIN(INITIAL); }
<dim>px\}?                                { BEGIN(INITIAL); }
<dim>.                                                     {}

 /* latex braket */

"{"|"}"                                 { return yytext[0]; }
"["|"]"                                 { return yytext[0]; }

 /* binomial coefficient */

\\choose                                  { return _CHOOSE; }
\\brack                                   { return _CHOOSE; }

\\dbinom[ ]*[0-9][0-9]        { RET_HACK(CHOOSE, COMBIN__); }
\\tbinom[ ]*[0-9][0-9]        { RET_HACK(CHOOSE, COMBIN__); } 
\\binom[ ]*[0-9][0-9]         { RET_HACK(CHOOSE, COMBIN__); } 
\\dbinom                                  { return _COMBIN; }
\\tbinom                                  { return _COMBIN; }
\\binom                                   { return _COMBIN; }

 /* modular operation */

\\pmod                                   { return _MODULAR; } 
\\bmod                                   { return _MODULAR; } 
\\mod                                    { return _MODULAR; } 

 /* prime */

"'"                                    { return _PRIME_SUP; }
\\prime                                { return _PRIME_VAR; } 

 /* sqrt and root */

\\sqrt                                      { return _SQRT; }
\\root                                      { return _ROOT; }
\\of                                        { return _OF;   }

 /* vector */
 
\\vec                                       { return _VECT; }
\\overleftarrow                             { return _VECT; }
\\overrightarrow                            { return _VECT; }

 /* partial */
 
\\partial                                { return _PARTIAL; } 
\\nabla                                  { return _PARTIAL; } 

 /* PI */

\\pi                                          { return _PI; } 

 /* infty */

\\infty                                    { return _INFTY; }
∞                                          { return _INFTY; }

 /* empty */

\\empty                                    { return _EMPTY; }
\\emptyset                                 { return _EMPTY; }
\\varnothing                               { return _EMPTY; }

 /* angle */

\\triangle                                 { return _ANGLE; }
\\triangledown                             { return _ANGLE; }
\\angle                                    { return _ANGLE; }
\\vartriangle                              { return _ANGLE; }
\\vartriangleleft                          { return _ANGLE; }
\\vartriangleright                         { return _ANGLE; }
\\triangleleft                             { return _ANGLE; }
\\triangleright                            { return _ANGLE; }
\\measuredangle                            { return _ANGLE; }
\\sphericalangle                           { return _ANGLE; }

 /* perpendicular */

\\perp                                      { return _PERP; }
\\bot                                       { return _PERP; }

 /* circle */

\\circ                                      { return _CIRC; }

 /* percentage */

\\%                                      { return _PERCENT; }

 /* dots */

\\dots                                      { return _DOTS; }
\.\.\.                                      { return _DOTS; }
\\ldots                                     { return _DOTS; }
\\vdots                                     { return _DOTS; }
\\cdots                                     { return _DOTS; }
\\ddots                                     { return _DOTS; }
\\ddot                                      { return _DOTS; }
\\dddot                                     { return _DOTS; }
\\ddddot                                    { return _DOTS; }
\\dotsb                                     { return _DOTS; }
\\dotsc                                     { return _DOTS; }
\\dotsi                                     { return _DOTS; }
\\dotsm                                     { return _DOTS; }
\\dotso                                     { return _DOTS; }
\\iddots                                    { return _DOTS; }

 /* equality class */

=                                            { return _REL; }
:=                                           { return _REL; }
"<"                                          { return _REL; }
">"                                          { return _REL; }
\\Cap                                        { return _REL; }
\\Cup                                        { return _REL; }
\\Join                                       { return _REL; }
\\Prec                                       { return _REL; }
\\Subset                                     { return _REL; }
\\Supset                                     { return _REL; }
\\[dD]oteq                                   { return _REL; }
\\and                                        { return _REL; } 
\\approx                                     { return _REL; }
\\approxeq                                   { return _REL; }
\\asymp                                      { return _REL; }
\\backepsilon                                { return _REL; }
\\backsim                                    { return _REL; }
\\backsimeq                                  { return _REL; }
\\bowtie                                     { return _REL; }
\\cap                                        { return _REL; }
\\circeq                                     { return _REL; }
\\cong                                       { return _REL; }
\\cup                                        { return _REL; }
\\curlyeqprec                                { return _REL; }
\\curlyeqsucc                                { return _REL; }
\\curlyvee                                   { return _REL; }
\\curlywedge                                 { return _REL; }
\\dashv                                      { return _REL; }
\\dot=                                       { return _REL; }
\\eqslantgtr                                 { return _REL; }
\\eqslantless                                { return _REL; }
\\equiv                                      { return _REL; }
\\ge                                         { return _REL; }
\\geq                                        { return _REL; }
\\geqq                                       { return _REL; }
\\geqslant                                   { return _REL; }
\\gg                                         { return _REL; }
\\gnapprox                                   { return _REL; }
\\gnsim                                      { return _REL; }
\\gt                                         { return _REL; }
\\gtrapprox                                  { return _REL; }
\\gtrdot                                     { return _REL; }
\\gtreqless                                  { return _REL; }  
\\gtreqqless                                 { return _REL; } 
\\gtrless                                    { return _REL; } 
\\gtrsim                                     { return _REL; }
\\in                                         { return _REL; }
\\land                                       { return _REL; } 
\\le                                         { return _REL; }
\\leadsto                                    { return _REL; }
\\leq                                        { return _REL; }
\\leqq                                       { return _REL; }
\\leqslant                                   { return _REL; }
\\lessapprox                                 { return _REL; }
\\lessdot                                    { return _REL; }
\\lesssim                                    { return _REL; }
\\ll                                         { return _REL; }
\\lnapprox                                   { return _REL; }
\\lneq                                       { return _REL; }
\\lneqq                                      { return _REL; }
\\lor                                        { return _REL; } 
\\lt                                         { return _REL; }
\\lvertneqq                                  { return _REL; }
\\ncong                                      { return _REL; }
\\ne                                         { return _REL; }
\\neq                                        { return _REL; }
\\ngeq                                       { return _REL; }
\\ngeqq                                      { return _REL; }
\\ngeqslant                                  { return _REL; }
\\ni                                         { return _REL; }
\\nleq                                       { return _REL; }
\\nleqq                                      { return _REL; }
\\nleqslant                                  { return _REL; }
\\nless                                      { return _REL; } 
\\not("="|"\equiv"|"\in")                    { return _REL; }
\\nparallel                                  { return _REL; }
\\nprec                                      { return _REL; }
\\npreceq                                    { return _REL; }
\\nsim                                       { return _REL; }
\\nsubseteq                                  { return _REL; }
\\nsucc                                      { return _REL; }
\\nsucceq                                    { return _REL; }
\\nsupseteq                                  { return _REL; }
\\owns                                       { return _REL; }
\\parallel                                   { return _REL; }
\\prec                                       { return _REL; }
\\preceq                                     { return _REL; }
\\propto                                     { return _REL; }
\\sim                                        { return _REL; }
\\simeq                                      { return _REL; }
\\sqcap                                      { return _REL; }
\\sqcup                                      { return _REL; }
\\sqsubset                                   { return _REL; }
\\sqsubseteq                                 { return _REL; }
\\sqsupset                                   { return _REL; }
\\sqsupseteq                                 { return _REL; }
\\subset                                     { return _REL; }
\\subseteq                                   { return _REL; }
\\subseteqq                                  { return _REL; }
\\subsetneq                                  { return _REL; }
\\subsetneqq                                 { return _REL; }
\\succ                                       { return _REL; }
\\succapprox                                 { return _REL; }
\\succcurlyeq                                { return _REL; }
\\succeq                                     { return _REL; }
\\succnapprox                                { return _REL; }
\\succneqq                                   { return _REL; }
\\succnsim                                   { return _REL; }
\\succsim                                    { return _REL; }
\\supset                                     { return _REL; }
\\supseteq                                   { return _REL; }
\\supseteqq                                  { return _REL; }
\\supsetneq                                  { return _REL; }
\\supsetneqq                                 { return _REL; }
\\thickapprox                                { return _REL; }
\\thicksim                                   { return _REL; }
\\trianglelefteq                             { return _REL; }
\\triangleq                                  { return _REL; }
\\trianglerighteq                            { return _REL; }
\\unlhd                                      { return _REL; }
\\unrhd                                      { return _REL; }
\\varsubsetneq                               { return _REL; }
\\varsubsetneqq                              { return _REL; }
\\varsupsetneq                               { return _REL; }
\\varsupsetneqq                              { return _REL; }
\\vee                                        { return _REL; } 
\\veebar                                     { return _REL; } 
\\wedge                                      { return _REL; } 
 
 /* stack above operations */

\\xleftarrow                            { return _X_ARROW;  }
\\xrightarrow                           { return _X_ARROW;  }
\\stackrel                              { return _STACKREL; }
\\buildrel                              { return _BUILDREL; }
\\overset                               { return _SET_REL;  }
\\underset                              { return _SET_REL;  }

 /* seperation class */

 /* table related */                              
\\\\                                   { return _SEP_CLASS; }
\\cr                                   { return _SEP_CLASS; }
\\newline                              { return _SEP_CLASS; }
\\:                                    { return _SEP_CLASS; }
\\>                                    { return _SEP_CLASS; }
\\enspace                              { return _SEP_CLASS; }
 /* -- */                              
,                                      { return _SEP_CLASS; }
;                                      { return _SEP_CLASS; }
\colon                                 { return _SEP_CLASS; }
":"                                    { return _SEP_CLASS; }
\\\&                                   { return _SEP_CLASS; }
\\And                                  { return _SEP_CLASS; }
\\Downarrow                            { return _SEP_CLASS; }
\\Leftarrow                            { return _SEP_CLASS; }
\\Leftrightarrow                       { return _SEP_CLASS; }
\\Lleftarrow                           { return _SEP_CLASS; }
\\Longleftarrow                        { return _SEP_CLASS; }
\\Longleftrightarrow                   { return _SEP_CLASS; }
\\Longrightarrow                       { return _SEP_CLASS; }
\\Lsh                                  { return _SEP_CLASS; }
\\Rightarrow                           { return _SEP_CLASS; }
\\Rrightarrow                          { return _SEP_CLASS; }
\\Rsh                                  { return _SEP_CLASS; }
\\Uparrow                              { return _SEP_CLASS; }
\\Updownarrow                          { return _SEP_CLASS; }
\\atop                                 { return _SEP_CLASS; }
\\between                              { return _SEP_CLASS; }
\\circlearrowleft                      { return _SEP_CLASS; }
\\circlearrowright                     { return _SEP_CLASS; }
\\curvearrowleft                       { return _SEP_CLASS; }
\\curvearrowright                      { return _SEP_CLASS; }
\\downarrow                            { return _SEP_CLASS; }
\\downdownarrows                       { return _SEP_CLASS; }
\\downharpoonleft                      { return _SEP_CLASS; }
\\downharpoonright                     { return _SEP_CLASS; }
\\eqcirc                               { return _SEP_CLASS; }
\\eqsim                                { return _SEP_CLASS; }
\\exists                               { return _SEP_CLASS; }
\\forall                               { return _SEP_CLASS; }
\\frown                                { return _SEP_CLASS; }
\\gets                                 { return _SEP_CLASS; }
\\ggg                                  { return _SEP_CLASS; }
\\gggtr                                { return _SEP_CLASS; }
\\gneq                                 { return _SEP_CLASS; }
\\gneqq                                { return _SEP_CLASS; }
\\gvertneqq                            { return _SEP_CLASS; }
\\hookleftarrow                        { return _SEP_CLASS; }
\\hookrightarrow                       { return _SEP_CLASS; }
\\iff                                  { return _SEP_CLASS; }
\\impliedby                            { return _SEP_CLASS; }
\\implies                              { return _SEP_CLASS; }
\\leftarrow                            { return _SEP_CLASS; }
\\leftarrowtail                        { return _SEP_CLASS; }
\\leftharpoondown                      { return _SEP_CLASS; }
\\leftharpoonup                        { return _SEP_CLASS; }
\\leftleftarrows                       { return _SEP_CLASS; }
\\leftrightarrow                       { return _SEP_CLASS; }
\\leftrightarrows                      { return _SEP_CLASS; }
\\leftrightharpoons                    { return _SEP_CLASS; }
\\leftrightsquigarrow                  { return _SEP_CLASS; }
\\longleftarrow                        { return _SEP_CLASS; }
\\longleftrightarrow                   { return _SEP_CLASS; }
\\longmapsto                           { return _SEP_CLASS; }
\\longrightarrow                       { return _SEP_CLASS; }
\\looparrowleft                        { return _SEP_CLASS; }
\\looparrowright                       { return _SEP_CLASS; }
\\mapsto                               { return _SEP_CLASS; }
\\multimap                             { return _SEP_CLASS; }
\\nLeftarrow                           { return _SEP_CLASS; }
\\nLeftrightarrow                      { return _SEP_CLASS; }
\\nRightarrow                          { return _SEP_CLASS; }
\\nearrow                              { return _SEP_CLASS; }
\\nexists                              { return _SEP_CLASS; }
\\ngtr                                 { return _SEP_CLASS; }
\\nleftarrow                           { return _SEP_CLASS; }
\\nleftrightarrow                      { return _SEP_CLASS; }
\\nmid                                 { return _SEP_CLASS; }
\\nrightarrow                          { return _SEP_CLASS; }
\\nwarrow                              { return _SEP_CLASS; }
\\precapprox                           { return _SEP_CLASS; }
\\preccurlyeq                          { return _SEP_CLASS; }
\\precnapprox                          { return _SEP_CLASS; }
\\precneqq                             { return _SEP_CLASS; }
\\precnsim                             { return _SEP_CLASS; }
\\precsim                              { return _SEP_CLASS; }
\\qquad                                { return _SEP_CLASS; }
\\quad                                 { return _SEP_CLASS; }
\\rightarrow                           { return _SEP_CLASS; }
\\rightarrowtail                       { return _SEP_CLASS; }
\\rightharpoondown                     { return _SEP_CLASS; }
\\rightharpoonup                       { return _SEP_CLASS; }
\\rightleftarrows                      { return _SEP_CLASS; }
\\rightleftharpoons                    { return _SEP_CLASS; }
\\rightrightarrows                     { return _SEP_CLASS; }
\\rightsquigarrow                      { return _SEP_CLASS; }
\\searrow                              { return _SEP_CLASS; }
\\smallfrown                           { return _SEP_CLASS; }
\\smallsmile                           { return _SEP_CLASS; }
\\swarrow                              { return _SEP_CLASS; }
\\to                                   { return _SEP_CLASS; }
\\uparrow                              { return _SEP_CLASS; }
\\updownarrow                          { return _SEP_CLASS; }
\\upharpoonleft                        { return _SEP_CLASS; }
\\upharpoonright                       { return _SEP_CLASS; }
\\upuparrows                           { return _SEP_CLASS; }

 /* variables */
 
 /* unicode variables */
α                                    { RET_TOK(alpha, VAR); }
β                                    { RET_TOK(beta, VAR);  }
 /* auto-generated discriminative tokens, where NIL is to be 
  * replaced by specially assigned symbol. */
\\Alpha                                { RET_TOK(NIL, VAR); } 
\\Beta                                 { RET_TOK(NIL, VAR); } 
\\Chi                                  { RET_TOK(NIL, VAR); } 
\\Delta                                { RET_TOK(NIL, VAR); } 
\\Epsilon                              { RET_TOK(NIL, VAR); } 
\\Eta                                  { RET_TOK(NIL, VAR); } 
\\Gamma                                { RET_TOK(NIL, VAR); } 
\\Iota                                 { RET_TOK(NIL, VAR); }
\\Kappa                                { RET_TOK(NIL, VAR); }
\\Lambda                               { RET_TOK(NIL, VAR); } 
\\Mu                                   { RET_TOK(NIL, VAR); } 
\\Nu                                   { RET_TOK(NIL, VAR); } 
\\Omega                                { RET_TOK(NIL, VAR); } 
\\Omicron                              { RET_TOK(NIL, VAR); }
\\Phi                                  { RET_TOK(NIL, VAR); } 
\\Pi                                   { RET_TOK(NIL, VAR); } 
\\Psi                                  { RET_TOK(NIL, VAR); } 
\\Re                                   { RET_TOK(NIL, VAR); }
\\Rho                                  { RET_TOK(NIL, VAR); } 
\\Sigma                                { RET_TOK(NIL, VAR); } 
\\Tau                                  { RET_TOK(NIL, VAR); } 
\\Theta                                { RET_TOK(NIL, VAR); } 
\\Upsilon                              { RET_TOK(NIL, VAR); } 
\\VarLambda                            { RET_TOK(NIL, VAR); } 
\\VarOmega                             { RET_TOK(NIL, VAR); } 
\\Xi                                   { RET_TOK(NIL, VAR); } 
\\Zeta                                 { RET_TOK(NIL, VAR); }
\\aleph                                { RET_TOK(NIL, VAR); }
\\alpha                                { RET_TOK(NIL, VAR); } 
\\amalg                                { RET_TOK(NIL, VAR); }
\\beta                                 { RET_TOK(NIL, VAR); } 
\\beth                                 { RET_TOK(NIL, VAR); } 
\\chi                                  { RET_TOK(NIL, VAR); } 
\\delta                                { RET_TOK(NIL, VAR); } 
\\ell                                  { RET_TOK(NIL, VAR); }
\\epsilon                              { RET_TOK(NIL, VAR); } 
\\eta                                  { RET_TOK(NIL, VAR); } 
\\eth                                  { RET_TOK(NIL, VAR); } 
\\gamma                                { RET_TOK(NIL, VAR); } 
\\imath                                { RET_TOK(NIL, VAR); }
\\iota                                 { RET_TOK(NIL, VAR); }
\\jmath                                { RET_TOK(NIL, VAR); }
\\kappa                                { RET_TOK(NIL, VAR); }
\\lambda                               { RET_TOK(NIL, VAR); } 
\\mho                                  { RET_TOK(NIL, VAR); }
\\mu                                   { RET_TOK(NIL, VAR); } 
\\nu                                   { RET_TOK(NIL, VAR); } 
\\omega                                { RET_TOK(NIL, VAR); } 
\\omicron                              { RET_TOK(NIL, VAR); }
\\phi                                  { RET_TOK(NIL, VAR); } 
\\psi                                  { RET_TOK(NIL, VAR); } 
\\rho                                  { RET_TOK(NIL, VAR); } 
\\sigma                                { RET_TOK(NIL, VAR); } 
\\tau                                  { RET_TOK(NIL, VAR); } 
\\theta                                { RET_TOK(NIL, VAR); } 
\\top                                  { RET_TOK(NIL, VAR); }
\\upsilon                              { RET_TOK(NIL, VAR); } 
\\varDelta                             { RET_TOK(NIL, VAR); } 
\\varGamma                             { RET_TOK(NIL, VAR); } 
\\varPhi                               { RET_TOK(NIL, VAR); } 
\\varPi                                { RET_TOK(NIL, VAR); }
\\varPsi                               { RET_TOK(NIL, VAR); } 
\\varSigma                             { RET_TOK(NIL, VAR); } 
\\varTheta                             { RET_TOK(NIL, VAR); } 
\\varUpsilon                           { RET_TOK(NIL, VAR); } 
\\varXi                                { RET_TOK(NIL, VAR); } 
\\varepsilon                           { RET_TOK(NIL, VAR); } 
\\varkappa                             { RET_TOK(NIL, VAR); } 
\\varphi                               { RET_TOK(NIL, VAR); } 
\\varpi                                { RET_TOK(NIL, VAR); }
\\varpropto                            { RET_TOK(NIL, VAR); } 
\\varrho                               { RET_TOK(NIL, VAR); }
\\varsigma                             { RET_TOK(NIL, VAR); } 
\\vartheta                             { RET_TOK(NIL, VAR); } 
\\wr                                   { RET_TOK(NIL, VAR); }
\\xi                                   { RET_TOK(NIL, VAR); } 
\\zeta                                 { RET_TOK(NIL, VAR); }

 /* functions, auto-generated */

\\Pr                             { RET_TOK(NIL, FUN_CLASS); }
\\arccos                         { RET_TOK(NIL, FUN_CLASS); }
\\arcsin                         { RET_TOK(NIL, FUN_CLASS); }
\\arctan                         { RET_TOK(NIL, FUN_CLASS); }
\\cos                            { RET_TOK(NIL, FUN_CLASS); }
\\cosh                           { RET_TOK(NIL, FUN_CLASS); }
\\cot                            { RET_TOK(NIL, FUN_CLASS); }
\\coth                           { RET_TOK(NIL, FUN_CLASS); }
\\csc                            { RET_TOK(NIL, FUN_CLASS); }
\\deg                            { RET_TOK(NIL, FUN_CLASS); }
\\det                            { RET_TOK(NIL, FUN_CLASS); }
\\dim                            { RET_TOK(NIL, FUN_CLASS); }
\\exp                            { RET_TOK(NIL, FUN_CLASS); }
\\gcd                            { RET_TOK(NIL, FUN_CLASS); }
\\hom                            { RET_TOK(NIL, FUN_CLASS); }
\\ker                            { RET_TOK(NIL, FUN_CLASS); }
\\lg                             { RET_TOK(NIL, FUN_CLASS); }
\\ln                             { RET_TOK(NIL, FUN_CLASS); }
\\log                            { RET_TOK(NIL, FUN_CLASS); }
\\max                            { RET_TOK(NIL, FUN_CLASS); }
\\min                            { RET_TOK(NIL, FUN_CLASS); }
\\sec                            { RET_TOK(NIL, FUN_CLASS); }
\\sin                            { RET_TOK(NIL, FUN_CLASS); }
\\sinh                           { RET_TOK(NIL, FUN_CLASS); }
\\tan                            { RET_TOK(NIL, FUN_CLASS); }
\\tanh                           { RET_TOK(NIL, FUN_CLASS); }
 /* not LaTeX standard commands */
\\sgn                            { RET_TOK(NIL, FUN_CLASS); }
\\signum                         { RET_TOK(NIL, FUN_CLASS); }
\\sign                           { RET_TOK(NIL, FUN_CLASS); }
 /* self-defined operator */
\\operatorname\*?\{[^}]*\}    { RET_TOK(sd_fun, FUN_CLASS); }

 /* sum class, auto-generated */

\\arg                            { RET_TOK(NIL, SUM_CLASS); }
\\bigcap                         { RET_TOK(NIL, SUM_CLASS); }
\\bigcup                         { RET_TOK(NIL, SUM_CLASS); }
\\bigcirc                        { RET_TOK(NIL, SUM_CLASS); }
\\bigodot                        { RET_TOK(NIL, SUM_CLASS); }
\\bigoplus                       { RET_TOK(NIL, SUM_CLASS); }
\\bigotimes                      { RET_TOK(NIL, SUM_CLASS); }
\\bigsqcup                       { RET_TOK(NIL, SUM_CLASS); }
\\bigtriangledown                { RET_TOK(NIL, SUM_CLASS); }
\\bigtriangleup                  { RET_TOK(NIL, SUM_CLASS); }
\\biguplus                       { RET_TOK(NIL, SUM_CLASS); }
\\bigvee                         { RET_TOK(NIL, SUM_CLASS); }
\\bigwedge                       { RET_TOK(NIL, SUM_CLASS); }
\\coprod                         { RET_TOK(NIL, SUM_CLASS); }
\\idotsint                       { RET_TOK(NIL, SUM_CLASS); }
\\int                            { RET_TOK(NIL, SUM_CLASS); }
\\iint                           { RET_TOK(NIL, SUM_CLASS); }
\\iiint                          { RET_TOK(NIL, SUM_CLASS); }
\\iiiint                         { RET_TOK(NIL, SUM_CLASS); }
\\intop                          { RET_TOK(NIL, SUM_CLASS); }
\\inf                            { RET_TOK(NIL, SUM_CLASS); }
\\injlim                         { RET_TOK(NIL, SUM_CLASS); }
\\smallint                       { RET_TOK(NIL, SUM_CLASS); }
\\sup                            { RET_TOK(NIL, SUM_CLASS); }
\\varinjlim                      { RET_TOK(NIL, SUM_CLASS); }
\\varprojlim                     { RET_TOK(NIL, SUM_CLASS); }
\\lim                            { RET_TOK(NIL, SUM_CLASS); }
\\liminf                         { RET_TOK(NIL, SUM_CLASS); }
\\limsup                         { RET_TOK(NIL, SUM_CLASS); }
\\varliminf                      { RET_TOK(NIL, SUM_CLASS); }
\\varlimsup                      { RET_TOK(NIL, SUM_CLASS); }
\\oint                           { RET_TOK(NIL, SUM_CLASS); }
\\prod                           { RET_TOK(NIL, SUM_CLASS); }
\\projlim                        { RET_TOK(NIL, SUM_CLASS); }
\\sum                            { RET_TOK(NIL, SUM_CLASS); }
 
 /* skip evironment statements */

\\begin\{align\}                                           {} 
\\end\{align\}                                             {}  
\\begin\{align\*\}                                         {} 
\\end\{align\*\}                                           {}  
\\begin\{alignat\}\{[^}]*\}                                {} 
\\end\{alignat\}                                           {}  
\\begin\{alignat\*\}\{[^}]*\}                              {} 
\\end\{alignat\*\}                                         {}  
\\begin\{aligned\}                                         {} 
\\end\{aligned\}                                           {}  
\\begin\{alignedat\}\{[^}]*\}                              {} 
\\end\{alignedat\}                                         {}  
\\begin\{array\}\{[^}]*\}                                  {} 
\\end\{array\}                                             {}  
\\begin\{eqnarray\}                                        {} 
\\end\{eqnarray\}                                          {}  
\\begin\{eqnarray\*\}                                      {} 
\\end\{eqnarray\*\}                                        {}  
\\begin\{equation\}                                        {} 
\\end\{equation\}                                          {}  
\\begin\{equation\*\}                                      {} 
\\end\{equation\*\}                                        {}  
\\begin\{gather\}                                          {} 
\\end\{gather\}                                            {}  
\\begin\{gather\*\}                                        {} 
\\end\{gather\*\}                                          {}  
\\begin\{gathered\}                                        {} 
\\end\{gathered\}                                          {}  
\\begin\{multline\}                                        {} 
\\end\{multline\}                                          {}  
\\begin\{multline\*\}                                      {} 
\\end\{multline\*\}                                        {}  
\\begin\{split\}                                           {} 
\\end\{split\}                                             {}  
\\begin\{subarray\}\{[^}]*\}                               {} 
\\end\{subarray\}                                          {}  

 /* number and variable */

[0-9]+\.[0-9]*    { return ret_float(yytext, LVAL);         }
[0-9]+            { return ret_num(yytext, LVAL);           }
[a-z]             { return ret_var(yytext, LVAL, 'a');      }
[A-Z]             { return ret_var(yytext, LVAL, 'A' - 26); }


 /* omit undefined control sequence */

\\[a-zA-Z]+                                                {} 
&                        { /* ignore when not in matrix. */ }
 
 /* escaped symbols */

.                  { trace(YYDEBUG, "`%s' esc.\n", yytext); }
%%

unsigned int ign_stack = 0;
unsigned int mat_stack = 0;

/*
Symbol ID Space:
	0 ... S_N - 1:                         tokens
	S_N ... S_N + 51:                      a - Z 
	S_N + 52 ... MAX_BRW_SYMBOL_ID_SZ - 1: number 
	S_BIGNUM:                              big number
(the first big number is not far from 65303)
*/

int ret_var(char *text, struct tex_tr **tr, char offset)
{
	int c = (int)(text[0] - offset);
	*tr = tex_tr_alloc(S_N + c, T_VAR);

	return VAR;
}

static uint32_t num2space(uint32_t n)
{
	if (n >= MAX_BRW_SYMBOL_ID_SZ - S_N - 52) 
		n = S_BIGNUM;
	else 
		n = n + S_N + 52;
	return n;
}

int ret_num(char *text, struct tex_tr **tr)
{
	uint32_t n;
	sscanf(text, "%u", &n);

	if (n == 0) {
		*tr = tex_tr_alloc(S_ZERO, T_ZERO); 
		return ZERO;
	} else if (n == 1) {
		*tr = tex_tr_alloc(S_ONE, T_ONE); 
		return ONE;
	} else {
		n = num2space(n);
		*tr = tex_tr_alloc(n, T_NUM); 
		return NUM;
	}
}

int ret_float(char *text, struct tex_tr **tr)
{
	struct tex_tr  *tr_decimal, *tr_intger;
	uint32_t       decimal, intger;

	if (strlen(text) < MAX_FLOAT_NUM_LEN) {
		sscanf(text, "%u.%u", &intger, &decimal);
		
		*tr = tex_tr_alloc(S_FLOAT, T_FLOAT);
		tr_intger = tex_tr_alloc(num2space(intger), T_NUM); 
		tr_decimal = tex_tr_alloc(num2space(decimal), T_NUM); 
		
		tex_tr_attatch(*tr, tr_intger);
		tex_tr_attatch(*tr, tr_decimal);

		return NUM;
	} else {
		trace(LIMIT, "%s is too long a float number (len=%d).\n", 
		      text, strlen(text));

		*tr = tex_tr_alloc(S_ZERO, T_ZERO); 
		return ZERO;
	}
}
	
int ret_hack(int symbol, int token, int ret,
             char *text, struct tex_tr **tr) { 
	struct tex_tr  *tr_a, *tr_b;
	uint32_t       a, b, l = strlen(text);

	a = (uint32_t)(text[l - 2] - '0');
	b = (uint32_t)(text[l - 1] - '0');

	*tr = tex_tr_alloc(symbol, token);
	tr_a = tex_tr_alloc(num2space(a), T_NUM);
	tr_b = tex_tr_alloc(num2space(b), T_NUM);

	tex_tr_attatch(*tr, tr_a);
	tex_tr_attatch(*tr, tr_b);

	return ret;
}

int bgn_mat()
{
	mat_stack ++;
	return _BEGIN_MAT;
}

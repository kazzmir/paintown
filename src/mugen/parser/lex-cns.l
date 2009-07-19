%{
#include <string.h>
#include "parse-cns.hh"

// #define yydebug printf("yytext is %s at %d\n", yytext, __LINE__);
#define yydebug ;

#define LEX_PREFIX cns
// #define LEX_VAL(p) LEX_PREFIX##p
#define LEX_VAL(p) cns##p

%}

%option yylineno

%%

;[^\n\r]*[\n\r] { yydebug; return COMMENT; }

[\n\r] { yydebug; return LINE_END; }

\[ { return LBRACKET; }
\] { return RBRACKET; }

\"[^"\n\r]*[^\\]\" { yydebug; LEX_VAL(lval).stringValue = strdup(yytext); return QUOTESTRING; }

([-+])?([0-9]+|[0-9]*\.[0-9]+) { yydebug; LEX_VAL(lval).numberValue = atof(yytext); return NUMBER; }

[ \t]* { ; }

. { return yytext[0]; }
%%

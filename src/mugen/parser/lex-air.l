%{
#include <string.h>
#include "parse-air.hh"

// #define yydebug printf("yytext is %s at %d\n", yytext, __LINE__);
#define yydebug ;

#define LEX_PREFIX air
// #define LEX_VAL(p) LEX_PREFIX##p
#define LEX_VAL(p) air##p

int airwrap(void){
	return yywrap();
}

%}

%option yylineno

%%

;[^\n\r]*[\n\r] { yydebug; return COMMENT; }

[\n\r] { yydebug; return LINE_END; }

\[ { return LBRACKET; }
\] { return RBRACKET; }

\"[^"\n\r]*[^\\]\" { yydebug; LEX_VAL(lval).stringValue = strdup(yytext); return QUOTESTRING; }

(?i:begin) { yydebug; return AIR_BEGIN; }
(?i:action) { yydebug; return AIR_ACTION; }

([a-zA-Z_][a-zA-Z0-9_\.-]*)([ ][XYxy])? { yydebug; LEX_VAL(lval).stringValue = strdup(yytext); return IDENTIFIER; }

([-+])?([0-9]+|[0-9]*\.[0-9]+) { yydebug; LEX_VAL(lval).numberValue = atof(yytext); return NUMBER; }

[ \t]* { ; }

. { return yytext[0]; }
%%

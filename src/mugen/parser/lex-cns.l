%{
#include <string.h>
#include "parse-cns.hh"

// #define yydebug printf("yytext is %s at %d\n", yytext, __LINE__);
#define yydebug ;

#define LEX_PREFIX cns
// #define LEX_VAL(p) LEX_PREFIX##p
#define LEX_VAL(p) cns##p

// (d|D)(a|A)(t|T)(a|A) { return CNS_DATA; }
// (s|S)(t|T)(a|A)(t|T)(e|E)(d|D)(e|E)(f|F) { return CNS_STATEDEF; }
// (s|S)(t|T)(a|A)(t|T)(e|E) { return CNS_STATE; }

// \[\s*(s|S)(t|T)(a|A)(t|T)(e|E)\s+\d+\s*(,[^\]]*)?\] {

%}

%option yylineno

%%

;[^\n\r]*[\n\r] { yydebug; return COMMENT; }

[\n\r] { yydebug; return LINE_END; }

\[ { yydebug; return LBRACKET; }
\] { yydebug; return RBRACKET; }

\"[^"\n\r]*[^\\]\" { yydebug; LEX_VAL(lval).stringValue = strdup(yytext); return QUOTESTRING; }

\[[ \t]*(s|S)(t|T)(a|A)(t|T)(e|E)[ \t]*-?[0-9]+(,[^\]]*)?\] {
	yydebug;
	LEX_VAL(lval).stringValue = strdup(yytext);
	return CNS_STATE;
}

(s|S)(t|T)(a|A)(t|T)(e|E)(d|D)(e|E)(f|F) { return CNS_STATEDEF; }

([-+])?([0-9]+|[0-9]*\.[0-9]+) { yydebug; LEX_VAL(lval).numberValue = atof(yytext); return NUMBER; }

!= { return CNS_NOTEQ; }
>= { return CNS_GREATERTHANEQ; }
\<= { return CNS_LESSTHANEQ; }
> { return CNS_GREATERTHAN; }
\< { return CNS_LESSTHAN; }

M { return CNS_MISS; }
D { return CNS_DODGE; }

&& { return CNS_AND; }
\|\| { return CNS_OR; }

[a-zA-Z_][a-zA-Z0-9_]* { yydebug; LEX_VAL(lval).stringValue = strdup(yytext); return IDENTIFIER; }

[ \t]* { ; }

. { return yytext[0]; }
%%

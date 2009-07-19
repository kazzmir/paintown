%{
#include <string.h>
#include "parse-air.hh"

// #define yydebug printf("yytext is %s at %d\n", yytext, __LINE__);
#define yydebug ;

#define LEX_PREFIX air
// #define LEX_VAL(p) LEX_PREFIX##p
#define LEX_VAL(p) air##p

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
(?i:clsn2default) { return AIR_COLLISION_DEFENSE_DEFAULT; }
(?i:clsn2) { return AIR_COLLISION_DEFENSE; }
(?i:clsn1default) { return AIR_COLLISION_ATTACK_DEFAULT; }
(?i:clsn1) { return AIR_COLLISION_ATTACK; }

(?i:loopstart) { return AIR_LOOPSTART; }

H { return AIR_HORIZONTAL; }
V { return AIR_VERTICAL; }
VH|HV { return AIR_VERTICAL_HORIZONTAL; }

AS[0-9]+D[0-9]+ { return AIR_ALPHA_BLEND; }

A { return AIR_COLOR_ADDITION; }
S { return AIR_COLOR_SUBTRACT; }

([-+])?([0-9]+|[0-9]*\.[0-9]+) { yydebug; LEX_VAL(lval).numberValue = atof(yytext); return NUMBER; }

[ \t]* { ; }

. { return yytext[0]; }
%%

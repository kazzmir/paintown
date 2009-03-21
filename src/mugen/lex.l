%{
#include <string.h>
#include "parse.hh"

// #define yydebug printf("yytext is %s at %d\n", yytext, __LINE__);
#define yydebug ;
%}

%option yylineno

%%

;[^\n\r]*[\n\r] { yydebug; return COMMENT; }

[\n\r] { yydebug; return LINE_END; }

\[[^0-9][^]\n\r]*[^\\]\] { yydebug; yylval.stringValue = strdup(yytext); return BRACKETSTRING; }

\"[^"\n\r]*[^\\]\" { yydebug; yylval.stringValue = strdup(yytext); return QUOTESTRING; }

=|<|>|<=|>=|!= { yydebug; yylval.stringValue = strdup(yytext); return COMPARISON_OPERATOR; }

([a-zA-Z_][a-zA-Z0-9_\.-]*)([ ][XYxy])? { yydebug; yylval.stringValue = strdup(yytext); return IDENTIFIER; }

([-+])?([0-9]+|[0-9]*\.[0-9]+) { yydebug; yylval.numberValue = atof(yytext); return NUMBER; }

[ \t]* { ; }

. { return yytext[0]; }
%%

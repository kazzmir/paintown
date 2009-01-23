%{
#include <string.h>
#include "parse.hh"
%}

%option yylineno

%%

;[^\n\r]*[\n\r] { return COMMENT; }

[\n\r] { return LINE_END; }

\[[^0-9][^]\n\r]*[^\\]\] { yylval.stringValue = strdup(yytext); return BRACKETSTRING; }

\"[^"\n\r]*[^\\]\" { yylval.stringValue = strdup(yytext); return QUOTESTRING; }

=|<|>|<=|>=|!= { yylval.stringValue = strdup(yytext); return COMPARISON_OPERATOR; }

([a-zA-Z_][a-zA-Z0-9_\.-]*)([ ][XYxy])? { yylval.stringValue = strdup(yytext); return IDENTIFIER; }

-?([0-9]+|[0-9]*\.[0-9]+) { yylval.numberValue = atof(yytext); return NUMBER; }

[ \t]* { ; }

&&&&&&&&&&&&&&&&&&&&&&&&&&&&: { *(char*) 0 = 0; REJECT; }

. { return yytext[0]; }
%%

char * dont_care = (char*) yyunput;
char * dont_care2 = (char*) input;
char * dont_care3 = (char*) yy_flex_realloc;

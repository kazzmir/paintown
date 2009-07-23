%{
#include <string.h>
#include "parse-def.hh"

// #define yydebug printf("yytext is %s at %d\n", yytext, __LINE__);
#define yydebug ;

#define LEX_PREFIX def
// #define LEX_VAL(p) LEX_PREFIX##p
#define LEX_VAL(p) def##p

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

(i|I)(n|N)(f|F)(o|O) { yydebug; return DEF_INFO; }
(d|D)(a|A)(t|T)(a|A) { yydebug; return DEF_DATA; }
(f|F)(i|I)(l|L)(e|E)(s|S) { yydebug; return DEF_FILES; }
(a|A)(r|R)(c|C)(a|A)(d|D)(e|E) { yydebug; return DEF_ARCADE; }

(s|S)(c|C)(e|E)(n|N)(e|E) { yydebug; return DEF_SCENE; }
(s|S)(c|C)(e|E)(n|N)(e|E)(d|D)(e|E)(f|F) { yydebug; return DEF_SCENEDEF; }

(b|B)(e|E)(g|G)(i|I)(n|N) { yydebug; return DEF_BEGIN; }
(a|A)(c|C)(t|T)(i|I)(o|O)(n|N) { yydebug; return DEF_ACTION; }
(l|L)(o|O)(o|O)(p|P)(s|S)(t|T)(a|A)(r|R)(t|T) { return DEF_LOOPSTART; }

(c|C)(a|A)(m|M)(e|E)(r|R)(a|A) { yydebug; return DEF_CAMERA; }
(p|P)(l|L)(a|A)(y|Y)(e|E)(r|R)(i|I)(n|N)(f|F)(o|O) { yydebug; return DEF_PLAYERINFO; }
(s|S)(c|C)(a|A)(l|L)(i|I)(n|N)(g|G) { yydebug; return DEF_SCALING; }
(r|R)(e|E)(f|F)(l|L)(e|E)(c|C)(t|T)(i|I)(o|O)(n|N) { yydebug; return DEF_REFLECTION; }
(b|B)(o|O)(u|U)(n|N)(d|D) { yydebug; return DEF_BOUND; }
(s|S)(t|T)(a|A)(g|G)(e|E)(i|I)(n|N)(f|F)(o|O) { yydebug; return DEF_STAGEINFO; }
(s|S)(h|H)(a|A)(d|D)(o|O)(w|W) { yydebug; return DEF_SHADOW; }
(m|M)(u|U)(s|S)(i|I)(c|C) { yydebug; return DEF_MUSIC; }

(b|B)(g|G)(d|D)(e|E)(f|F) { yydebug; return DEF_BGDEF; }
\[(b|B)(g|G).*\] { yydebug; LEX_VAL(lval).stringValue = strdup(yytext);  return DEF_BG; }
(b|B)(g|G)(c|C)(t|T)(r|R)(l|L)(d|D)(e|E)(f|F) { yydebug; return DEF_BGCTRLDEF; }
\[(b|B)(g|G)(c|C)(t|T)(r|R)(l|L).*\] { yydebug; return DEF_BGCTRL; }

(h|H) { return DEF_HORIZONTAL; }
(v|V) { return DEF_VERTICAL; }
(vh|hv|VH|HV) { return DEF_VERTICAL_HORIZONTAL; }

(as|AS)[0-9]+(d|D)[0-9]+ { return DEF_ALPHA_BLEND; }

(a|A) { return DEF_COLOR_ADDITION; }
(s|S) { return DEF_COLOR_SUBTRACT; }

([-+])?([0-9]+|[0-9]*\.[0-9]+) { yydebug; LEX_VAL(lval).numberValue = atof(yytext); return NUMBER; }

[a-zA-Z_\\\/\_\-][a-zA-Z\\\/\_\-0-9_]* { yydebug; LEX_VAL(lval).stringValue = strdup(yytext); return IDENTIFIER; }

[ \t]* { ; }

. { return yytext[0]; }
%%

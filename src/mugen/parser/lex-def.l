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

/*

;[^\n\r]*[\n\r] { yydebug; return COMMENT; }
*/

%}

%x xCOMMENT
%option yylineno

%%

; { yydebug; BEGIN(xCOMMENT); }

<xCOMMENT>. { ; }

<xCOMMENT>[\n\r] { BEGIN(INITIAL); return COMMENT; }

[\n\r] { yydebug; return LINE_END; }

\[ { yydebug; return LBRACKET; }
\] { yydebug; return RBRACKET; }

\"[^"\n\r]*[^\\]\" { yydebug; LEX_VAL(lval).stringValue = strdup(yytext); return QUOTESTRING; }

<xCOMMENT><<EOF>> {
  static char newline[2] = "\n";
  // printf("Switching to a new buffer\n");
  // we have to close this stuff somehow 
  int files[2];
  extern int parser_get_pipe(int[2]);
  if (parser_get_pipe(files) == -1){
    perror("lex switch buffer");
    exit(-1);
  }
  write(files[1], "\n", 1);
  close(files[1]);
  FILE * buffer = fdopen(files[0], "r");
  YY_BUFFER_STATE state = yy_create_buffer(buffer, YY_BUF_SIZE);
  // yypush_buffer_state(state);

  // yy_flush_buffer(state);
  yy_switch_to_buffer(state);
}
\[(b|B)(g|G).*\] { yydebug; LEX_VAL(lval).stringValue = strdup(yytext);  return DEF_BG; }
\[(b|B)(g|G)(c|C)(t|T)(r|R)(l|L).*\] { yydebug; LEX_VAL(lval).stringValue = strdup(yytext); return DEF_BGCTRL; }

(h|H) { return DEF_HORIZONTAL; }
(v|V) { return DEF_VERTICAL; }
(vh|hv|VH|HV) { return DEF_VERTICAL_HORIZONTAL; }

(as|AS)[0-9]+(d|D)[0-9]+ { return DEF_ALPHA_BLEND; }

(a|A) { return DEF_COLOR_ADDITION; }
(s|S) { return DEF_COLOR_SUBTRACT; }

([-+])?([0-9]+|[0-9]*\.[0-9]+) { yydebug; LEX_VAL(lval).numberValue = atof(yytext); return NUMBER; }

[a-zA-Z_\\\/\_\-][a-zA-Z\\\/\_\-0-9_]* { yydebug; LEX_VAL(lval).stringValue = strdup(yytext); return IDENTIFIER; }

[ \t]* { ; }

. { yydebug; return yytext[0]; }
%%

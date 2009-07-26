%{
#include <string.h>
#include "parse-air.hh"

// #define yydebug printf("yytext is %s at %d\n", yytext, __LINE__);
#define yydebug ;

#define LEX_PREFIX air
// #define LEX_VAL(p) LEX_PREFIX##p
#define LEX_VAL(p) air##p

/*
only flex 2.5.25 can handle (?). windows can't do it so just give up
(?i:begin) { yydebug; return AIR_BEGIN; }
(?i:action) { yydebug; return AIR_ACTION; }
(?i:clsn2default) { return AIR_COLLISION_DEFENSE_DEFAULT; }
(?i:clsn2) { return AIR_COLLISION_DEFENSE; }
(?i:clsn1default) { return AIR_COLLISION_ATTACK_DEFAULT; }
(?i:clsn1) { return AIR_COLLISION_ATTACK; }
(?i:loopstart) { return AIR_LOOPSTART; }
*/

%}

%x xCOMMENT
%option yylineno

DIGIT [0-9]

%%

; { yydebug; BEGIN(xCOMMENT); }

<xCOMMENT>. { ; }

<xCOMMENT>[\n\r] { BEGIN(INITIAL); return COMMENT; }

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


[\n\r] { yydebug; return LINE_END; }

\[ { return LBRACKET; }
\] { return RBRACKET; }

\"[^"\n\r]*[^\\]\" { yydebug; LEX_VAL(lval).stringValue = strdup(yytext); return QUOTESTRING; }

(b|B)(e|E)(g|G)(i|I)(n|N) { yydebug; return AIR_BEGIN; }
(a|A)(c|C)(t|T)(i|I)(o|O)(n|N) { yydebug; return AIR_ACTION; }
(c|C)(l|L)(s|S)(n|N)(2|2)(d|D)(e|E)(f|F)(a|A)(u|U)(l|L)(t|T) { return AIR_COLLISION_DEFENSE_DEFAULT; }
(c|C)(l|L)(s|S)(n|N)(2|2) { return AIR_COLLISION_DEFENSE; }
(c|C)(l|L)(s|S)(n|N)(1|1)(d|D)(e|E)(f|F)(a|A)(u|U)(l|L)(t|T) { return AIR_COLLISION_ATTACK_DEFAULT; }
(c|C)(l|L)(s|S)(n|N)(1|1) { return AIR_COLLISION_ATTACK; }
(l|L)(o|O)(o|O)(p|P)(s|S)(t|T)(a|A)(r|R)(t|T) { return AIR_LOOPSTART; }

(h|H) { return AIR_HORIZONTAL; }
(v|V) { return AIR_VERTICAL; }
((v|V)(h|H))|((h|H)(v|V)) { return AIR_VERTICAL_HORIZONTAL; }

(a|A)(s|S){DIGIT}+(d|D){DIGIT}+ { return AIR_ALPHA_BLEND; }
(a{DIGIT}|A{DIGIT}) { return AIR_ALPHA_BLEND_SHORTHAND; }

(a|A) { return AIR_COLOR_ADDITION; }
(s|S) { return AIR_COLOR_SUBTRACT; }

([-+])?({DIGIT}+|{DIGIT}*\.{DIGIT}+) { yydebug; LEX_VAL(lval).numberValue = atof(yytext); return NUMBER; }

[ \t]* { ; }

. { return yytext[0]; }
%%

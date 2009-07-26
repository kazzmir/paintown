%{
#include <string.h>
#include "parse-def.hh"

// #define yydebug printf("yytext is %s at %d\n", yytext, __LINE__);
#define yydebug ;

#define LEX_PREFIX def
// #define LEX_VAL(p) LEX_PREFIX##p
#define LEX_VAL(p) def##p
  
FILE * def_open_file = 0;
YY_BUFFER_STATE def_buffer_state = 0;

void def_lex_reset(){
    yylineno = 1;
    if (def_buffer_state){
    	yy_delete_buffer(def_buffer_state);
	def_buffer_state = 0;
    }
    if (def_open_file != 0){
    	fclose(def_open_file);
	def_open_file = 0;
    }
    // deflex_destroy();
}

// (d|D)(a|A)(t|T)(a|A) { return CNS_DATA; }
// (s|S)(t|T)(a|A)(t|T)(e|E)(d|D)(e|E)(f|F) { return CNS_STATEDEF; }
// (s|S)(t|T)(a|A)(t|T)(e|E) { return CNS_STATE; }

// \[\s*(s|S)(t|T)(a|A)(t|T)(e|E)\s+\d+\s*(,[^\]]*)?\] {

/*

;[^\n\r]*[\n\r] { yydebug; return COMMENT; }
FILENAME_CHAR [^\/, \t\n\r]
*/

%}

%x xCOMMENT
%option yylineno

DIGIT [0-9]
TEXT (t|T)(e|E)(x|X)(t|T)
SPACE [ \t]
FILENAME_CHAR [^\/,=;\n\r]
FILENAME_CHAR2 [^\/, \t\n\r]

%%

; { yydebug; BEGIN(xCOMMENT); }

<xCOMMENT>. { ; }

<xCOMMENT>[\n\r] { BEGIN(INITIAL); return COMMENT; }

[\n\r] { yydebug; return LINE_END; }

\[ { yydebug; return LBRACKET; }
\] { yydebug; return RBRACKET; }

\"[^"\n\r]*[^\\]?\" { yydebug; LEX_VAL(lval).stringValue = strdup(yytext); return QUOTESTRING; }

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
  def_open_file = fdopen(files[0], "r");
  def_buffer_state = yy_create_buffer(def_open_file, YY_BUF_SIZE);

  /* push not available in flex 2.5.4a */
  // yypush_buffer_state(state);
  // yy_flush_buffer(state);

  yy_switch_to_buffer(def_buffer_state);
}

\[(b|B)(g|G)(c|C)(t|T)(r|R)(l|L).*\] { yydebug; LEX_VAL(lval).stringValue = strdup(yytext); return DEF_BGCTRL; }
\[(b|B)(g|G).*\] { yydebug; LEX_VAL(lval).stringValue = strdup(yytext);  return DEF_BG; }

\[(s|S)(e|E)(l|L)(e|E)(c|C)(t|T)(b|B)(g|G)(c|C)(t|T)(r|R)(l|L).*\] { yydebug; LEX_VAL(lval).stringValue = strdup(yytext); return DEF_SELECTBGCTRL; }
\[(s|S)(e|E)(l|L)(e|E)(c|C)(t|T)(b|B)(g|G).*\] { yydebug; LEX_VAL(lval).stringValue = strdup(yytext);  return DEF_SELECTBG; }

(h|H) { return DEF_HORIZONTAL; }
(v|V) { return DEF_VERTICAL; }
((v|V)(h|H))|((h|H)(v|V)) { return DEF_VERTICAL_HORIZONTAL; }

(a|A)(s|S){DIGIT}+(d|D){DIGIT}+ { return DEF_ALPHA_BLEND; }
(a{DIGIT}|A{DIGIT}) { return DEF_ALPHA_BLEND_SHORTHAND; }

(l|L)(o|O)(o|O)(p|P)(s|S)(t|T)(a|A)(r|R)(t|T) { return DEF_LOOPSTART; }

(b|B)(l|L)(a|A)(n|N)(k|K) { return DEF_BLANK; }

^=.*$ { return COMMENT; }
^-.*$ { return COMMENT; }

(a|A) { return DEF_COLOR_ADDITION; }
(s|S) { return DEF_COLOR_SUBTRACT; }

^.*{TEXT}{SPACE}*=.*$ { yydebug; LEX_VAL(lval).stringValue = strdup(yytext); return DEF_TEXT; }

{FILENAME_CHAR}+(\/{FILENAME_CHAR}+)+ { yydebug; LEX_VAL(lval).stringValue = strdup(yytext); return DEF_FILENAME; }

([-+])?({DIGIT}+|{DIGIT}*\.{DIGIT}+) { yydebug; LEX_VAL(lval).numberValue = atof(yytext); return NUMBER; }

[a-zA-Z_\\\/\_\-][a-zA-Z\\\/\_\-0-9_]* { yydebug; LEX_VAL(lval).stringValue = strdup(yytext); return IDENTIFIER; }

[ \t]* { ; }

. { yydebug; return yytext[0]; }
%%

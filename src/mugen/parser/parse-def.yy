%{
#include <list>
#include <stdio.h>
#include <string.h>
#include <string>

#include "../ast/Configuration.h"
#include "../ast/Value.h"
#include "../ast/Modifier.h"
#include "util/system.h"
#include "globals.h"
#include "parse-exception.h"

#define bugon(a) if ((a)){ printf("parsing bug at %s:%d\n", __FILE__, __LINE__); }

extern "C" int yylex(void);
extern "C" int yyerror(const char *);

static Ast::Configuration *configuration;
static Ast::Section *currentSection;
static Ast::Variable *currentLhs;
static std::list<Ast::Value *> *currentRhs;
static Ast::Value *currentValue;
static std::list<Ast::Modifier *> *currentModifiers;

%}

%union {
    double numberValue;
    char *stringValue;
}
%token <stringValue> QUOTESTRING 
%token <numberValue> NUMBER 
%token <stringValue> IDENTIFIER
%token LBRACKET
%token RBRACKET

%token DEF_TEXT
       DEF_LOOPSTART
       DEF_HORIZONTAL
       DEF_VERTICAL
       DEF_VERTICAL_HORIZONTAL
       DEF_ALPHA_BLEND
       DEF_ALPHA_BLEND_SHORTHAND
       DEF_COLOR_ADDITION
       DEF_COLOR_SUBTRACT
%token <stringValue> DEF_BG
%token <stringValue> DEF_BGCTRL
%token <stringValue> DEF_SELECTBG
%token <stringValue> DEF_SELECTBGCTRL
       

%token COMMENT
%token LINE_END

%error-verbose
%%
file: 
    end_or_comment file
    | stuff
    |
    ;

stuff:
    line ends stuff
    | line ends
    | line

ends:
    end_or_comment ends
    | end_or_comment

line:
    unquoted_text
    | section1
    | section2
    | section3
    | section4
    | section5
    | section6
    | section7
    | section8
    | section9
    | section10
    | bg
    | bgctrl
    | selectbg
    | selectbgctrl
    | NUMBER ',' NUMBER ',' NUMBER ',' NUMBER ',' NUMBER maybe_flip
    | DEF_LOOPSTART
    | assignment
    | assign_none
    ;

assignment:
    lhs '=' rhs

lhs:
    variable
    | variable '(' value ')'

rhs:
   expression_list
   
assign_none:
    lhs '='

expression:
    variable '=' value
    | expression1

expression1:
    '(' expression ')'
    | '!' value
    | value
    | multiple_values

expression_list:
    expression
    | expression ',' expression_list

multiple_values:
    value
    | value multiple_values

value:
    NUMBER
    | QUOTESTRING { free($1); }
    | variable '(' expression_list ')'
    | variable
    ;

variable:
     IDENTIFIER '.' variable { free($1); }
     | IDENTIFIER { free($1); }

end_or_comment:
    LINE_END 
  | COMMENT
  ;

unquoted_text:
    DEF_TEXT;
    
section1:
    LBRACKET IDENTIFIER RBRACKET { free($2); }

section2:
    LBRACKET NUMBER RBRACKET;

section3:
    LBRACKET IDENTIFIER IDENTIFIER RBRACKET { free($2); }

section4: 
    LBRACKET IDENTIFIER NUMBER RBRACKET { free($2); }
    
section5:
    LBRACKET NUMBER NUMBER RBRACKET;

section6:
    LBRACKET IDENTIFIER IDENTIFIER IDENTIFIER RBRACKET { free($2); free($3); free($4); }

section7:
    LBRACKET IDENTIFIER NUMBER IDENTIFIER RBRACKET { free($2); free($4); }

section8:
    LBRACKET IDENTIFIER IDENTIFIER NUMBER RBRACKET { free($2); free($3); }
    
section9:
    LBRACKET IDENTIFIER NUMBER NUMBER RBRACKET { free($2); }

section10:
    LBRACKET NUMBER NUMBER NUMBER RBRACKET;
    
ident_num:
    IDENTIFIER RBRACKET { free($1); }
    | NUMBER RBRACKET
    | IDENTIFIER NUMBER RBRACKET { free($1); }
    | NUMBER IDENTIFIER RBRACKET { free($2); }

maybe_flip:
   | ',' flip
   | ',' flip ',' color_sub
   | ',' ',' color_sub
   |

flip:
    DEF_HORIZONTAL
    | DEF_VERTICAL
    | DEF_VERTICAL_HORIZONTAL
    | ','
    
color_sub:
    DEF_COLOR_ADDITION
    | DEF_COLOR_SUBTRACT
    | DEF_ALPHA_BLEND
    | DEF_ALPHA_BLEND_SHORTHAND
    | ','
    
    
/* Implement properly later */
bg:
    DEF_BG {
	Global::debug(0) << "Got Bg: " << $1 << std::endl;
	free($1);
    };
    
selectbg:
    DEF_SELECTBG {
	Global::debug(0) << "Got Select Bg: " << $1 << std::endl;
	free($1);
    };

bgctrl:
    DEF_BGCTRL{
	Global::debug(0) << "Got BgCtrl: " << $1 << std::endl;
	free($1);
    };
    
selectbgctrl:
    DEF_SELECTBGCTRL{
	Global::debug(0) << "Got Select BgCtrl: " << $1 << std::endl;
	free($1);
    };    
%%

int yyerror(const char *msg) {
    extern int deflineno;
    extern char *deftext;
    printf("Parse error at line %d: %s at \n  \'%s\'\n", deflineno, msg, deftext);
    /*if (yytext)
	for (int i = 0; i < strlen(yytext); i++) {
	    printf("%d, ", yytext[i]);
	}*/
    return 0;
}

#include "parsers.h"
    
extern "C" void def_lex_reset();

static void reset(){
    def_lex_reset();
}

void Mugen::parseDef(const std::string & filename) throw (Mugen::ParserException) {
    extern FILE * defin;

    reset();

    if (!System::readableFile(filename)){
    	throw ParserException(std::string("Cannot open ") + filename + " for reading");
    }

    defin = fopen(filename.c_str(), "r");
    if (defin == NULL){
    	throw ParserException(std::string("Could not open ") + filename);
    }
    int success = yyparse();
    fclose(defin);
    defin = 0;
    if (success == 0){
        Global::debug(0) << "Successfully parsed " << filename << std::endl;
    } else {
    	throw ParserException(std::string("Failed to parse ") + filename);
    }
}

#if 0
Ast::Configuration * mugenParse(std::string filename){
    /* lex input thing */
    extern FILE * yyin;

    /* todo: delete all this crap */
    if (configuration != NULL){
    	delete configuration;
    }

    configuration = new Ast::Configuration();
    currentSection = NULL;
    currentLhs = NULL;
    currentRhs = NULL;
    currentValue = NULL;
    currentModifiers = NULL;

    /* the lex reader thing */
    yyin = fopen(filename.c_str(), "r");
    yyparse();
    fclose(yyin);

    return configuration;
}
#endif

#undef bugon

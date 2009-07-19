%{
#include <list>
#include <stdio.h>
#include <string.h>
#include <string>

#include "../ast/Configuration.h"
#include "../ast/Value.h"
#include "../ast/Modifier.h"
#include "globals.h"

#define bugon(a) if ((a)){ printf("parsing bug at %s:%d\n", __FILE__, __LINE__); }

extern "C" int yylex(void);
extern "C" int yyerror(const char *);
extern "C" int airget_lineno();

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
%token AIR_BEGIN
%token AIR_ACTION
%token LBRACKET
%token RBRACKET

%token AIR_COLLISION_ATTACK_DEFAULT
%token AIR_COLLISION_ATTACK
%token AIR_COLLISION_DEFENSE_DEFAULT
%token AIR_COLLISION_DEFENSE

%token AIR_LOOPSTART

%token COMMENT
%token LINE_END

%token AIR_HORIZONTAL
%token AIR_VERTICAL
%token AIR_VERTICAL_HORIZONTAL

%token AIR_ALPHA_BLEND

%token AIR_COLOR_ADDITION
%token AIR_COLOR_SUBTRACT

%error-verbose
%%
file: 
    line file
    | line

line:
    COMMENT 
    | LINE_END
    | section end_or_comment {
    	/*
    		bugon(configuration == NULL);
    		bugon(currentSection == NULL);
		configuration->getSections().push_back(currentSection);
		*/
	}
    | collision ':' NUMBER end_or_comment {
    }
    | collision_box ':' NUMBER end_or_comment {
    }
    | collision_box LBRACKET NUMBER RBRACKET '=' NUMBER ',' NUMBER ',' NUMBER ',' NUMBER end_or_comment{
    }
    | NUMBER ',' NUMBER ',' NUMBER ',' NUMBER ',' NUMBER maybe_flip end_or_comment
    | AIR_LOOPSTART end_or_comment
    ;

maybe_flip:
    ',' flip
    |

flip:
    AIR_HORIZONTAL
    | AIR_VERTICAL
    | AIR_VERTICAL_HORIZONTAL

collision:
    AIR_COLLISION_ATTACK_DEFAULT
    | AIR_COLLISION_DEFENSE_DEFAULT

collision_box:
    AIR_COLLISION_DEFENSE
    | AIR_COLLISION_ATTACK

end_or_comment:
    LINE_END 
  | COMMENT;

section:
    LBRACKET AIR_BEGIN AIR_ACTION NUMBER RBRACKET { 
        double value = $4;
	Global::debug(0) << "Read section number " << value << std::endl;
	currentSection = new Ast::Section();
	// currentSection->setName($1);
 	};

%%
// extern int yylineno;
extern char *yytext;

int yyerror(const char *msg) {
    printf("Parse error at line %d: %s at \'%s\'\n", airget_lineno(), msg, yytext);
    /*if (yytext)
	for (int i = 0; i < strlen(yytext); i++) {
	    printf("%d, ", yytext[i]);
	}*/
    return 0;
}

#include "parsers.h"

void Mugen::parseAir(const std::string & filename){
    extern FILE * airin;
    airin = fopen(filename.c_str(), "r");
    yyparse();
    fclose(airin);
    Global::debug(0) << "Successfully parsed " << filename << std::endl;
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

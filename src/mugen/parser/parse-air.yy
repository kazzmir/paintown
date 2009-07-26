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
       AIR_ACTION
       LBRACKET
       RBRACKET
       AIR_COLLISION_ATTACK_DEFAULT
       AIR_COLLISION_ATTACK
       AIR_COLLISION_DEFENSE_DEFAULT
       AIR_COLLISION_DEFENSE
       AIR_LOOPSTART
       COMMENT
       LINE_END
       AIR_HORIZONTAL
       AIR_VERTICAL
       AIR_VERTICAL_HORIZONTAL
       AIR_ALPHA_BLEND
       AIR_ALPHA_BLEND_SHORTHAND
       AIR_COLOR_ADDITION
       AIR_COLOR_SUBTRACT

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
    section {
    	/*
    		bugon(configuration == NULL);
    		bugon(currentSection == NULL);
		configuration->getSections().push_back(currentSection);
		*/
	}
    | collision ':' NUMBER {
    }
    | collision_box ':' NUMBER {
    }
    | collision_box LBRACKET NUMBER RBRACKET '=' NUMBER ',' NUMBER ',' NUMBER ',' NUMBER {
    }
    | NUMBER ',' NUMBER ',' NUMBER ',' NUMBER ',' NUMBER maybe_flip
    | AIR_LOOPSTART
    ;

maybe_flip:
   | ',' flip
   | ',' flip ',' color_sub
   | ',' ',' color_sub
   |

flip:
    AIR_HORIZONTAL
    | AIR_VERTICAL
    | AIR_VERTICAL_HORIZONTAL
    | ','
    
color_sub:
    AIR_COLOR_ADDITION
    | AIR_COLOR_SUBTRACT
    | AIR_ALPHA_BLEND
    | AIR_ALPHA_BLEND_SHORTHAND
    | ','

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
extern int airlineno;
extern char *airtext;

int yyerror(const char *msg) {
    printf("Parse error at line %d: %s at \'%s\'\n", airlineno, msg, airtext);
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
    int success = yyparse();
    fclose(airin);
    if (success == 0){
        Global::debug(0) << "Successfully parsed " << filename << std::endl;
    } else {
    	Global::debug(0) << "Failed to parse " << filename << std::endl;
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

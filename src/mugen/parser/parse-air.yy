%{
#include <list>
#include <stdio.h>
#include <string.h>
#include <string>

#include "../ast/Configuration.h"
#include "../ast/Value.h"
#include "../ast/Modifier.h"

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
%token AIR_ACTION
%token LBRACKET
%token RBRACKET

%token COMMENT
%token LINE_END

%error-verbose
%%
file: 
    line file
    | line

line:
    COMMENT 
    | LINE_END
    | section end_or_comment {
    		bugon(configuration == NULL);
    		bugon(currentSection == NULL);
		configuration->getSections().push_back(currentSection);
	}
    ;

end_or_comment:
    LINE_END 
  | COMMENT;

section:
    LBRACKET AIR_BEGIN AIR_ACTION NUMBER RBRACKET { 
        double value = $4;
	currentSection = new Ast::Section();
	// currentSection->setName($1);
 	};

%%
extern int yylineno;
extern char *yytext;

int yyerror(const char *msg) {
    printf("%d: %s at \'%s\'\n", yylineno, msg, yytext);
    /*if (yytext)
	for (int i = 0; i < strlen(yytext); i++) {
	    printf("%d, ", yytext[i]);
	}*/
    return 0;
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

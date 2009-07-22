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
%token LBRACKET
%token RBRACKET

%token CNS_DATA
       CNS_STATEDEF
       CNS_STATE
       CNS_AND
       CNS_OR
       CNS_NOTEQ
       CNS_GREATERTHANEQ
       CNS_LESSTHANEQ
       CNS_GREATERTHAN
       CNS_LESSTHAN
       CNS_MISS
       CNS_DODGE

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
    	/*
    		bugon(configuration == NULL);
    		bugon(currentSection == NULL);
		configuration->getSections().push_back(currentSection);
		*/
	}
    | assignment end_or_comment
    ;

assignment:
    lhs '=' rhs

lhs:
    variable
    | variable '(' value ')'

rhs:
   expression_list

expression:
    expression1 CNS_AND expression
    | expression1 CNS_OR expression
    | expression1

expression1:
    expression2 '=' expression1
    | expression2 CNS_NOTEQ expression1
    | expression2 CNS_GREATERTHANEQ expression1
    | expression2 CNS_LESSTHANEQ expression1
    | expression2 CNS_GREATERTHAN expression1
    | expression2 CNS_LESSTHAN expression1
    | expression2

expression2:
    | expression3 '+' expression2
    | expression3 '-' expression2
    | expression3

expression3:
    | expression4 '*' expression3
    | expression4 '/' expression3
    | expression4

expression4:
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
    | QUOTESTRING
    | variable '(' expression_list ')'
    | variable
    | miss_dodge
    ;

miss_dodge:
    CNS_MISS
    | CNS_MISS '-'
    | CNS_DODGE
    | CNS_DODGE '-'

variable:
     IDENTIFIER '.' variable
     | IDENTIFIER

end_or_comment:
    LINE_END 
  | COMMENT;

section:
    LBRACKET CNS_STATEDEF NUMBER RBRACKET
    | CNS_STATE
    | LBRACKET IDENTIFIER RBRACKET { 
        /*
        double value = $4;
	Global::debug(0) << "Read section number " << value << std::endl;
	*/
	currentSection = new Ast::Section();
	// currentSection->setName($1);
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

void Mugen::parseDef(const std::string & filename){
    extern FILE * defin;
    defin = fopen(filename.c_str(), "r");
    int success = yyparse();
    fclose(defin);
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

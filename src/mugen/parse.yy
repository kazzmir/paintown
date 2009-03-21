%{
#include <list>
#include <stdio.h>
#include <string.h>
#include <string>

#include "ast/Configuration.h"
#include "ast/Value.h"
#include "ast/Modifier.h"

#define bugon(a) if (!(a)){ printf("parsing bug at %s:%d\n", __FILE__, __LINE__); }

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
%token <stringValue> BRACKETSTRING 
%token <numberValue> NUMBER 
%token <stringValue> IDENTIFIER
%token <stringValue> COMMAND
%token <stringValue> COMPARISON_OPERATOR

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
    | section LINE_END {
    		bugon(configuration == NULL);
    		bugon(currentSection == NULL);
		configuration->getSections().push_back(currentSection);
	}
    | section COMMENT {
    		bugon(configuration == NULL);
    		bugon(currentSection == NULL);
		configuration->getSections().push_back(currentSection);
	}
    | assignment LINE_END {
    		bugon(currentSection == NULL);
    		bugon(currentLhs == NULL);
		currentSection->getKeyValueMap()[currentLhs] = *currentRhs;
	}
    | assignment COMMENT {
    		bugon(currentSection == NULL);
    		bugon(currentLhs == NULL);
		currentSection->getKeyValueMap()[currentLhs] = *currentRhs;
	};

section:
    BRACKETSTRING { 
	currentSection = new Ast::Section();
	currentSection->setName($1);
 	};

assignment:
    lhs COMPARISON_OPERATOR { currentRhs = new std::list<Ast::Value *>(); } rhs;

lhs: 
    variable;

variable:
    IDENTIFIER { currentLhs = new Ast::Variable($1); }
    | IDENTIFIER '(' NUMBER ')' { currentLhs = new Ast::Variable($1, (int)$3); }
    | IDENTIFIER '(' IDENTIFIER ')' { currentLhs = new Ast::Variable($1, $3); }

rhs: 
    valueList;

valueList:
    value ',' { currentRhs->push_back(currentValue); } valueList
    | ',' { currentRhs->push_back(NULL); } valueList
    | value { currentRhs->push_back(currentValue); };

value:
    NUMBER { currentValue = new Ast::Value($1); }
    | QUOTESTRING { currentValue = new Ast::Value($1); }
    | IDENTIFIER { currentValue = new Ast::Value(); currentValue->setIdentifier($1); }
    | modifiedIdentifier
    | expression
    | IDENTIFIER '(' valueList ')'
    | range;

range:
    '[' NUMBER ',' NUMBER ']' { currentValue = new Ast::Value($2, $4); }
    | '[' NUMBER ',' NUMBER ')' { currentValue = new Ast::Value($2, $4); }
    | '(' NUMBER ',' NUMBER ']' { currentValue = new Ast::Value($2, $4); }
    | '(' NUMBER ',' NUMBER ')' { currentValue = new Ast::Value($2, $4); }

modifiedIdentifier:
    modifiers { currentValue = new Ast::Value(); 
	currentValue->setSpecies(Ast::ModifiedIdentifier); } chainedIdentifiers
    | chainedIdentifiers

modifiers:
    modifier modifiers
    | modifier

modifier:
    '/' | '~' | '~' NUMBER | '$' | '>'

chainedIdentifiers:
    IDENTIFIER { printf("    %s\n", $1); } '+' chainedIdentifiers
    | IDENTIFIER { printf("    %s\n", $1); };

expression:
    | booleanExpression
    | mathExpression;

booleanExpression:
    value COMPARISON_OPERATOR value
    | '(' booleanExpression ')'
    | value '|' '|' value
    | value '&' '&' value
    | '!' value;

mathExpression:
    value '+' value
    | value '-' value
    | value '*' value
    | value '/' value
    | '(' mathExpression ')'

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

Ast::Configuration * mugenParse(std::string filename){
    /* lex input thing */
    extern FILE * yyin;
    configuration = NULL;
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

#undef bugon

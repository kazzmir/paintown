%{
#include <list>
#include <stdio.h>
#include <string.h>

#include "ast/Configuration.h"
#include "ast/Value.h"
#include "ast/Modifier.h"

extern "C" int yylex(void);
extern "C" int yyerror(const char *);

Configuration *configuration;
Section *currentSection;
Variable *currentLhs;
std::list<Value *> *currentRhs;
Value *currentValue;
std::list<Modifier *> *currentModifiers;

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
		configuration->getSections().push_back(currentSection);
	}
    | section COMMENT {
		configuration->getSections().push_back(currentSection);
	}
    | assignment LINE_END {
		currentSection->getKeyValueMap()[currentLhs] = *currentRhs;
	}
    | assignment COMMENT {
		currentSection->getKeyValueMap()[currentLhs] = *currentRhs;
	};

section:
    BRACKETSTRING { 
	currentSection = new Section();
	currentSection->setName($1);
 	};

assignment:
    lhs COMPARISON_OPERATOR { currentRhs = new std::list<Value *>(); } rhs;

lhs: 
    variable;

variable:
    IDENTIFIER { currentLhs = new Variable($1); }
    | IDENTIFIER '(' NUMBER ')' { currentLhs = new Variable($1, (int)$3); }
    | IDENTIFIER '(' IDENTIFIER ')' { currentLhs = new Variable($1, $3); }

rhs: 
    valueList;

valueList:
    value ',' { currentRhs->push_back(currentValue); } valueList
    | ',' { currentRhs->push_back(NULL); } valueList
    | value { currentRhs->push_back(currentValue); };

value:
    NUMBER { currentValue = new Value($1); }
    | QUOTESTRING { currentValue = new Value($1); }
    | IDENTIFIER { currentValue = new Value(); currentValue->setIdentifier($1); }
    | modifiedIdentifier
    | expression
    | IDENTIFIER '(' valueList ')'
    | range;

range:
    '[' NUMBER ',' NUMBER ']' { currentValue = new Value($2, $4); }
    | '[' NUMBER ',' NUMBER ')' { currentValue = new Value($2, $4); }
    | '(' NUMBER ',' NUMBER ']' { currentValue = new Value($2, $4); }
    | '(' NUMBER ',' NUMBER ')' { currentValue = new Value($2, $4); }

modifiedIdentifier:
    modifiers { currentValue = new Value(); 
	currentValue->setSpecies(ModifiedIdentifier); } chainedIdentifiers
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

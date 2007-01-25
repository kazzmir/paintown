#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <string>
#include "parser.h"

using namespace std;

token * current_token;

extern FILE * yyin;
extern int yylex();

void push( char * s ){
	current_token->inputs.push_back( s );
}

void destroyToken( token * head ){
	for ( vector<token*>::iterator it = head->tokens.begin(); it != head->tokens.end(); it++ ){
		destroyToken( *it );
	}
	delete head;
}

void startStack(){
	token * t = new token;
	current_token->tokens.push_back( t );
	t->parent = current_token;
	current_token = t;
}

void endStack(){
	if ( current_token->parent == NULL ){
		printf("Error in input\n");
		exit( 1 );
	}
	current_token = current_token->parent;
}

void printToken( token * f, string space ){
	vector< string > ::iterator it;
	for ( it = f->inputs.begin(); it != f->inputs.end(); it++ ){
		cout<< space << " " << *it << endl;;
	}
	vector< token * >::iterator xit;
	space += "  ";
	for ( xit = f->tokens.begin(); xit != f->tokens.end(); xit++ ){
		printToken( *xit, space );
	}

}

token * parseFile( string file ){
	yyin = fopen( file.c_str(), "r" );
	token * head = new token;
	head->parent = NULL;
	current_token = head;
	yylex();
	return head;
}

/*
int main(){
	yyin = fopen( "foobar", "r" );
	token head;
	head.parent = NULL;
	current_token = &head;
	yylex();

	printToken( &head, "" );
}
*/

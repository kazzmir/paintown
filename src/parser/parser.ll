%{
	void push( char * s );
	void startStack();
	void endStack();
%}

DIGIT [0-9]
ANY [0-9a-zA-Z]
%%

{ANY}+ push( yytext );
\".*\" push( yytext );
"(" startStack();
")" endStack();

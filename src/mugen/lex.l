/* lexer */

	int num_chars;

%%
.	++num_chars;
%%

	void * dont_care = yyunput;

/*
main(){
	yylex();
	printf("chars %d\n", num_chars);
}
*/

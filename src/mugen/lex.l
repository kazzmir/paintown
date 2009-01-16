/* lexer */

	int num_chars;

%%
.	++num_chars;
%%

	void * dont_care = yyunput;
	void * dont_care2 = input;

/*
main(){
	yylex();
	printf("chars %d\n", num_chars);
}
*/

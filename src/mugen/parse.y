/* any extra C stuff goes here, like #define or #include */
%{
extern int yylex(void);

void yyerror(char* d){
}
%}

/* the syntactic tokens */
%token kSOMETHING
       kSOMETHING_ELSE

/* the CFG */
%%
something : ';';
%%

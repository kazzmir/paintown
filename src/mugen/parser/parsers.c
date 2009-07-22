extern int yywrap(void);

/* lex wants to call airwrap which should have been #defined to yywrap but its not.
 * yywrap is defined in libfl.a
 */
int airwrap(void){
    return yywrap();
}

int cnswrap(void){
    return yywrap();
}

int cmdwrap(void){
    return yywrap();
}

int defwrap(void){
    return yywrap();
}

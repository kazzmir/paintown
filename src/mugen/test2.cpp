extern int yylex(void);
extern int yyparse(void);

#include "ast/Configuration.h"

extern Configuration *configuration;

int main(int argc, char **argv) {
    configuration = new Configuration();
    //yylex();
    yyparse();
    configuration->debugExplain();
    delete configuration;
    return 0;
}

/* this is just used to test that yacc can be run.
 * scons will try to run yacc on this file so don't mess with it
 */
%{
%}

%%
foobar: 	';';
%%

void foobar;

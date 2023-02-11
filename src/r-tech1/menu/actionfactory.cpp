#include "r-tech1/menu/actionfactory.h"
#include "r-tech1/token.h"
#include "r-tech1/debug.h"
#include "r-tech1/menu/action_speed.h"

using namespace std;

void ActionAct(Token *token){
    const Token * tok;
    token->view() >> tok;
    if ( *tok == "background" ){
    } else if (*tok == "fixedspeed") {
        ActionSpeed temp(tok);
        temp.act();
    } else {
        Global::debug( 3 ) <<"Unhandled menu attribute: "<<endl;
        tok->print(" ");
    }
}


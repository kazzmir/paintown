#include "actionfactory.h"
#include "util/token.h"
#include "util/debug.h"
#include "action_speed.h"

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


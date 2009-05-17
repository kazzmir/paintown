#include "menu/actionfactory.h"
#include "util/token.h"
#include "globals.h"

#include "menu/action_speed.h"

using namespace std;

void ActionAct(Token *token){
	Token * tok;
	*token >> tok;
	if ( *tok == "background" ){
	} else if ( *tok == "fixedspeed" ) {
		ActionSpeed temp(tok);
		temp.act();
	} else {
		Global::debug( 3 ) <<"Unhandled menu attribute: "<<endl;
		tok->print(" ");
	}
}


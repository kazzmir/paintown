#include "menu/actionfactory.h"
#include "util/token.h"

#include "menu/action_speed.h"

void ActionAct(Token *token)
{
	Token * tok;
	*token >> tok;
	if ( *tok == "music" )
	{
	}
	else if ( *tok == "background" )
	{
	}
	else if ( *tok == "fixedspeed" )
	{
		ActionSpeed temp(tok);
		temp.act();
	}
}


#include "menu/itemfactory.h"
#include "util/token.h"

#include "menu/item_speed.h"

void ItemAct(Token *token)
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
		ItemSpeed temp(tok);
		temp.act();
	}
}


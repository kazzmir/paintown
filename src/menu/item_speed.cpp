#include "menu/item_speed.h"
#include "menu/menu_global.h"
#include "util/token.h"

ItemSpeed::ItemSpeed(Token *token)throw( LoadException ) : speed(0.1)
{
	*token >> speed;
}

ItemSpeed::~ItemSpeed()
{
}

void ItemSpeed::act()
{
	if(speed < 0.1)speed = 0.1;
	MenuGlobals::setGameSpeed(speed);
}


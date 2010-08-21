#include "menu/action_speed.h"
#include "configuration.h"
#include "util/token.h"

ActionSpeed::ActionSpeed(Token *token)throw( LoadException ) : speed(0.1)
{
	*token >> speed;
}

ActionSpeed::~ActionSpeed()
{
}

void ActionSpeed::act()
{
	if(speed < 0.1){
	    speed = 0.1;
	}
	Configuration::setGameSpeed(speed);
}


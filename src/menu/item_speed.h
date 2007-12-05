#ifndef _paintown_item_speed_h
#define _paintown_item_speed_h

#include "menu_option.h"
#include "util/load_exception.h"
class Token;
class Menu;

class ItemSpeed
{
	public:
		// Do whatever is needed for the requested item
		virtual void act();

		ItemSpeed(Token *token)throw( LoadException );
	
		virtual ~ItemSpeed();
		
	private:
		// speed
		double speed;
};

#endif

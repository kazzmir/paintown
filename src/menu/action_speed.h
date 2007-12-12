#ifndef _paintown_action_speed_h
#define _paintown_action_speed_h

#include "util/load_exception.h"
class Token;

class ActionSpeed
{
	public:
		// Do whatever is needed for the requested item
		virtual void act();

		ActionSpeed(Token *token)throw( LoadException );
	
		virtual ~ActionSpeed();
		
	private:
		// speed
		double speed;
};

#endif

#ifndef _paintown_option_adventure_h
#define _paintown_option_adventure_h

#include "menu_option.h"
#include "util/load_exception.h"
#include "return_exception.h"
class Token;

/*! Handles key reconfiguration */
class OptionAdventure : public MenuOption
{
	public:
		// Do logic before run part
		virtual void logic();
		
		// Finally it has been selected, this is what shall run 
		// endGame will be set true if it is a terminating option
		virtual void run(bool &endGame) throw (ReturnException);
		
		OptionAdventure(Token *token)throw( LoadException );
	
		virtual ~OptionAdventure();
};

#endif


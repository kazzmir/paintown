#ifndef _paintown_option_background_h
#define _paintown_option_background_h

#include "menu_option.h"
class Token;

/*! Handles menu backgrounds, could be static, animated or whatever */
class OptionBg : public MenuOption
{
	public:
		// Do logic before run part
		virtual void logic();
	
		// Do drawing before run part
		virtual void draw(Bitmap *work);
		
		// Finally it has been selected, this is what shall run 
		// endGame will be set true if it is a terminating option
		virtual void run(bool &endGame);
		
		OptionBg(Token *token);
	
		virtual ~OptionBg();
};

#endif


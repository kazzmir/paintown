#ifndef _paintown_option_background_h
#define _paintown_option_background_h

#include "menu_option.h"
#include "util/load_exception.h"
#include <vector>
class Token;
class Bitmap;

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
		
		OptionBg(Token *token)throw( LoadException );
	
		virtual ~OptionBg();
	private:
		// Image vector
		std::vector<Bitmap *> images;
		
		// animation delay in milliseconsd?
		int animDelay;
};

#endif


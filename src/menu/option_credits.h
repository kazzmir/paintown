#ifndef _paintown_option_credits_h
#define _paintown_option_credits_h

#include "menu_option.h"
#include "util/load_exception.h"

#include <vector>

class Token;
class Bitmap;

/*! Handles key reconfiguration */
class OptionCredits : public MenuOption
{
	public:
		// Do logic before run part
		virtual void logic();
	
		// Do drawing before run part
		virtual void draw(Bitmap *work);
		
		// Finally it has been selected, this is what shall run 
		// endGame will be set true if it is a terminating option
		virtual void run(bool &endGame);
		
		OptionCredits(Token *token)throw( LoadException );
	
		virtual ~OptionCredits();
	private:
		Bitmap *background;
		std::vector<std::string> credits;
};

#endif


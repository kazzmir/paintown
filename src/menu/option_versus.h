#ifndef _paintown_option_versus_h
#define _paintown_option_versus_h

#include "menu_option.h"
#include "util/load_exception.h"
class Token;

/*! Handles key reconfiguration */
class OptionVersus : public MenuOption
{
	public:
		// Do logic before run part
		virtual void logic();
	
		// Do drawing before run part
		virtual void draw(Bitmap *work);
		
		// Finally it has been selected, this is what shall run 
		// endGame will be set true if it is a terminating option
		virtual void run(bool &endGame);
		
		OptionVersus(Token *token)throw( LoadException );
	
		virtual ~OptionVersus();
		
	private:
		bool human;
};

#endif


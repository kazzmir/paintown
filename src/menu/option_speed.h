#ifndef _paintown_option_speed_h
#define _paintown_option_speed_h

#include "menu_option.h"
#include "util/load_exception.h"
class Token;

/*! Handles key reconfiguration */
class OptionSpeed : public MenuOption
{
	public:
		// Do logic before run part
		virtual void logic();
	
		// Do drawing before run part
		virtual void draw(Bitmap *work);
		
		// Finally it has been selected, this is what shall run 
		// endGame will be set true if it is a terminating option
		virtual void run(bool &endGame);
		
		// This is to pass paramaters to an option ie a bar or something
		virtual void leftKey();
		virtual void rightKey();
		
		OptionSpeed(Token *token)throw( LoadException );
	
		virtual ~OptionSpeed();
	public:
		// name
		std::string name;
};

#endif


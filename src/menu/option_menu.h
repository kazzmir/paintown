#ifndef _paintown_option_menu_h
#define _paintown_option_menu_h

#include "menu_option.h"
#include "util/load_exception.h"
class Token;
class Menu;

/*! Handles sub menus */
class OptionMenu : public MenuOption
{
	public:
		// Do logic before run part
		virtual void logic();
	
		// Do drawing before run part
		virtual void draw(Bitmap *work);
		
		// Finally it has been selected, this is what shall run 
		// endGame will be set true if it is a terminating option
		virtual void run(bool &endGame);
		
		OptionMenu(Token *token)throw( LoadException );
	
		virtual ~OptionMenu();
		
	private:
		Menu *_menu;
};

#endif


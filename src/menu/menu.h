#ifndef _paintown_menu_h
#define _paintown_menu_h

#include <vector>
#include <string>

#include "util/load_exception.h"
#include "return_exception.h"

class Bitmap;
class MenuOption;
class Token;

class Menu
{
	public:
		/*! ctor dtor */
		Menu();
		virtual ~Menu();
		
		/*! load */
		void load(const std::string &filename)throw( LoadException );
		
		/*! load */
		void load(Token *token)throw( LoadException );
		
		/*! do logic, draw whatever */
		virtual void run()throw( ReturnException );
		
		static void setBitmap(Bitmap *bmp);
		
	protected:
		// Our current bitmap to draw to, defaults to screen
		static Bitmap *work;
		std::string music;
		MenuOption *background;
		
	private:
		std::vector <MenuOption *> menuOptions;
		std::vector <MenuOption *>::iterator selectedOption;
		
};
#endif

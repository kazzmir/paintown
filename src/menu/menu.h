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
		Menu(const std::string &filename);
		Menu(const Token &token);
		virtual ~Menu();
		
		/*! load */
		void load(const std::string &filename)throw( LoadException );
		
		/*! load */
		void load(const Token &token)throw( LoadException );
		
		/*! do logic, draw whatever */
		virtual void run()throw( ReturnException );
		
		static void setBitmap(Bitmap *bmp);
		
	protected:
		static Bitmap *work;
		
	private:
		std::vector <MenuOption *> menuOptions;
		std::vector <MenuOption *>::iterator selectedOption;
};
#endif

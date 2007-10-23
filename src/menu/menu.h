#ifndef _paintown_menu_h
#define _paintown_menu_h

#include <vector>
#include <string>

#include "util/ftalleg.h"
#include "util/load_exception.h"
#include "return_exception.h"

class Bitmap;
class MenuOption;
class Token;

class RectArea
{
	public:
		RectArea();
		RectArea(int x, int y, int w, int h); bool empty();
		int x;
		int y;
		int width; // Really not necessary but is there in case of later need
		int height;
};
		

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
		//! Our current bitmap to draw to, defaults to screen
		static Bitmap *work;
		//! Current music
		std::string music;
		//! background of current
		MenuOption *background;
		//! Area for placement of option list
		RectArea position;
		//! Font for this menu
		ftalleg::freetype *vFont;
		
	private:
		std::vector <MenuOption *> menuOptions;
		std::vector <MenuOption *>::iterator selectedOption;
		
};
#endif

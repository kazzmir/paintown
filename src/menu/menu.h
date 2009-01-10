#ifndef _paintown_menu_h
#define _paintown_menu_h

#include <vector>
#include <string>

#include "util/load_exception.h"
#include "return_exception.h"
#include "gui/box.h"

#ifdef _MSC_VER
#ifndef uint32_t
	typedef unsigned __int32 uint32_t;
#endif
#endif

class Font;
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
		void load(const std::string &filename) throw (LoadException);
		
		/*! load */
		void load(Token *token) throw (LoadException);
		
		/*! do logic, draw whatever */
		virtual void run();
		
		/*! get menu name */
		const std::string &getName();
		
		/*! set the bitmap */
		static void setBitmap(Bitmap *bmp);
		
		/*! Get current background in Bitmap */
		static Bitmap *getBackground();
		
		//! Our current bitmap to draw to
		static Bitmap *work;

		//! Get menu by name
		static Menu *getMenu(const std::string &name);
		
		//! Get top level menu name
		static std::string &getParentMenu();
		
		//! get font
		static std::string &getFont();
		
		//! get font width
		static int getFontWidth();
		
		//! get font height
		static int getFontHeight();
		
		//! set new font menu wide
		static void setFont(const std::string &font, int w, int h);

                static void setFontName(const std::string & str);
                static void setFontWidth(int w);
                static void setFontHeight(int h);
		
		inline bool checkRemoval() const { return removeOption; }
		
		inline void setAsOption( const bool o ) { option = o; }
		
		inline bool isOption() const { return option; }
		
	protected:
		//! Current music
		std::string music;
		//! Current select sound, beep or whatever
		std::string selectSound;
                std::string backSound;
                std::string okSound;
		//! background of current
		//MenuOption *background;
		//! Area for placement of option list
		Box backboard;
		//! longest menu text
		int longestTextLength;
		
	private:
		std::vector <MenuOption *> menuOptions;
		std::vector <MenuOption *>::iterator selectedOption;
		std::string _name;
		
		// Error checker if no options do not add to menu and remove from list
		bool hasOptions;
		
		// this is for menu removal if an option
		bool removeOption;
		
		//! Set longest length
		void checkTextLength(MenuOption *opt);
		
		//! Draw background
		void drawBackground(Bitmap *work);
		
		//! Draw board
		void drawTextBoard(Bitmap *work);
		
		//! Draw text
		void drawText(Bitmap *work);
		
		//! Draw info text
		void drawInfoText(Bitmap *work);
		
		//! Cleanup options to remove omited items
		void optionCleanup();
		
		//! Draw states
		enum menuDrawState
		{
			FadeIn = 0,
   			FadeInText,
      			NoFade
		};
		
		//! Reset fade data
		void resetFadeInfo();
		
		//! Enum containing the draw state so we can do some effects
		menuDrawState currentDrawState;
		
		//! Fade speed
		static int fadeSpeed;

		Box fadeBox;
		
		//! Set background for this menu
		Bitmap *background;
		
		//! Is this an option?
		bool option;
};
#endif

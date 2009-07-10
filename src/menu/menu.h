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
class MenuAnimation;

class Menu
{
	public:
		/*! ctor dtor */
		Menu();
		virtual ~Menu();
		
		/*! load */
		virtual void load(const std::string &filename) throw (LoadException);
		
		/*! load */
		virtual void load(Token *token) throw (LoadException);
		
		/*! do logic, draw whatever */
		virtual void run();
		
		/*! get menu name */
		inline const std::string &getName() const { return _name; };
		
		/*! set menu name */
		inline void setName(const std::string &n){ _name = n; };
		
		//! draw snapshot of menu to buffer to facilitate tabs
		void drawMenuSnap(Bitmap *bmp);
		
		/*! Add options to menu */
		void addOption(MenuOption *opt);
		
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
		
		//! Area for placement of option list
		Box backboard;
		
	protected:
		//! Current music
		std::string music;
		//! Current select sound, beep or whatever
		std::string selectSound;
                std::string backSound;
                std::string okSound;
		//! longest menu text
		int longestTextLength;
		
		std::vector <MenuOption *> menuOptions;
		std::vector <MenuOption *>::iterator selectedOption;
                double motion;
		//! Animations
		std::vector <MenuAnimation *>backgroundAnimations;
		std::vector <MenuAnimation *>foregroundAnimations;
		
		
		//! Draw background
		void drawBackground(Bitmap *work);
		
		//! Reset fade data
		void resetFadeInfo();
		
		//! Do fade logic
		void updateFadeInfo();
		
		//! Draw board
		void drawTextBoard(Bitmap *work);
		
	private:
	    
		std::string _name;
		
		// Error checker if no options do not add to menu and remove from list
		bool hasOptions;
		
		// this is for menu removal if an option
		bool removeOption;
		
		//! Set longest length
		void checkTextLength(MenuOption *opt);
		
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
		
		//! Enum containing the draw state so we can do some effects
		menuDrawState currentDrawState;
		
		//! Fade speed
		static int fadeSpeed;

		Box fadeBox;
		
		//! Set background for this menu
		Bitmap *background;
		
		//! Clear color only used if no background is set this is for every existant menu
		static int clearColor;
		
		//! Is this an option?
		bool option;
};
#endif

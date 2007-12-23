#ifndef _paintown_menu_h
#define _paintown_menu_h

#include <vector>
#include <string>

#include "util/font.h"
#include "util/load_exception.h"
#include "return_exception.h"
#include "gui/rectarea.h"

#ifdef _MSC_VER
#ifndef uint32_t
	typedef unsigned __int32 uint32_t;
#endif
#endif

// Keep consistent
typedef uint32_t useflags;

class Bitmap;
class MenuOption;
class Token;
		
class Menu
{
	public:
		/*! ctor dtor */
		Menu();
		virtual ~Menu();

		/*! Return flags for menu */
		enum MenuFlags
		{
			    M_Empty		=1<<0,
			    M_Ok		=1<<1,
			    M_Cancel		=1<<2,
			    M_Error		=1<<3,
			    M_Off		=1<<4,
       			    M_On		=1<<5,
	                    M_Quit		=1<<6,
		            M_Resume		=1<<7,
	                    M_Default		=1<<8
		};
		
		/*! load */
		void load(const std::string &filename)throw( LoadException );
		
		/*! load */
		void load(Token *token)throw( LoadException );
		
		/*! do logic, draw whatever */
		virtual useflags run();
		
		/*! get menu name */
		const std::string &getName();
		
		/*! set the bitmap */
		static void setBitmap(Bitmap *bmp);

		//! Get menu by name
		static Menu *getMenu(const std::string &name);
		
		//! get font
		static FreeTypeFont *getFont();

		//! Set flag(s)
		inline void setFlags(useflags f, bool tf){ if(tf){_menuflags |= f;}else{_menuflags &= ~f;}}
		
		//! Check flag(s)
		inline useflags getFlags() { return _menuflags; }
		
	protected:
		//! Our current bitmap to draw to
		static Bitmap *work;
		//! Current music
		std::string music;
		//! background of current
		MenuOption *background;
		//! Area for placement of option list
		RectArea position;
		//! Font for this menu
		FreeTypeFont *vFont;
		//! Font width
		int fontWidth;
		//! Font height
		int fontHeight;
		//! menu flags
		useflags _menuflags;
		//! longest menu text
		int longestTextLength;
		
	private:
		std::vector <MenuOption *> menuOptions;
		std::vector <MenuOption *>::iterator selectedOption;
		std::string _name;
		
		//! Set longest length
		void checkTextLength(MenuOption *opt);
		
};
#endif

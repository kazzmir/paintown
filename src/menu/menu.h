#ifndef _paintown_menu_h
#define _paintown_menu_h

#include <vector>
#include <string>

#include "util/load_exception.h"
#include "game/input-map.h"
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



class Point{
    public:
    int x;
    int y;
    Point();
    Point(int x, int y);
    ~Point();
};

class Menu
{
	public:
		/*! ctor dtor */
		Menu();
        Menu(const std::string & str) throw (LoadException);
        Menu(Token * token) throw (LoadException);
		virtual ~Menu();
		
		/*! load */
		virtual void load(const std::string &filename) throw (LoadException);
		
		/*! load */
		virtual void load(Token *token) throw (LoadException);
		
		/*! Logic */
		virtual void act(bool &endGame, bool reset = true) throw (ReturnException);
		
		/*! Draw */
		virtual void draw(const Box &area, Bitmap *bmp);
		
		//! Draw text
		virtual void drawText(const Box &area, Bitmap *bmp);
		
		/*! run as it's own menu */
		virtual void run() throw (ReturnException);
		
		/*! Parent */
		virtual void setParent(Menu *menu);
		
		/*! get parent */
		inline Menu *getParent() { return parent; }
		
		/*! get background */
		virtual Bitmap *getBackground();
		
		/*! get menu name */
		inline const std::string &getName() const { return _name; };
		
		/*! set menu name */
		inline void setName(const std::string &n){ _name = n; };
		
		/*! Add options to menu */
		void addOption(MenuOption *opt);
		
		/*! get working bitmap */
		Bitmap *getWork();
		
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
		
		inline void setInfoText(const std::string &text) { menuInfo = text; }

        inline bool hasSomeOptions() const {
            return hasOptions;
        }
        
        /* finish initializing stuff for options */
        void setupOptions();
		
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
    public:
        /* hack to make tab menu work */
		std::vector <MenuOption *>::iterator selectedOption;
    protected:
        double motion;
		//! Animations
		std::vector <MenuAnimation *>backgroundAnimations;
		std::vector <MenuAnimation *>foregroundAnimations;
		
		//! Draw states
		enum menuDrawState
		{
			FadeIn = 0,
   			FadeInText,
      			NoFade
		};
		
		//! Enum containing the draw state so we can do some effects
		menuDrawState currentDrawState;
		
		//! Our current bitmap to draw to
                /* can't this be passed in instead of created for every menu? */
		Bitmap *work;
		
		//! Draw background
		void drawBackground(Bitmap *work);
		
		//! Reset fade data
		void resetFadeInfo();
		
		//! Do fade logic
		void updateFadeInfo();
		
		//! Draw board
		void drawTextBoard(Bitmap *work);
		
		//! Draw info box
		void drawInfoBox (const std::string &info, const Point &location, Bitmap *bmp );

                int getSelectedColor(bool selected);

		
		//! General text about the menu, similar to info about options if empty it draws nothing it also borrows the colors of the main menu box
		std::string menuInfo;
		
		//! menu info location
		Point menuInfoLocation;
		
		//! parent menu
		Menu *parent;
		
	private:
		std::string _name;
		
		// Error checker if no options do not add to menu and remove from list
		bool hasOptions;
		
		// this is for menu removal if an option
		bool removeOption;
		
		//! Set longest length
		void checkTextLength(MenuOption *opt);
		
		//! Cleanup options to remove omited items
		void optionCleanup();
		
		//! Fade alpha
		int fadeAlpha;
		
		//! Fade speed
		int fadeSpeed;

		Box fadeBox;
		
		//! Set background for this menu
		Bitmap *background;
		
		//! Clear color only used if no background is set this is for every existant menu
		int clearColor;
		
		//! Is this an option?
		bool option;
		
		//! This is the location of the option info text
		Point optionInfoTextLocation;

                unsigned int selectedGradientIndex;
                static const int selectedGradientMax = 40;
                int selectedGradient[selectedGradientMax];

        enum MenuInput{
            Up,
            Down,
            Left,
            Right,
            Select,
            /* return is a safe menu return */
            Return,
            /* exit is usually done by ESC */
            Exit,
        };

        InputMap<MenuInput> input;
};
#endif

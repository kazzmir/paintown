#ifndef _paintown_tab_menu_h
#define _paintown_tab_menu_h

#include "menu.h"
#include "gui/box.h"

class Font;
class Bitmap;
class MenuOption;
class Token;
class MenuAnimation;

// Provide menus in a box
class MenuBox : public Box
{
    public:
	MenuBox();
	virtual ~MenuBox();
	Menu menu;
};

class TabMenu : public Menu
{
    public:
	/*! ctor dtor */
	TabMenu();
	virtual ~TabMenu();
	
	/*! load */
	void load(const std::string &filename) throw (LoadException);
	
	/*! load */
	void load(Token *token) throw (LoadException);
	
	/*! do logic, draw whatever */
	void run();
    private:
	RectArea tabInfo;
	RectArea selectedTabInfo;
	
	std::vector<MenuBox *>tabs;
	std::vector<MenuBox *>::iterator currentTab;
	
	void updateSnapshots();
	
	void drawTabs(Bitmap *work);
	void drawSnapshots();
};
#endif

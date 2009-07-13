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
	MenuBox(int w, int h);
	virtual ~MenuBox();
	void updateSnapshot();
	Menu menu;
	Bitmap *snap;
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
	int location;
	// Our actual targetOffset
	double targetOffset;
	// Actual offset
	double totalOffset;
	double scrollSpeed;
	
	void drawTabs(Bitmap *bmp);
	void drawSnapshots(Bitmap *bmp);
};
#endif

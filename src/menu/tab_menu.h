#ifndef _paintown_tab_menu_h
#define _paintown_tab_menu_h

#include "menu.h"

class Font;
class Bitmap;
class MenuOption;
class Token;
class MenuAnimation;

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
	std::vector <Menu *> menus;
	
	RectArea tabInfo;
	RectArea selectedTabInfo;
	
	std::vector<Box *>tabs;
	std::vector<Box *>::iterator currentTab;
	
	void updateSnapshots();
	
	void drawTabs(Bitmap *work);
	void drawSnapshots();
};
#endif

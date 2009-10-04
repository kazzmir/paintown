#ifndef _paintown_tab_menu_h
#define _paintown_tab_menu_h

#include "menu.h"
#include "gui/box.h"
#include "return_exception.h"
#include "game/input-map.h"

class Font;
class Bitmap;
class MenuOption;
class Token;
class MenuAnimation;

// Provides a buffer for going between two colors
class ColorBuffer{
    public:
	ColorBuffer(int color1, int color2);
	virtual ~ColorBuffer();

    static const int maxColors = 100;
	int colors[maxColors];
	int index;
	bool forward;
	
	int update();
	
	void reset();
};

// Provide menus in a box
class MenuBox : public Box
{
    public:
	MenuBox(int w, int h);
	virtual ~MenuBox();
	bool checkVisible(const RectArea &area);
	void setColors(const RectArea &info, const int fontColor);
	void setColors(const int bodyColor, const int borderColor, const int fontColor);
	Menu menu;
	int fontColor;
	Box snapPosition;
	bool running;
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
	void run() throw (ReturnException);
    private:
	RectArea tabInfo;
	RectArea selectedTabInfo;
	RectArea runningTabInfo;
	
	int fontColor;
	int selectedFontColor;
	int runningFontColor;
	
	std::string runningInfo;
	
	std::vector<MenuBox *>tabs;
	std::vector<MenuBox *>::iterator currentTab;
	int location;
	// Our actual targetOffset
	double targetOffset;
	// Actual offset
	double totalOffset;
	// double scrollSpeed;
	// Total lines from options
	int totalLines;

    enum TabInput{
        Left,
        Right,
        Select,
        Exit,
    };

    InputMap<TabInput> input;
	
	void drawMenus(Bitmap *bmp);
	
	void calculateTabLines();
};
#endif

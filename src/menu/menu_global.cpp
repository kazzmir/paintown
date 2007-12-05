#include "menu/menu_global.h"

double MenuGlobals::gamespeed = 1.0;

//static std::priority_queue<std::string> lastPlayed;

//static std::queue<MenuOption *> backgrounds;

MenuGlobals::MenuGlobals()
{
}

MenuGlobals::~MenuGlobals()
{
}

void MenuGlobals::setMusic(const std::string &file)
{
}

/*! game speed */
double MenuGlobals::getGameSpeed()
{
	return gamespeed;
}

/*! set speed */
void MenuGlobals::setGameSpeed(double s)
{
	gamespeed = s;
}




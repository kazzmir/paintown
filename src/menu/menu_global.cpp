#include "menu/menu_global.h"

#include "menu/menu.h"
#include "menu/menu_global.h"
#include "menu/menu_option.h"
#include "util/bitmap.h"
#include "util/funcs.h"
#include "util/keyboard.h"
#include "util/sound.h"
#include "util/token.h"
#include "util/tokenreader.h"
#include "globals.h"
#include "init.h"
#include "music.h"

double MenuGlobals::gamespeed = 1.0;
bool MenuGlobals::invincible = false;
bool MenuGlobals::fullscreen = false;
int MenuGlobals::lives = 4;
int MenuGlobals::npc_buddies = 1;

std::priority_queue<std::string> MenuGlobals::lastPlayed;

//static std::queue<MenuOption *> backgrounds;

MenuGlobals::MenuGlobals()
{
}

MenuGlobals::~MenuGlobals()
{
}

void MenuGlobals::setMusic(const std::string &file)
{
	lastPlayed.push(file);
	if(Music::loadSong( Util::getDataPath() + file ))
	{
		Music::pause();
		Music::play();
	}
}

const std::string MenuGlobals::currentMusic()
{
	if(!lastPlayed.empty())
	{
		return lastPlayed.top();
	}
	else return std::string();
}

void MenuGlobals::popMusic()
{
	if(!lastPlayed.empty())
	{
		lastPlayed.pop();
		Music::pause();
		Music::loadSong( Util::getDataPath() + lastPlayed.top() );
		Music::play();
	}
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

/*! invincible */
bool MenuGlobals::getInvincible()
{
	return invincible;
}

/*! set invincible */
void MenuGlobals::setInvincible(bool i)
{
	invincible = i;
}

/*! game fullscreen */
bool MenuGlobals::getFullscreen()
{
	return fullscreen;
}

/*! set speed */
void MenuGlobals::setFullscreen(bool f)
{
	fullscreen = f;
}

/*! game lives */
int MenuGlobals::getLives()
{
	return lives;
}

/*! set lives */
void MenuGlobals::setLives(int l)
{
	lives = l;
}

int MenuGlobals::getNpcBuddies(){
	return npc_buddies;
}

void MenuGlobals::setNpcBuddies( int i ){
	npc_buddies = i;
}

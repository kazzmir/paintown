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
#include "configuration.h"
#include "music.h"

std::priority_queue<std::string> MenuGlobals::lastPlayed;

//static std::queue<MenuOption *> backgrounds;

MenuGlobals::MenuGlobals(){
}

MenuGlobals::~MenuGlobals(){
}

void MenuGlobals::setMusic(const std::string &file){
	lastPlayed.push(file);
	if(Music::loadSong( Util::getDataPath() + file )){
		Music::pause();
		Music::play();
	}
}

const std::string MenuGlobals::currentMusic(){
	if(!lastPlayed.empty()){
		return lastPlayed.top();
	}
	else return std::string();
}

void MenuGlobals::popMusic(){
	if(!lastPlayed.empty()){
		lastPlayed.pop();
		Music::pause();
		Music::loadSong( Util::getDataPath() + lastPlayed.top() );
		Music::play();
	}
}

/*! game speed */
double MenuGlobals::getGameSpeed(){
	return Configuration::getGameSpeed();
}

/*! set speed */
void MenuGlobals::setGameSpeed(double s){
	Configuration::setGameSpeed( s );
}

/*! invincible */
bool MenuGlobals::getInvincible(){
	return Configuration::getInvincible();
}

/*! set invincible */
void MenuGlobals::setInvincible(bool i){
	Configuration::setInvincible( i );
}

/*! game fullscreen */
bool MenuGlobals::getFullscreen(){
	return Configuration::getFullscreen();
}

/*! set speed */
void MenuGlobals::setFullscreen(bool f){
	Configuration::setFullscreen( f );
}

/*! game lives */
int MenuGlobals::getLives(){
	return Configuration::getLives();
}

/*! set lives */
void MenuGlobals::setLives(int l){
	Configuration::setLives( l );
}

int MenuGlobals::getNpcBuddies(){
	return Configuration::getNpcBuddies();
}

void MenuGlobals::setNpcBuddies( int i ){
	Configuration::setNpcBuddies( i );
}

bool MenuGlobals::freeForAll(){
    return Configuration::getPlayMode() == Configuration::FreeForAll;
}

bool MenuGlobals::cooperative(){
    return Configuration::getPlayMode() == Configuration::Cooperative;
}

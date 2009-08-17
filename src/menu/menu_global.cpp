#include "util/bitmap.h"
#include "menu/menu_global.h"
#include "menu/menu.h"
#include "menu/menu_global.h"
#include "menu/menu_option.h"
#include "menu/option_level.h"
#include "util/funcs.h"
#include "util/keyboard.h"
#include "util/sound.h"
#include "util/token.h"
#include "util/file-system.h"
#include "util/tokenreader.h"
#include "globals.h"
#include "resource.h"
#include "init.h"
#include "configuration.h"
#include "music.h"

using namespace std;

std::priority_queue<std::string> MenuGlobals::selectSound;

// std::string MenuGlobals::level = "";

MenuGlobals::MenuGlobals(){
}

MenuGlobals::~MenuGlobals(){
}

void MenuGlobals::setMusic(const std::string &file){
    if(Music::loadSong(Filesystem::find(file))){
	Music::pause();
	Music::play();
    }
}

void MenuGlobals::setSelectSound(const std::string &file){
	selectSound.push(file);
}

const std::string MenuGlobals::currentSelectSound(){
	if(!selectSound.empty()){
		return selectSound.top();
	}
	else return std::string();
}

void MenuGlobals::playSelectSound() throw (LoadException){
	if (!selectSound.empty()){
	      try{
                  Sound * select = Resource::getSound(selectSound.top());
                  select->play();
	      } catch ( const LoadException & ex ) {
		      throw ex;
	      }
	}
}

void MenuGlobals::popSelectSound(){
	if(!selectSound.empty()){
		selectSound.pop();
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

std::string MenuGlobals::doLevelMenu(const std::string dir, Menu *parent){
    std::vector<std::string> possible = Util::getFiles(Filesystem::find(dir + "/"), "*.txt" );

    /* count is the number of pixels the menu can be. try not to hard code
     * if possible. it should probably be based on the size of the font.
     */
    int count = 0;
    for ( vector<string>::iterator it = possible.begin(); it != possible.end(); it++ ){
        string & path = *it;
        path = Filesystem::cleanse(path);
        /*
        path.erase(0, Util::getDataPath().length() + 1);
        */
        count += 60;
    }

    if (count > 250){
        count = 250;
    }

    if ( possible.size() == 0 ){
        return "no-files";
    }

    if (possible.size() == 1){
        return Filesystem::find(possible[0]);
    }

    try{
        Menu temp;
	temp.setParent(parent);
        string level;
        for ( unsigned int i = 0; i < possible.size(); i++ ){
            OptionLevel *opt = new OptionLevel(0, &level);
            opt->setText(possible[i]);
            opt->setInfoText("Select a set of levels to play");
            temp.addOption(opt);
        }
        temp.load(Filesystem::find("menu/level_select.txt"));
        temp.backboard.position.height = count;
        // Run it
        temp.run();
        return Filesystem::find(level);
    } catch (const TokenException & ex){
        Global::debug(0) << "There was a problem with the token. Error was:\n  " << ex.getReason() << endl;
        return "";
    } catch (const LoadException & ex){
        Global::debug(0) << "There was a problem loading the level select menu. Error was:\n  " << ex.getReason() << endl;
        return "";
    }
    throw LoadException("No level chosen!");

    /*
    // Now lets get the level or return
    std::string l = level;
    level = "";

    return l;
    */
}

bool MenuGlobals::freeForAll(){
    return Configuration::getPlayMode() == Configuration::FreeForAll;
}

bool MenuGlobals::cooperative(){
    return Configuration::getPlayMode() == Configuration::Cooperative;
}

void MenuGlobals::setFreeForAll(){
    Configuration::setPlayMode(Configuration::FreeForAll);
}

void MenuGlobals::setCooperative(){
    Configuration::setPlayMode(Configuration::Cooperative);
}

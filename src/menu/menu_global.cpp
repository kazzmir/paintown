#include "util/bitmap.h"
#include "menu/menu_global.h"
#include "menu/menu.h"
#include "menu/menu_global.h"
#include "menu/menu_option.h"
#include "util/funcs.h"
#include "util/sound.h"
#include "util/token.h"
#include "util/file-system.h"
#include "util/tokenreader.h"
#include "game/mod.h"
#include "level/utils.h"
#include "globals.h"
#include "resource.h"
#include "init.h"
#include "configuration.h"
#include "music.h"
#include "options.h"

using namespace std;

std::priority_queue<std::string> MenuGlobals::selectSound;

// std::string MenuGlobals::level = "";

MenuGlobals::MenuGlobals(){
}

MenuGlobals::~MenuGlobals(){
}

void MenuGlobals::setMusic(const std::string &file) throw (Filesystem::NotFound){
    if (Music::loadSong(Filesystem::find(Filesystem::RelativePath(file)).path())){
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

void MenuGlobals::playSelectSound(){
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

Level::LevelInfo MenuGlobals::doLevelMenu(const std::string dir, Menu *parent){
#if 0
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

    if (possible.size() == 0){
        throw LoadException("No level files found");
    }

    if (possible.size() == 1){
        return Level::readLevels(Filesystem::find(possible[0]));
    }
#endif
    
    vector<Level::LevelInfo> & possible = Paintown::Mod::getCurrentMod()->getLevels();
    if (possible.size() == 0){
        throw LoadException("No level sets defined!");
    }

    if (possible.size() == 1){
        return possible[0];
    }

    /* don't hardcode 60, base it on the size of the font */
    int count = possible.size() * 60;
    /* what is 250?? */
    if (count > 250){
        count = 250;
    }

    try{
        Menu temp;
        temp.setParent(parent);
        int index = 0;
        for ( unsigned int i = 0; i < possible.size(); i++ ){
            OptionLevel *opt = new OptionLevel(0, &index, i);
            opt->setText(possible[i].getName());
            opt->setInfoText("Select a set of levels to play");
            temp.addOption(opt);
        }
        temp.load(Filesystem::find(Filesystem::RelativePath("menu/level_select.txt")));
        //temp.backboard.position.height = count;
        temp.setMenuHeight(count);
        // Run it
        temp.run();
        return possible[index];
    } catch (const TokenException & ex){
        Global::debug(0) << "There was a problem with the token. Error was:\n  " << ex.getReason() << endl;
        throw LoadException("Could not load levels " + ex.getReason());
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

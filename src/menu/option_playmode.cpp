#include "util/bitmap.h"
#include "menu/option_playmode.h"
#include "util/token.h"
#include "menu/menu.h"
#include "menu/menu_global.h"
#include "globals.h"
#include "init.h"
#include <sstream>

using namespace std;

OptionPlayMode::OptionPlayMode(Token *token) throw (LoadException):
MenuOption(token, AdjustableOption),
lblue(255),
lgreen(255),
rblue(255),
rgreen(255){
    setRunnable(false);

    if ( *token != "play-mode" ){
        throw LoadException("Not a play-mode");
    }

    readName(token);
}

OptionPlayMode::~OptionPlayMode(){
    // Nothing
}
    
std::string OptionPlayMode::getText(){
    ostringstream out;
    out << MenuOption::getText() << ": ";

    /* TODO: language translations of these */
    if (MenuGlobals::freeForAll()){
        out << "Free for all";
    } else if (MenuGlobals::cooperative()){
        out << "Cooperative";
    }

    return out.str();
}

void OptionPlayMode::logic(){
    

    if (lblue < 255){
        lblue += 5;
    }

    if (rblue < 255){
        rblue += 5;
    }

    if (lgreen < 255){
        lgreen += 5;
    }

    if (rgreen < 255){
        rgreen += 5;
    }

    setLeftAdjustColor(Bitmap::makeColor(255, lblue, lgreen));
    setRightAdjustColor(Bitmap::makeColor(255, rblue, rgreen));
}

void OptionPlayMode::run(bool &endGame){
}
    
void OptionPlayMode::changeMode(){
    if (MenuGlobals::freeForAll()){
        MenuGlobals::setCooperative();
    } else if (MenuGlobals::cooperative()){
        MenuGlobals::setFreeForAll();
    }
}

bool OptionPlayMode::leftKey(){
    changeMode();
    lblue = lgreen = 0;
    return true;
}

bool OptionPlayMode::rightKey(){
    changeMode();
    rblue = rgreen = 0;
    return true;
}

#include "util/bitmap.h"
#include "menu/option_playmode.h"
#include "util/token.h"
#include "menu/menu.h"
#include "menu/menu_global.h"
#include "globals.h"
#include "init.h"

using namespace std;

OptionPlayMode::OptionPlayMode(Token *token) throw (LoadException):
MenuOption(token, AdjustableOption),
name(""),
lblue(255),
lgreen(255),
rblue(255),
rgreen(255){
    setRunnable(false);

    if ( *token != "play-mode" ){
        throw LoadException("Not a play-mode");
    }

    while (token->hasTokens()){
        try{
            Token * tok;
            *token >> tok;
            if ( *tok == "name" ){
                *tok >> name;
            } else {
                Global::debug( 3 ) <<"Unhandled menu attribute: "<<endl;
                if (Global::getDebug() >= 3){
                    tok->print(" ");
                }
            }
        } catch ( const TokenException & ex ) {
            // delete current;
            string m( "Menu parse error: " );
            m += ex.getReason();
            throw LoadException( m );
        } catch ( const LoadException & ex ) {
            // delete current;
            throw ex;
        }
    }

    if (name.empty()){
        throw LoadException("No name set, this option should have a name!");
    }
}

OptionPlayMode::~OptionPlayMode(){
    // Nothing
}

void OptionPlayMode::logic(){
    //ostringstream temp;
    if (MenuGlobals::freeForAll()){
        setText(name + ": Free for all");
    } else if (MenuGlobals::cooperative()){
        setText(name + ": Cooperative");
    }

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

void OptionPlayMode::run(bool &endGame) throw (ReturnException) {
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

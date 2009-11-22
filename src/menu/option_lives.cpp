#include "util/bitmap.h"
#include "menu/option_lives.h"
#include "util/token.h"
#include "menu/menu.h"
#include "menu/menu_global.h"
#include "globals.h"
#include <sstream>

using namespace std;

OptionLives::OptionLives( Token * token ) throw( LoadException ):
MenuOption(token, AdjustableOption),
lblue(255),
lgreen(255),
rblue(255),
rgreen(255){
	setRunnable(false);
	
	if ( *token != "lives" ){
		throw LoadException( "Not lives option" );
	}

        readName(token);
}

OptionLives::~OptionLives(){
}
    
std::string OptionLives::getText(){
    ostringstream out;
    out << MenuOption::getText() << ": " << MenuGlobals::getLives();
    return out.str();
}

void OptionLives::logic(){

    if ( lblue < 255 ){
        lblue += 5;
    }

    if ( rblue < 255 ){
        rblue += 5;
    }

    if ( lgreen < 255 ){
        lgreen += 5;
    }

    if ( rgreen < 255 ){
        rgreen += 5;
    }

    setLeftAdjustColor(Bitmap::makeColor( 255, lblue, lgreen ));
    setRightAdjustColor(Bitmap::makeColor( 255, rblue, rgreen ));
}

void OptionLives::run(bool &endGame){
}

bool OptionLives::leftKey(){
	MenuGlobals::setLives(MenuGlobals::getLives() - 1);
	if ( MenuGlobals::getLives() < 1 ){
		MenuGlobals::setLives(1);
	}
	
	lblue = lgreen = 0;
	return false;
}

bool OptionLives::rightKey(){
	MenuGlobals::setLives( MenuGlobals::getLives() + 1 );
	rblue = rgreen = 0;
	return false;
}

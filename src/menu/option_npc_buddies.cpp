#include "util/bitmap.h"
#include "menu/option_npc_buddies.h"
#include "util/token.h"
#include "menu/menu.h"
#include "menu/menu_global.h"
#include "globals.h"
#include <sstream>

using namespace std;

OptionNpcBuddies::OptionNpcBuddies( Token * token ) throw( LoadException ):
MenuOption(token, AdjustableOption),
lblue(255),
lgreen(255),
rblue(255),
rgreen(255){
    setRunnable(false);

    if ( *token != "npc" ){
        throw LoadException( "Not npc option" );
    }

    readName(token);
}

OptionNpcBuddies::~OptionNpcBuddies(){
	// Nothing
}
        
std::string OptionNpcBuddies::getText(){
    ostringstream out;
    out << MenuOption::getText() << ": " << MenuGlobals::getNpcBuddies();
    return out.str();
}

void OptionNpcBuddies::logic(){

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

void OptionNpcBuddies::run(bool &endGame){
}

bool OptionNpcBuddies::leftKey(){
	MenuGlobals::setNpcBuddies(MenuGlobals::getNpcBuddies() - 1);
	if ( MenuGlobals::getNpcBuddies() < 1 ){
		MenuGlobals::setNpcBuddies(1);
	}
	
	lblue = lgreen = 0;
	return false;
}

bool OptionNpcBuddies::rightKey(){
	MenuGlobals::setNpcBuddies( MenuGlobals::getNpcBuddies() + 1 );
	rblue = rgreen = 0;
	return false;
}

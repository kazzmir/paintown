#include "util/bitmap.h"
#include "menu/option_speed.h"
#include "util/token.h"
#include "menu/menu.h"
#include "menu/menu_global.h"
#include "globals.h"
#include <sstream>

#include <stdio.h>

using namespace std;

OptionSpeed::OptionSpeed(Token *token) throw (LoadException): MenuOption(token, AdjustableOption), name(""), lblue(255), lgreen(255), rblue(255), rgreen(255)
{
    setRunnable(false);

    if ( *token != "speed" )
        throw LoadException("Not speed option");

    readName(token);
}

OptionSpeed::~OptionSpeed(){
	// Nothing
}


std::string OptionSpeed::getText(){
    ostringstream out;
    out << MenuOption::getText() << ": " << MenuGlobals::getGameSpeed();
    return out.str();
}

void OptionSpeed::logic(){
    /*
	//ostringstream temp;
	char temp[255];
	sprintf( temp, "%s: %0.2f", name.c_str(), MenuGlobals::getGameSpeed() );
	setText(std::string(temp));
        */
	
	if(lblue < 255)lblue+=5;
	if(rblue < 255)rblue+=5;
	if(lgreen < 255)lgreen+=5;
	if(rgreen < 255)rgreen+=5;
	
	setLeftAdjustColor(Bitmap::makeColor( 255, lblue, lgreen ));
	setRightAdjustColor(Bitmap::makeColor( 255, rblue, rgreen ));
}

void OptionSpeed::run(bool &endGame){
}

bool OptionSpeed::leftKey()
{
	MenuGlobals::setGameSpeed(MenuGlobals::getGameSpeed() - 0.05);
	if( MenuGlobals::getGameSpeed() < 0.1 )MenuGlobals::setGameSpeed(0.1);
	
	lblue = lgreen = 0;
	return false;
}
bool OptionSpeed::rightKey()
{
	MenuGlobals::setGameSpeed(MenuGlobals::getGameSpeed() + 0.05);
	
	rblue = rgreen = 0;
	return false;
}


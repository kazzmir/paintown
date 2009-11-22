#include "util/bitmap.h"
#include "menu/option_fullscreen.h"
#include "util/token.h"
#include "menu/menu.h"
#include "menu/menu_global.h"
#include "globals.h"
#include "init.h"
#include <sstream>

using namespace std;

OptionFullscreen::OptionFullscreen(Token *token) throw (LoadException):
MenuOption(token, AdjustableOption),
lblue(255),
lgreen(255),
rblue(255),
rgreen(255){
	setRunnable(false);
	
	if ( *token != "fullscreen" )
		throw LoadException("Not fullscreen option");

        readName(token);
}

OptionFullscreen::~OptionFullscreen()
{
	// Nothing
}
    
std::string OptionFullscreen::getText(){
    ostringstream out;
    out << MenuOption::getText() << ": " << (MenuGlobals::getFullscreen() ? "Yes" : "No");
    return out.str();
}

void OptionFullscreen::logic(){
	if(lblue < 255)lblue+=5;
	if(rblue < 255)rblue+=5;
	if(lgreen < 255)lgreen+=5;
	if(rgreen < 255)rgreen+=5;
	
	setLeftAdjustColor(Bitmap::makeColor( 255, lblue, lgreen ));
	setRightAdjustColor(Bitmap::makeColor( 255, rblue, rgreen ));
}

void OptionFullscreen::run(bool &endGame){
}

bool OptionFullscreen::leftKey()
{
	MenuGlobals::setFullscreen(!MenuGlobals::getFullscreen());
	lblue = lgreen = 0;
	int gfx = (MenuGlobals::getFullscreen() ? Global::FULLSCREEN : Global::WINDOWED);
	Bitmap::setGraphicsMode( gfx, GFX_X, GFX_Y );
	return true;
}
bool OptionFullscreen::rightKey()
{
	MenuGlobals::setFullscreen(!MenuGlobals::getFullscreen());
	rblue = rgreen = 0;
	int gfx = (MenuGlobals::getFullscreen() ? Global::FULLSCREEN : Global::WINDOWED);
	Bitmap::setGraphicsMode( gfx, GFX_X, GFX_Y );
	return true;
}


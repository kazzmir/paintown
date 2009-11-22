#include "util/bitmap.h"
#include "menu/option_invincible.h"
#include "util/token.h"
#include "menu/menu.h"
#include "menu/menu_global.h"
#include "globals.h"

using namespace std;

OptionInvincible::OptionInvincible(Token *token) throw (LoadException): MenuOption(token, AdjustableOption), name(""), lblue(255), lgreen(255), rblue(255), rgreen(255)
{
	setRunnable(false);
	
	if ( *token != "invincible" )
		throw LoadException("Not invincible option");

        readName(token);
}

OptionInvincible::~OptionInvincible()
{
	// Nothing
}

void OptionInvincible::logic()
{
	setText(name + (MenuGlobals::getInvincible() ? ": Yes" : ": No"));
	
	if(lblue < 255)lblue+=5;
	if(rblue < 255)rblue+=5;
	if(lgreen < 255)lgreen+=5;
	if(rgreen < 255)rgreen+=5;
	
	setLeftAdjustColor(Bitmap::makeColor( 255, lblue, lgreen ));
	setRightAdjustColor(Bitmap::makeColor( 255, rblue, rgreen ));
}

void OptionInvincible::run(bool &endGame){
}

bool OptionInvincible::leftKey()
{
	MenuGlobals::setInvincible(!MenuGlobals::getInvincible());
	lblue = lgreen = 0;
	return true;
}
bool OptionInvincible::rightKey()
{
	MenuGlobals::setInvincible(!MenuGlobals::getInvincible());
	rblue = rgreen = 0;
	return true;
}


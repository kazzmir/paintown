#include "util/bitmap.h"
#include "menu/option_fullscreen.h"
#include "util/token.h"
#include "menu/menu.h"
#include "menu/menu_global.h"
#include "globals.h"
#include "init.h"

using namespace std;

OptionFullscreen::OptionFullscreen(Token *token) throw (LoadException): MenuOption(token, AdjustableOption), name(""), lblue(255), lgreen(255), rblue(255), rgreen(255)
{
	setRunnable(false);
	
	if ( *token != "fullscreen" )
		throw LoadException("Not fullscreen option");
	
	while ( token->hasTokens() )
	{
		try 
		{
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
		} 
		catch ( const TokenException & ex )
		{
			// delete current;
			string m( "Menu parse error: " );
			m += ex.getReason();
			throw LoadException( m );
		} 
		catch ( const LoadException & ex )
		{
			// delete current;
			throw ex;
		}
	}
	
	if(name.empty())throw LoadException("No name set, this option should have a name!");
}

OptionFullscreen::~OptionFullscreen()
{
	// Nothing
}

void OptionFullscreen::logic()
{
	//ostringstream temp;
	setText(name + (MenuGlobals::getFullscreen() ? ": Yes" : ": No"));
	
	if(lblue < 255)lblue+=5;
	if(rblue < 255)rblue+=5;
	if(lgreen < 255)lgreen+=5;
	if(rgreen < 255)rgreen+=5;
	
	setLeftAdjustColor(Bitmap::makeColor( 255, lblue, lgreen ));
	setRightAdjustColor(Bitmap::makeColor( 255, rblue, rgreen ));
}

void OptionFullscreen::run(bool &endGame)
{
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


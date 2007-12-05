#include "menu/option_speed.h"
#include "util/token.h"
#include "menu/menu.h"
#include "menu/menu_global.h"
#include "globals.h"

OptionSpeed::OptionSpeed(Token *token)throw( LoadException ) : MenuOption(event), name("")
{
	if ( *token != "speed" )
		throw LoadException("Not speed option");
	
	while ( token->hasTokens() )
	{
		try 
		{
			Token * tok;
			*token >> tok;
			if ( *tok == "name" )
			{
				*tok >> name;
			}
			else 
			{
				Global::debug( 3 ) <<"Unhandled menu attribute: "<<endl;
				tok->print(" ");
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

OptionSpeed::~OptionSpeed()
{
	// Nothing
}

void OptionSpeed::logic()
{
	//ostringstream temp;
	char temp[255];
	sprintf( temp, "%s: %0.2f", name.c_str(), MenuGlobals::getGameSpeed() );
	setText(std::string(temp));
}

void OptionSpeed::draw(Bitmap *work)
{
}

void OptionSpeed::run(bool &endGame)
{
}

void OptionSpeed::leftKey()
{
	MenuGlobals::setGameSpeed(MenuGlobals::getGameSpeed() - 0.05);
	if( MenuGlobals::getGameSpeed() < 0.1 )MenuGlobals::setGameSpeed(0.1);
}
void OptionSpeed::rightKey()
{
	MenuGlobals::setGameSpeed(MenuGlobals::getGameSpeed() + 0.05);
}


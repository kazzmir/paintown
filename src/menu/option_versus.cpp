#include "menu/option_versus.h"
#include "util/token.h"
#include "globals.h"

#include <iostream>

using namespace std;

OptionVersus::OptionVersus(Token *token)throw( LoadException ) : MenuOption(event)
{
	if ( *token != "versus" )
		throw LoadException("Not versus");
	
	while ( token->hasTokens() )
	{
		try 
		{
			Token * tok;
			*token >> tok;
			if ( *tok == "name" )
			{
				// Create an image and push it back on to vector
				std::string temp;
				*tok >> temp;
				this->setText(temp);
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
	
	if(getText().empty())throw LoadException("No name set, this option should have a name!");
}

OptionVersus::~OptionVersus()
{
	// Nothing
}

void OptionVersus::logic()
{
}

void OptionVersus::draw(Bitmap *work)
{
}

void OptionVersus::run(bool &endGame)
{
}


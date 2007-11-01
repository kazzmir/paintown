#include "menu/option_adventure.h"
#include "util/token.h"

#include <iostream>

using namespace std;

OptionAdventure::OptionAdventure(Token *token)throw( LoadException ) : MenuOption(event)
{
	if ( *token != "adventure" )
		throw LoadException("Not an adventure");
	
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
				cout<<"Unhandled menu attribute: "<<endl;
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

OptionAdventure::~OptionAdventure()
{
	// Nothing
}

void OptionAdventure::logic()
{
}

void OptionAdventure::draw(Bitmap *work)
{
}

void OptionAdventure::run(bool &endGame)
{
}


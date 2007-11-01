#include "menu/option_credits.h"
#include "util/token.h"

#include <iostream>

using namespace std;

OptionCredits::OptionCredits(Token *token)throw( LoadException ) : MenuOption(event)
{
	if ( *token != "credits" )
		throw LoadException("Not a credit menu");
	
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

OptionCredits::~OptionCredits()
{
	// Nothing
}

void OptionCredits::logic()
{
}

void OptionCredits::draw(Bitmap *work)
{
}

void OptionCredits::run(bool &endGame)
{
}


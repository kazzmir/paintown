#include "menu/option_background.h"
#include "util/bitmap.h"
#include "util/token.h"
#include "util/funcs.h"
#include "globals.h"

OptionBg::OptionBg(Token *token)throw( LoadException ) : MenuOption(event) , animDelay(5)
{
	if ( *token != "background" )
		throw LoadException("Not a background");
	
	while ( token->hasTokens() )
	{
		try 
		{
			Token * tok;
			*token >> tok;
			if ( *tok == "image" )
			{
				// Create an image and push it back on to vector
				std::string temp;
				*tok >> temp;
				Bitmap *bmp = new Bitmap(Util::getDataPath() +temp);
				if(!bmp->getError())images.push_back(bmp);
				else delete bmp;
			}
			else if( *tok == "delay" )
			{
				*tok >> animDelay;
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
}

OptionBg::~OptionBg()
{
	// Nothing
	while(!images.empty())
	{
		delete images.back();
		images.pop_back();
	}
}

void OptionBg::logic()
{
}

void OptionBg::draw(Bitmap *work)
{
	// Need to set up animation if more than 1 image
	if(!images.empty())
	{
		images[0]->draw(0,0,*work);
	}
}

void OptionBg::run(bool &endGame)
{
}


#include "util/bitmap.h"
#include "menu/option_background.h"
#include "util/token.h"
#include "util/funcs.h"
#include "globals.h"
#include "init.h"

OptionBg::OptionBg(Token *token)throw( LoadException ) : MenuOption(token, event) , animDelay(5)
{
	if ( *token != "background" )
		throw LoadException("Not a background");
	
	while ( token->hasTokens() ){
		try{
			Token * tok;
			*token >> tok;
			if ( *tok == "image" ){
				// Create an image and push it back on to vector
				std::string temp;
				*tok >> temp;

                                /*
                                 * just testing the grey scale code..
                                 *
				Bitmap * bmp = new Bitmap(Util::getDataPath() + temp);
				if (!bmp->getError()){
                                    images.push_back(new Bitmap(bmp->greyScale()));
                                }
                                delete bmp;
                                */

				Bitmap *bmp = new Bitmap(Util::getDataPath() +temp);
				if (!bmp->getError()){
                                    images.push_back(bmp);
                                } else {
                                    delete bmp;
                                }
			} else if( *tok == "delay" ) {
				*tok >> animDelay;
			} else {
				Global::debug( 3 ) << "Unhandled menu attribute: "<<endl;
				tok->print(" ");
			}
		} catch ( const TokenException & ex ) {
			// delete current;
			string m( "Menu parse error: " );
			m += ex.getReason();
			throw LoadException( m );
		} catch ( const LoadException & ex ) {
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
		if(images[0]->getWidth() < GFX_X || images[0]->getHeight() < GFX_Y)
		{
			work->rectangle(images[0]->getWidth(),images[0]->getHeight(),GFX_X,GFX_Y,Bitmap::makeColor(0,0,0));
		}
	}
}

void OptionBg::run(bool &endGame)
{
}


Bitmap *OptionBg::getCurrentBackground()
{
	// Fix later! For now lets just give the first background
	return images.front();
}

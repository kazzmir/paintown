#include "menu/menu_option.h"
#include "util/bitmap.h"
#include "util/token.h"
#include "globals.h"

MenuOption::MenuOption( Token *token, const type t)throw( LoadException ) : currentState(Deselected), text(""), infoText(""), bmp(0), adjustLeftColor(Bitmap::makeColor( 255, 255, 255 )), adjustRightColor(Bitmap::makeColor( 255, 255, 255 )), runnable(true), ID(0)
{
	setType(t);
	
	Token _token(*token);
	
	while ( _token.hasTokens() ){
		try{
			Token * tok;
			_token >> tok;
			if ( *tok == "info" ){
				// get info text and location and add to option
				std::string temp;
				int x, y;
				*tok >> temp >> x >> y;
				setInfoText(temp);
				setInfoTextLocation(x,y);
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

MenuOption::~MenuOption()
{
	// Nothing
}


// This is to pass paramaters to an option ie a bar or something
bool MenuOption::leftKey()
{
	return false;
}
bool MenuOption::rightKey()
{
	return false;
}

		


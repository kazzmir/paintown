#include "util/bitmap.h"
#include "menu/option_lives.h"
#include "util/token.h"
#include "menu/menu.h"
#include "menu/menu_global.h"
#include "globals.h"

OptionLives::OptionLives( Token * token ) throw( LoadException ):
MenuOption(token, AdjustableOption),
name(""),
lblue(255),
lgreen(255),
rblue(255),
rgreen(255){
	setRunnable(false);
	
	if ( *token != "lives" ){
		throw LoadException( "Not lives option" );
	}
	
	while ( token->hasTokens() ){
		try{
			Token * tok;
			*token >> tok;
			if ( *tok == "name" ){
				*tok >> name;
			} else {
				Global::debug( 3 ) << "Unhandled menu attribute: " << endl;
                                if (Global::getDebug() >= 3){
                                    tok->print(" ");
                                }
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
	
	if ( name.empty() ){
		throw LoadException("No name set, this option should have a name!");
	}
}

OptionLives::~OptionLives(){
	// Nothing
}

void OptionLives::logic(){
	//ostringstream temp;
	char temp[255];
	sprintf( temp, "%s: %d", name.c_str(), MenuGlobals::getLives() );
	setText(std::string(temp));
	
	if ( lblue < 255 ){
		lblue += 5;
	}

	if ( rblue < 255 ){
		rblue += 5;
	}

	if ( lgreen < 255 ){
		lgreen += 5;
	}

	if ( rgreen < 255 ){
		rgreen += 5;
	}
	
	setLeftAdjustColor(Bitmap::makeColor( 255, lblue, lgreen ));
	setRightAdjustColor(Bitmap::makeColor( 255, rblue, rgreen ));
}

void OptionLives::draw(Bitmap *work){
}

void OptionLives::run(bool &endGame){
}

bool OptionLives::leftKey(){
	MenuGlobals::setLives(MenuGlobals::getLives() - 1);
	if ( MenuGlobals::getLives() < 1 ){
		MenuGlobals::setLives(1);
	}
	
	lblue = lgreen = 0;
	return false;
}

bool OptionLives::rightKey(){
	MenuGlobals::setLives( MenuGlobals::getLives() + 1 );
	rblue = rgreen = 0;
	return false;
}

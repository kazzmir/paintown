#include "util/bitmap.h"
#include "menu/tab_menu.h"
#include "menu/menu_global.h"
#include "menu/menu_option.h"
#include "util/funcs.h"
#include "util/sound.h"
#include "util/font.h"
#include "util/token.h"
#include "util/tokenreader.h"
#include "resource.h"
#include "globals.h"
#include "init.h"
#include "configuration.h"
#include "music.h"

#include "menu/optionfactory.h"
#include "menu/actionfactory.h"
#include "menu/menu_global.h"
#include "menu/menu_animation.h"
#include "gui/keyinput_manager.h"

#include <queue>
#include <map>

using namespace std;

TabMenu::TabMenu(){
	
}

void TabMenu::load(Token *token)throw( LoadException ){
	if ( *token != "tabmenu" )
		throw LoadException("Not a tabbed menu");
	else if ( ! token->hasTokens() )
		return;
	
	while ( token->hasTokens() ){
		try{
			Token * tok;
			*token >> tok;
			if ( *tok == "name" ){
				// Set menu name
				*tok >> name;
			} else if ( *tok == "position" ) {
				// This handles the placement of the menu list and surrounding box
				*tok >> backboard.position.x >> backboard.position.y >> backboard.position.width >> backboard.position.height;
			} else if ( *tok == "position-body" ) {
				// This handles the body color of the menu box
				int r,g,b;
				*tok >> r >> g >> b >> backboard.position.bodyAlpha;
				backboard.position.body = Bitmap::makeColor(r,g,b);
			} else if ( *tok == "position-border" ) {
				// This handles the border color of the menu box
				int r,g,b;
				*tok >> r >> g >> b >> backboard.position.borderAlpha;
				backboard.position.border = Bitmap::makeColor(r,g,b);
			} else {
				Global::debug( 3 ) <<"Unhandled menu attribute: "<<endl;
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
		throw LoadException("No name set, the menu should have a name!");
	}
}

void TabMenu::load(const std::string &filename) throw (LoadException){
    // Must check for initial token, menu
    try{
        TokenReader tr( filename );
        Token * token = tr.readToken();
        load(token);
    } catch (const TokenException & e){
        throw LoadException(e.getReason());
    }
}

void TabMenu::run(){
    Bitmap screen_buffer(320, 240);
    bool done = false;
    bool endGame = false;

    if ( menus.empty() ){
        return;
    }

    double runCounter = 0;
    Global::speed_counter = 0;
    Global::second_counter = 0;
    int game_time = 100;
    while ( ! done ){

	bool draw = false;
	const char vi_up = 'k';
	const char vi_down = 'j';
	const char vi_left = 'h';
	const char vi_right = 'l';

	keyInputManager::update();

	if ( Global::speed_counter > 0 ){
	    draw = true;
	    runCounter += Global::speed_counter * Global::LOGIC_MULTIPLIER;
	    while ( runCounter >= 1.0 ){
		runCounter -= 1;
		// Keys

		if (keyInputManager::keyState(keys::LEFT, true) ||
		    keyInputManager::keyState(vi_left, true)){
		    MenuGlobals::playSelectSound();
		}

		if ( keyInputManager::keyState(keys::RIGHT, true )||
			keyInputManager::keyState(vi_right, true )){
		    MenuGlobals::playSelectSound();
		}

		if ( keyInputManager::keyState(keys::ENTER, true ) ){
		    
		}
	    }

	    Global::speed_counter = 0;
	}

	while ( Global::second_counter > 0 ){
	    game_time--;
	    Global::second_counter--;
	    if ( game_time < 0 ){
		game_time = 0;
	    }
	}

	if ( draw ){
	    // Draw
	    
	    // Finally render to screen
	    work->BlitToScreen();
	}

	while ( Global::speed_counter < 1 ){
	    Util::rest( 1 );
	    keyInputManager::update();
	}

	done |= keyInputManager::keyState(keys::ESC, true );
    }
}

TabMenu::~TabMenu(){
    /*
	// cleanup
	std::vector <MenuOption *>::iterator b = menuOptions.begin();
	std::vector <MenuOption *>::iterator e = menuOptions.end();
	for(;b!=e;++b){
		if((*b))delete (*b);
	}
	if( background )delete background;
	
	for (std::vector<MenuAnimation *>::iterator i = backgroundAnimations.begin(); i != backgroundAnimations.end(); ++i){
	    if (*i){
		delete *i;
	    }
	}
	for (std::vector<MenuAnimation *>::iterator i = foregroundAnimations.begin(); i != foregroundAnimations.end(); ++i){
	    if (*i){
		delete *i;
	    }
	}
	*/
}

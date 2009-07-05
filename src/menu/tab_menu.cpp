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
	
	/*if( !option){
	    if( !getMenu(parentMenu) ){
	      throw LoadException("\"main\" menu not found, the top level directory must be named \"main\"!");
	    }
	}*/
	
    } catch (const TokenException & e){
        throw LoadException(e.getReason());
    }
}

void TabMenu::run(){
/*
    Bitmap screen_buffer(320, 240);
    bool done = false;
    bool endGame = false;

    if ( menuOptions.empty() ){
        return;
    }

    selectedOption = menuOptions.begin();
    menuOptions.front()->setState(MenuOption::Selected);

    if ( !music.empty() ){
        MenuGlobals::setMusic(music);
    }

    if ( !selectSound.empty() ){
        MenuGlobals::setSelectSound(selectSound);
    }

    double runCounter = 0;
    while( ! endGame ){
        Global::speed_counter = 0;
        Global::second_counter = 0;
        int game_time = 100;
        motion = 0;

        // Reset fade stuff
        resetFadeInfo();

        // Reset animations
        for (std::vector<MenuAnimation *>::iterator i = backgroundAnimations.begin(); i != backgroundAnimations.end(); ++i){
            (*i)->reset();
        }
        for (std::vector<MenuAnimation *>::iterator i = foregroundAnimations.begin(); i != foregroundAnimations.end(); ++i){
            (*i)->reset();
        }

        while ( ! done && (*selectedOption)->getState() != MenuOption::Run ){

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

                    if ( keyInputManager::keyState(keys::UP, true ) ||
                            // for vi people like me
                            keyInputManager::keyState(vi_up, true )){	

                        motion -= getFontHeight();
                        (*selectedOption)->setState(MenuOption::Deselected);
                        if (selectedOption > menuOptions.begin()){
                            selectedOption--;
                        } else {
                            selectedOption = menuOptions.end() -1;
                        }

                        (*selectedOption)->setState(MenuOption::Selected);
                        (*selectedOption)->resetAnimations();

                        if (menuOptions.size() > 1){
                            MenuGlobals::playSelectSound();
                        }
                    }

                    if ( keyInputManager::keyState(keys::DOWN, true ) ||
                           
                            keyInputManager::keyState(vi_down, true )){

                        motion += getFontHeight();
                        (*selectedOption)->setState(MenuOption::Deselected);
                        if (selectedOption < menuOptions.begin()+menuOptions.size()-1){
                            selectedOption++;
                        } else {
                            selectedOption = menuOptions.begin();
                        }

                        (*selectedOption)->setState(MenuOption::Selected);
                        (*selectedOption)->resetAnimations();

                        if (menuOptions.size() > 1){
                            MenuGlobals::playSelectSound();
                        }
                    }

                    if (keyInputManager::keyState(keys::LEFT, true) ||
                        keyInputManager::keyState(vi_left, true)){

                        if ((*selectedOption)->leftKey()){
                       
                        }
                    }

                    if ( keyInputManager::keyState(keys::RIGHT, true )||
                         keyInputManager::keyState(vi_right, true )){

                        if ((*selectedOption)->rightKey()){
                       
                        }
                    }

                    if ( keyInputManager::keyState(keys::ENTER, true ) ){
                        if ((*selectedOption)->isRunnable()){
                            (*selectedOption)->setState(MenuOption::Run);
                        }
                    }

                    std::vector <MenuOption *>::iterator b = menuOptions.begin();
                    std::vector <MenuOption *>::iterator e = menuOptions.end();
                    for ( ; b != e; b++ ){
                        (*b)->logic();

                        // Recalculate placement
                        checkTextLength((*b));
                    }

                    const double motion_speed = 1.8;
                    if (motion >= motion_speed){
                        motion -= motion_speed;
                    } else if (motion <= -motion_speed){
                        motion += motion_speed;
                    } else {
                        motion = 0;
                    }

                    // motion = 0;

                    // Current option animation logic
                    (*selectedOption)->updateAnimations();

                    // Animations
                    for (std::vector<MenuAnimation *>::iterator i = backgroundAnimations.begin(); i != backgroundAnimations.end(); ++i){
                        (*i)->act();
                    }
                    for (std::vector<MenuAnimation *>::iterator i = foregroundAnimations.begin(); i != foregroundAnimations.end(); ++i){
                        (*i)->act();
                    }

                    // Lets do some logic for the box with text
                    switch ( currentDrawState ){
                        case FadeIn : {
                            if (fadeBox.position.x> backboard.position.x){
                                fadeBox.position.x -= fadeSpeed;
                            } else if ( fadeBox.position.x < backboard.position.x ){
                                fadeBox.position.x = backboard.position.x;
                            }

                            if (fadeBox.position.y > backboard.position.y){
                                fadeBox.position.y -= fadeSpeed;
                            } else if (fadeBox.position.y<backboard.position.y){
                                fadeBox.position.y = backboard.position.y;
                            }

                            if (fadeBox.position.width<backboard.position.width){
                                fadeBox.position.width += (fadeSpeed*2);
                            } else if (fadeBox.position.width>backboard.position.width){
                                fadeBox.position.width = backboard.position.width;
                            }

                            if (fadeBox.position.height<backboard.position.height){
                                fadeBox.position.height += (fadeSpeed*2);
                            } else if (fadeBox.position.height>backboard.position.height){
                                fadeBox.position.height = backboard.position.height;
                            }

                            if (fadeBox.position == backboard.position){
                                currentDrawState = FadeInText;
                            }

                            break;
                        }
                        case FadeInText : {
                            if (fadeAlpha < 255){
                                fadeAlpha += (fadeSpeed+2);
                            }

                            if (fadeAlpha >= 255){
                                fadeAlpha = 255;
                                currentDrawState = NoFade;
                            }
                            break;
                        }
                        case NoFade : {
                            break;
                        }
                        default : {
                            break;
                        }
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

                // Do the background
                drawBackground(work);
                // Do background animations
                for (std::vector<MenuAnimation *>::iterator i = backgroundAnimations.begin(); i != backgroundAnimations.end(); ++i){
                    (*i)->draw(work);
                }
                // Draw any misc stuff in the background of the menu of selected object 
                (*selectedOption)->drawBelow(work);
                // Draw text board
                drawTextBoard(work);
                // Draw text
                drawText(work);
                // Draw info text
                drawInfoText(work);
                // Draw foreground animations
                for (std::vector<MenuAnimation *>::iterator i = foregroundAnimations.begin(); i != foregroundAnimations.end(); ++i){
                    (*i)->draw(work);
                }
                // Draw any misc stuff in the foreground of the menu of selected object 
                (*selectedOption)->drawAbove(work);
                // Finally render to screen
                work->BlitToScreen();
            }

            while ( Global::speed_counter < 1 ){
                Util::rest( 1 );
                keyInputManager::update();
            }

            // endGame |= keyInputManager::keyState(keys::ESC, true );
            done |= endGame;
            if (keyInputManager::keyState(keys::ESC, true )){
                (*selectedOption)->setState(MenuOption::Deselected);
                if (backSound != ""){
                    Sound * back = Resource::getSound(backSound);
                    back->play();
                }
                throw ReturnException();
            }
        }

        // do we got an option to run, lets do it
        if ((*selectedOption)->getState() == MenuOption::Run){
            try{
                if (backSound != ""){
                    Sound * ok = Resource::getSound(okSound);
                    ok->play();
                }
                (*selectedOption)->run(endGame);
            } catch ( const ReturnException & re ){
            }
            // Reset it's state
            (*selectedOption)->setState(MenuOption::Selected);
            if ( !music.empty() ){
                MenuGlobals::setMusic(music);
            }
            if ( !selectSound.empty() ){
                MenuGlobals::setSelectSound(selectSound);
            }
        }

        if (!music.empty()){
            if(MenuGlobals::currentMusic() != music){
                MenuGlobals::popMusic();
            }
        }

        if (!selectSound.empty()){
            if(MenuGlobals::currentSelectSound() != selectSound){
                MenuGlobals::popSelectSound();
            }
        }
    }
    */
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

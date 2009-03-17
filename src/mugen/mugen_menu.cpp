#include "mugen/mugen_menu.h"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <string>
#include <cstring>
#include <vector>
#include <ostream>
#include <sstream>
#include <iostream>

#include "mugen_stage.h"

#include "init.h"
#include "resource.h"
#include "util/funcs.h"
#include "util/bitmap.h"
#include "game/console.h"
#include "object/animation.h"
#include "object/object.h"
#include "object/character.h"
#include "object/object_attack.h"
#include "object/player.h"
#include "globals.h"
#include "factory/font_render.h"
#include "menu/menu_option.h"
#include "menu/menu_global.h"
#include "gui/keyinput_manager.h"
#include "gui/keys.h"

#include "mugen_animation.h"
#include "mugen_background.h"
#include "mugen_item.h"
#include "mugen_item_content.h"
#include "mugen_section.h"
#include "mugen_sound.h"
#include "mugen_reader.h"
#include "mugen_sprite.h"
#include "mugen_util.h"

MugenMenu::MugenMenu(const std::string &filename):
location(filename){
}

void MugenMenu::load() throw (LoadException){
     // Lets look for our def since some assholes think that all file systems are case insensitive
    std::string baseDir = Util::getDataPath() + "mugen/stages/";
    Global::debug(1) << baseDir << endl;
    location+=".def";
    const std::string ourDefFile = MugenUtil::fixFileName( baseDir, std::string(location) );
    
    if( ourDefFile.empty() )throw MugenException( "Cannot locate stage definition file for: " + location );
    
    std::string filesdir = "";
    
    size_t strloc = location.find_last_of("/");
    if (strloc != std::string::npos){
	filesdir = location.substr(0, strloc);
	filesdir += "/";
    }
    
    Global::debug(1) << "Got subdir: " << filesdir << endl;
     
    MugenReader reader( ourDefFile );
    std::vector< MugenSection * > collection;
    collection = reader.getCollection();
    
    // for linked position in backgrounds
    //MugenBackground *prior = 0;
    
    /* Extract info for our first section of our stage */
    for( unsigned int i = 0; i < collection.size(); ++i ){
	/*std::string head = collection[i]->getHeader();
	MugenUtil::fixCase(head);
	if( head == "info" ){
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		MugenUtil::removeSpaces(itemhead);
		if ( itemhead.find("name")!=std::string::npos ){
		    *content->getNext() >> name;
                    Global::debug(1) << "Read name '" << name << "'" << endl;
		} else if ( itemhead.find("author")!=std::string::npos ){
		    // Seems to be that some people think that author belongs in background defs
		    std::string temp;
		    *content->getNext() >> temp;
                    Global::debug(1) << "Made by this guy: '" << temp << "'" << endl;
		} else throw MugenException( "Unhandled option in Info Section: " + itemhead );
	    }
	}*/
	//else if( head == "music" ){ /* Ignore for now */ }
	//else throw MugenException( "Unhandled Section in '" + ourDefFile + "': " + head ); 
    }
}

MugenMenu::~MugenMenu(){
    
}

void MugenMenu::run(){
    Bitmap screen_buffer( 320, 240 );
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
	    
	    /*
	    sharedFont = ourFont;
	    sharedFontWidth = fontWidth;
	    sharedFontHeight = fontHeight;
	    */
	    
	    // Reset fade stuff
	   // resetFadeInfo();
	    
	    while ( ! done && (*selectedOption)->getState() != MenuOption::Run ){
    
		    bool draw = false;
		    
		    keyInputManager::update();
    
		    if ( Global::speed_counter > 0 ){
			    draw = true;
			    runCounter += Global::speed_counter * Global::LOGIC_MULTIPLIER;
			    while ( runCounter >= 1.0 ){
				runCounter -= 1;
				// Keys
				
				if ( keyInputManager::keyState(keys::UP, true ) ||
					/* for vi people like me */
				    keyInputManager::keyState('k', true )){	
					(*selectedOption)->setState(MenuOption::Deselected);
					if ( selectedOption > menuOptions.begin() ){
						selectedOption--;
					}
					else selectedOption = menuOptions.end() -1;
					(*selectedOption)->setState(MenuOption::Selected);
					if(menuOptions.size() > 1)MenuGlobals::playSelectSound();
				}

				if ( keyInputManager::keyState(keys::DOWN, true ) ||
					/* for vi people like me */
				    keyInputManager::keyState('j', true )){
					(*selectedOption)->setState(MenuOption::Deselected);
					if ( selectedOption < menuOptions.begin()+menuOptions.size()-1 ){
						selectedOption++;
					}
					else selectedOption = menuOptions.begin();
					(*selectedOption)->setState(MenuOption::Selected);
					if(menuOptions.size() > 1)MenuGlobals::playSelectSound();
				}
				
				if ( keyInputManager::keyState(keys::LEFT, true) ||
				    keyInputManager::keyState('h', true)){
					if ( (*selectedOption)->leftKey()){
					    /* ??? */
					}
				}
				
				if ( keyInputManager::keyState(keys::RIGHT, true )||
				    keyInputManager::keyState('l', true )){
					if ( (*selectedOption)->rightKey()){
					    /* ??? */
					}
				}
				
				if ( keyInputManager::keyState(keys::ENTER, true ) ){
					if((*selectedOption)->isRunnable())(*selectedOption)->setState( MenuOption::Run );
				}
				
				std::vector <MenuOption *>::iterator b = menuOptions.begin();
				std::vector <MenuOption *>::iterator e = menuOptions.end();
				for ( ; b != e; b++ ){
					(*b)->logic();
					
					// Recalculate placement
					//checkTextLength((*b));
				}
				
				// Lets do some logic for the box with text
				/*switch ( currentDrawState ){
					case FadeIn : {

						if ( fadeBox.position.x> backboard.position.x){
							fadeBox.position.x -= fadeSpeed;
						} else if ( fadeBox.position.x < backboard.position.x ){
							fadeBox.position.x = backboard.position.x;
						}

						if ( fadeBox.position.y > backboard.position.y ){
								fadeBox.position.y-=fadeSpeed;
						} else if ( fadeBox.position.y<backboard.position.y ){
								fadeBox.position.y=backboard.position.y;
						}

						if(fadeBox.position.width<backboard.position.width)fadeBox.position.width+=(fadeSpeed*2);
						else if(fadeBox.position.width>backboard.position.width)fadeBox.position.width=backboard.position.width;
						if(fadeBox.position.height<backboard.position.height)fadeBox.position.height+=(fadeSpeed*2);
						else if(fadeBox.position.height>backboard.position.height)fadeBox.position.height=backboard.position.height;
						if(fadeBox.position == backboard.position)currentDrawState = FadeInText;
						break;
					}
					case FadeInText : {
						if ( fadeAlpha<255 ){
							fadeAlpha+=(fadeSpeed+2);
						}

						if ( fadeAlpha >= 255 ){
							fadeAlpha=255;
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
				}*/
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
			    work->clear();
			    // Draw
			    
			    // Do the background
			    //drawBackground(work);
			    
			    // Draw any misc stuff in the background of the menu of selected object 
			    (*selectedOption)->draw(work);
			    // Draw text board
			    //drawTextBoard(work);
			    // Draw text
			    //drawText(work);
			    // Draw info text
			    //drawInfoText(work);
			    // Finally render to screen
			    work->BlitToScreen();
		    }
    
		    while ( Global::speed_counter < 1 ){
			    Util::rest( 1 );
			    keyInputManager::update();
		    }
    
		    endGame = done |= keyInputManager::keyState(keys::ESC, true );
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

	    if (endGame){
		    // Deselect selected entry
		    (*selectedOption)->setState(MenuOption::Deselected);
		    if (backSound != ""){
			Sound * back = Resource::getSound(backSound);
			back->play();
		    }
	    }
    }
    
    return;
}




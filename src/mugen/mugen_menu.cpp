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
#include "mugen_font.h"

const int DEFAULT_WIDTH = 320;
const int DEFAULT_HEIGHT = 240;
const int DEFAULT_SCREEN_X_AXIS = 160;
const int DEFAULT_SCREEN_Y_AXIS = 0;

MugenMenu::MugenMenu(const std::string &filename):
location(filename),
spriteFile(""),
soundFile(""),
logoFile(""),
introFile(""),
selectFile(""),
fightFile(""),
fadeInTime(0),
fadeOutTime(0),
windowVisibleItems(0),
showBoxCursor(false),
backgroundClearColor(Bitmap::makeColor(0,0,0)),
ticker(0){
}

void MugenMenu::load() throw (MugenException){
     // Lets look for our def since some assholes think that all file systems are case insensitive
    std::string baseDir = Util::getDataPath() + "mugen/data/";
    Global::debug(1) << baseDir << endl;
    const std::string ourDefFile = MugenUtil::fixFileName( baseDir, std::string(location) );
    
    if( ourDefFile.empty() )throw MugenException( "Cannot locate menu definition file for: " + location );
    
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
    MugenBackground *prior = 0;
    
    /* Extract info for our first section of our stage */
    for( unsigned int i = 0; i < collection.size(); ++i ){
	std::string head = collection[i]->getHeader();
	MugenUtil::fixCase(head);
	if( head == "info" ){
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		MugenUtil::removeSpaces(itemhead);
		if ( itemhead.find("name")!=std::string::npos ){
		    std::string temp;
		    *content->getNext() >> temp;
		    setName(temp);
                    Global::debug(1) << "Read name '" << getName() << "'" << endl;
		} else if ( itemhead.find("author")!=std::string::npos ){
		    std::string temp;
		    *content->getNext() >> temp;
                    Global::debug(1) << "Made by: '" << temp << "'" << endl;
		} else throw MugenException( "Unhandled option in Info Section: " + itemhead );
	    }
	}
	else if( head == "files" ){
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		MugenUtil::removeSpaces(itemhead);
		MugenUtil::fixCase(itemhead);
		if ( itemhead.find("spr")!=std::string::npos ){
		    *content->getNext() >> spriteFile;
		    Global::debug(1) << "Got Sprite File: '" << spriteFile << "'" << endl;
		    MugenUtil::readSprites( MugenUtil::fixFileName(baseDir + filesdir, spriteFile), "", sprites );
		} else if ( itemhead.find("snd")!=std::string::npos ){
		    *content->getNext() >> soundFile;
                    Global::debug(1) << "Got Sound File: '" << soundFile << "'" << endl;
		} else if ( itemhead.find("logo.storyboard")!=std::string::npos ){
		    *content->getNext() >> logoFile;
                    Global::debug(1) << "Got Logo Storyboard File: '" << logoFile << "'" << endl;
		} else if ( itemhead.find("intro.storyboard")!=std::string::npos ){
		    *content->getNext() >> introFile;
                    Global::debug(1) << "Got Intro Storyboard File: '" << introFile << "'" << endl;
		} else if ( itemhead.find("select")!=std::string::npos ){
		    *content->getNext() >> selectFile;
                    Global::debug(1) << "Got Select File: '" << selectFile << "'" << endl;
		} else if ( itemhead.find("fight")!=std::string::npos ){
		    *content->getNext() >> fightFile;
                    Global::debug(1) << "Got Fight File: '" << fightFile << "'" << endl;
		} else if ( itemhead.find("font")!=std::string::npos ){
		    std::string temp;
		    *content->getNext() >> temp;
		    MugenUtil::removeSpaces(temp);
		    fonts.push_back(new MugenFont(temp));
                    Global::debug(1) << "Got Font File: '" << temp << "'" << endl;
		} else throw MugenException( "Unhandled option in Files Section: " + itemhead );
	    }
	}
	else if( head == "title info" ){
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		MugenUtil::removeSpaces(itemhead);
		MugenUtil::fixCase(itemhead);
		if ( itemhead.find("fadein.time")!=std::string::npos ){
		    *content->getNext() >> fadeInTime;
		} else if ( itemhead.find("fadeout.time")!=std::string::npos ){
		    *content->getNext() >> fadeOutTime;
		} else if ( itemhead.find("menu.pos")!=std::string::npos ){
		    *content->getNext() >> position.x;
		    *content->getNext() >> position.y;
		} else if ( itemhead.find("menu.item.font")!=std::string::npos ){
		    *content->getNext() >> fontItem.index;
		    *content->getNext() >> fontItem.bank;
		    *content->getNext() >> fontItem.position;
		} else if ( itemhead.find("menu.item.active.font")!=std::string::npos ){
		    *content->getNext() >> fontActive.index;
		    *content->getNext() >> fontActive.bank;
		    *content->getNext() >> fontActive.position;
		} else if ( itemhead.find("menu.item.spacing")!=std::string::npos ){
		    *content->getNext() >> fontSpacing.x;
		    *content->getNext() >> fontSpacing.y;
		} else if ( itemhead.find("menu.itemname.")!=std::string::npos ){
		    // Add in info to setting individual options in the menu
		} else if ( itemhead.find("menu.window.margins.x")!=std::string::npos ){
		    *content->getNext() >> windowMarginX.x;
		    *content->getNext() >> windowMarginX.y;
		} else if ( itemhead.find("menu.window.margins.y")!=std::string::npos ){
		    *content->getNext() >> windowMarginY.x;
		    *content->getNext() >> windowMarginY.y;
		} else if ( itemhead.find("menu.window.visibleitems")!=std::string::npos ){
		    *content->getNext() >> windowVisibleItems;
		} else if ( itemhead.find("menu.boxcursor.visible")!=std::string::npos ){
		    *content->getNext() >> showBoxCursor;
		} else if ( itemhead.find("menu.boxcursor.coords")!=std::string::npos ){
		    *content->getNext() >> boxCursorCoords.x1;
		    *content->getNext() >> boxCursorCoords.y1;
		    *content->getNext() >> boxCursorCoords.x2;
		    *content->getNext() >> boxCursorCoords.y2;
		} else if ( itemhead.find("cursor.move.snd")!=std::string::npos ){
		    // Configure later
		} else if ( itemhead.find("cursor.done.snd")!=std::string::npos ){
		    // Configure later
		} else if ( itemhead.find("cancel.snd")!=std::string::npos ){
		    // Configure later
		} else throw MugenException( "Unhandled option in Info Section: " + itemhead );
	    }
	}
	else if( head == "titlebgdef" ){
	    while( collection[i]->hasItems() ){
		MugenItemContent *content = collection[i]->getNext();
		const MugenItem *item = content->getNext();
		std::string itemhead = item->query();
		MugenUtil::removeSpaces(itemhead);
		if ( itemhead.find("bgclearcolor")!=std::string::npos ){
		    int r,g,b;
		    *content->getNext() >> r;
		    *content->getNext() >> g;
		    *content->getNext() >> b;
		    backgroundClearColor = Bitmap::makeColor(r,g,b);
		} else throw MugenException( "Unhandled option in Info Section: " + itemhead );
	    }
	}
	// This our background data definitions for the title
	else if( head.find("titlebg ") !=std::string::npos ){
	    MugenBackground *temp = MugenUtil::getBackground(ticker, collection[i], sprites);
	    // Do some fixups and necessary things
	    // lets see where we lay
	    backgrounds.push_back(temp);
	    
	    // If position link lets set to previous item
	    if( temp->positionlink ){
		temp->linked = prior;
		Global::debug(1) << "Set positionlink to id: '" << prior->id << "' Position at x(" << prior->startx << ")y(" << prior->starty << ")" << endl;
	    } 
	    
	    // This is so we can have our positionlink info for the next item if true
	    prior = temp;
	}
	else if( head == "select info" ){ /* Ignore for now */ }
	else if( head == "selectbgdef" ){ /* Ignore for now */ }
	else if( head.find("selectbg ") != std::string::npos ){ /* Ignore for now */ }
	else if( head == "music" ){ /* Ignore for now */ }
	else if( head == "vs screen" ){ /* Ignore for now */ }
	else if( head == "versusbgdef" ){ /* Ignore for now */ }
	else if( head.find("versusbg " ) != std::string::npos ){ /* Ignore for now */ }
	else if( head == "demo mode" ){ /* Ignore for now */ }
	else if( head == "continue screen" ){ /* Ignore for now */ }
	else if( head == "game over screen" ){ /* Ignore for now */ }
	else if( head == "win screen" ){ /* Ignore for now */ }
	else if( head == "default ending" ){ /* Ignore for now */ }
	else if( head == "end credits" ){ /* Ignore for now */ }
	else if( head == "survival results screen" ){ /* Ignore for now */ }
	else if( head == "option info" ){ /* Ignore for now */ }
	else if( head == "optionbgdef" ){ /* Ignore for now */ }
	else if( head.find("optionbg ") != std::string::npos ){ /* Ignore for now */ }
	else if( head == "music" ){ /* Ignore for now */ }
	else if( head.find("begin action ") != std::string::npos ){ /* Ignore for now */ }
	else throw MugenException( "Unhandled Section in '" + ourDefFile + "': " + head ); 
    }
    // Set up the animations for those that have action numbers assigned (not -1 )
    // Also do their preload
    for( std::vector< MugenBackground * >::iterator i = backgrounds.begin(); i != backgrounds.end(); ++i ){
	if( (*i)->actionno != -1 ){
	    (*i)->action = animations[ (*i)->actionno ];
	    // Check tilespacing and mask
	    if ((*i)->tilespacingx == 0){
		(*i)->tilespacingx = 1;
	    }
	    if ((*i)->tilespacingy == 0){
		(*i)->tilespacingy = 1;
	    }
	    (*i)->mask = true;
	}
	// now load
	(*i)->preload( DEFAULT_SCREEN_X_AXIS, DEFAULT_SCREEN_Y_AXIS );
    }
}

MugenMenu::~MugenMenu(){
    // cleanup
    cleanup();
}

void MugenMenu::run(){
    bool done = false;
    bool endGame = false;
    /*
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
    */
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
				ticker++;
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
				
				// Backgrounds
				for( vector< MugenBackground *>::iterator i = backgrounds.begin(); i != backgrounds.end(); ++i ){
				    (*i)->logic( 0, 0, DEFAULT_SCREEN_X_AXIS, DEFAULT_SCREEN_Y_AXIS );
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
			    // Clear
			    work->fill(backgroundClearColor);
			    // Draw
			    
			    // Do the background
			    for( vector< MugenBackground *>::iterator i = backgrounds.begin(); i != backgrounds.end(); ++i ){
				(*i)->render( 0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT, work );
			    }
			    
			    // Draw any misc stuff in the background of the menu of selected object 
			    //(*selectedOption)->draw(work);
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
	    /*
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
	    */
    }
    
    return;
}


void MugenMenu::cleanup(){
    // Get rid of sprites
    for( std::map< unsigned int, std::map< unsigned int, MugenSprite * > >::iterator i = sprites.begin() ; i != sprites.end() ; ++i ){
      for( std::map< unsigned int, MugenSprite * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
	  if( j->second )delete j->second;
      }
    }
    
    // Get rid of animation lists;
    for( std::map< int, MugenAnimation * >::iterator i = animations.begin() ; i != animations.end() ; ++i ){
	if( i->second )delete i->second;
    }
    
    // Get rid of background lists;
    for( std::vector< MugenBackground * >::iterator i = backgrounds.begin() ; i != backgrounds.end() ; ++i ){
	if( (*i) )delete (*i);
    }
}



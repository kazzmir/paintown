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
#include "menu/option_quit.h"
#include "menu/option_dummy.h"

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
#include "mugen_storyboard.h"

const int DEFAULT_WIDTH = 320;
const int DEFAULT_HEIGHT = 240;
const int DEFAULT_SCREEN_X_AXIS = 160;
const int DEFAULT_SCREEN_Y_AXIS = 0;

MugenMenu::MugenMenu(const std::string &filename):
optionLocation(0),
location(filename),
spriteFile(""),
soundFile(""),
logoFile(""),
introFile(""),
selectFile(""),
fightFile(""),
windowVisibleItems(0),
showBoxCursor(false),
ticker(0),
background(0),
logo(0),
intro(0){
}

void MugenMenu::load() throw (MugenException){
     // Lets look for our def since some assholes think that all file systems are case insensitive
    std::string baseDir = Util::getDataPath() + "mugen/data/" + MugenUtil::getFileDir(location);
    const std::string ourDefFile = MugenUtil::fixFileName( baseDir, MugenUtil::stripDir(location) );
    // get real basedir
    //baseDir = MugenUtil::getFileDir( ourDefFile );
    Global::debug(1) << baseDir << endl;
    
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
		    MugenUtil::readSprites( MugenUtil::getCorrectFileLocation(baseDir, spriteFile), "", sprites );
		} else if ( itemhead.find("snd")!=std::string::npos ){
		    *content->getNext() >> soundFile;
                    Global::debug(1) << "Got Sound File: '" << soundFile << "'" << endl;
		} else if ( itemhead.find("logo.storyboard")!=std::string::npos ){
		    *content->getNext() >> logoFile;
		    try{
			Global::debug(0) << baseDir << " / " << logoFile << endl;
			logo = new MugenStoryboard(MugenUtil::getCorrectFileLocation(baseDir, logoFile));
			logo->load();
		    }
		    catch (MugenException &ex){
			throw MugenException( "Error loading logo storyboard: " + ex.getReason() );
		    }
                    Global::debug(1) << "Got Logo Storyboard File: '" << logoFile << "'" << endl;
		} else if ( itemhead.find("intro.storyboard")!=std::string::npos ){
		    *content->getNext() >> introFile;
		    try{
			intro = new MugenStoryboard(MugenUtil::getCorrectFileLocation(baseDir, introFile));
			intro->load();
		    }
		    catch (MugenException &ex){
			throw MugenException( "Error loading intro storyboard: " + ex.getReason() );
		    }
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
		    fonts.push_back(new MugenFont(MugenUtil::getCorrectFileLocation(baseDir, temp)));
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
		    int time;
		    *content->getNext() >> time;
		    fader.setFadeInTime(time);
		} else if ( itemhead.find("fadein.color")!=std::string::npos ){
		    int r,g,b;
		    *content->getNext() >> r;
		    *content->getNext() >> g;
		    *content->getNext() >> b;
		    fader.setFadeInColor(Bitmap::makeColor(r,g,b));
		} else if ( itemhead.find("fadeout.time")!=std::string::npos ){
		    int time;
		    *content->getNext() >> time;
		    fader.setFadeOutTime(time);
		} else if ( itemhead.find("fadeout.color")!=std::string::npos ){
		    int r,g,b;
		    *content->getNext() >> r;
		    *content->getNext() >> g;
		    *content->getNext() >> b;
		    fader.setFadeOutColor(Bitmap::makeColor(r,g,b));
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
		   if (itemhead == "menu.itemname.arcade"){
		       std::string temp;
		       *content->getNext() >> temp;
		       if (!temp.empty() && temp != "empty"){
			    OptionDummy *dummy = new OptionDummy(temp);
			    addOption(dummy);
		       }
		   } else if (itemhead == "menu.itemname.versus"){
		       std::string temp;
		       *content->getNext() >> temp;
		       if (!temp.empty() && temp != "empty"){
			    OptionDummy *dummy = new OptionDummy(temp);
			    addOption(dummy);
		       }
		   } else if (itemhead == "menu.itemname.teamarcade"){
		       std::string temp;
		       *content->getNext() >> temp;
		       if (!temp.empty() && temp != "empty"){
			    OptionDummy *dummy = new OptionDummy(temp);
			    addOption(dummy);
		       }
		   } else if (itemhead == "menu.itemname.teamcoop"){
		       std::string temp;
		       *content->getNext() >> temp;
		       if (!temp.empty() && temp != "empty"){
			    OptionDummy *dummy = new OptionDummy(temp);
			    addOption(dummy);
		       }
		   } else if (itemhead == "menu.itemname.survival"){
		       std::string temp;
		       *content->getNext() >> temp;
		       if (!temp.empty() && temp != "empty"){
			    OptionDummy *dummy = new OptionDummy(temp);
			    addOption(dummy);
		       }
		   } else if (itemhead == "menu.itemname.survivalcoop"){
		       std::string temp;
		       *content->getNext() >> temp;
		       if (!temp.empty() && temp != "empty"){
			    OptionDummy *dummy = new OptionDummy(temp);
			    addOption(dummy);
		       }
		   } else if (itemhead == "menu.itemname.training"){
		       std::string temp;
		       *content->getNext() >> temp;
		       if (!temp.empty() && temp != "empty"){
			    OptionDummy *dummy = new OptionDummy(temp);
			    addOption(dummy);
		       }
		   } else if (itemhead == "menu.itemname.watch"){
		       std::string temp;
		       *content->getNext() >> temp;
		       if (!temp.empty() && temp != "empty"){
			    OptionDummy *dummy = new OptionDummy(temp);
			    addOption(dummy);
		       }
		   } else if (itemhead == "menu.itemname.options"){
		       std::string temp;
		       *content->getNext() >> temp;
		       if (!temp.empty() && temp != "empty"){
			    OptionDummy *dummy = new OptionDummy(temp);
			    addOption(dummy);
		       }
		   } else if (itemhead == "menu.itemname.exit"){
		       std::string temp;
		       *content->getNext() >> temp;
		       if (!temp.empty()){
			    OptionQuit *quit = new OptionQuit(temp);
			    addOption(quit);
		       }
		   }
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
		    boxCursorCoords.alpha = 128;
		    boxCursorCoords.alphaMove = -6;
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
	    // Background management
	    MugenBackgroundManager *manager = new MugenBackgroundManager(baseDir,collection, i,ticker,&sprites);
	    background = manager;
	    Global::debug(1) << "Got background: '" << manager->getName() << "'" << endl;
	}
	else if( head == "select info" ){ /* Ignore for now */ }
	else if( head == "selectbgdef" ){ /* Ignore for now */ }
	else if( head.find("selectbg") != std::string::npos ){ /* Ignore for now */ }
	else if( head == "music" ){ /* Ignore for now */ }
	else if( head == "vs screen" ){ /* Ignore for now */ }
	else if( head == "versusbgdef" ){ /* Ignore for now */ }
	else if( head.find("versusbg" ) != std::string::npos ){ /* Ignore for now */ }
	else if( head == "demo mode" ){ /* Ignore for now */ }
	else if( head == "continue screen" ){ /* Ignore for now */ }
	else if( head == "game over screen" ){ /* Ignore for now */ }
	else if( head == "win screen" ){ /* Ignore for now */ }
	else if( head == "default ending" ){ /* Ignore for now */ }
	else if( head == "end credits" ){ /* Ignore for now */ }
	else if( head == "survival results screen" ){ /* Ignore for now */ }
	else if( head == "option info" ){ /* Ignore for now */ }
	else if( head == "optionbgdef" ){ /* Ignore for now */ }
	else if( head.find("optionbg") != std::string::npos ){ /* Ignore for now */ }
	else if( head == "music" ){ /* Ignore for now */ }
	else if( head.find("begin action") != std::string::npos ){ /* Ignore  fornow */ }
	else throw MugenException( "Unhandled Section in '" + ourDefFile + "': " + head ); 
    }
    // Set up the animations for those that have action numbers assigned (not -1 )
    // Also do their preload
    if (background) background->preload(DEFAULT_SCREEN_X_AXIS, DEFAULT_SCREEN_Y_AXIS );
}

MugenMenu::~MugenMenu(){
    // cleanup
    cleanup();
}

void MugenMenu::run(){
    Bitmap workArea(DEFAULT_WIDTH,DEFAULT_HEIGHT);
    bool done = false;
    bool endGame = false;
    
    if ( menuOptions.empty() ){
	    return;
    }
    
    selectedOption = menuOptions.begin();
    optionLocation = 0;
    menuOptions.front()->setState(MenuOption::Selected);
  /*  
    if ( !music.empty() ){
	    MenuGlobals::setMusic(music);
    }
    
    if ( !selectSound.empty() ){
	    MenuGlobals::setSelectSound(selectSound);
    }
    */
  // Set the fade state
  fader.setState(FADEIN);
  
    // Do we have logos or intros?
    // Logo run it no repeat
    if (logo){
	logo->run( work,false);
    }
    // Intro run it no repeat
    if (intro){
	intro->run( work,false);
    }
  
    double runCounter = 0;
    while( ! endGame ){
	    Global::speed_counter = 0;
	    Global::second_counter = 0;
	    int game_time = 100;
	 
	    while ( ! done && (*selectedOption)->getState() != MenuOption::Run && fader.getState() != RUNFADE ){
    
		    bool draw = false;
		    
		    keyInputManager::update();
    
		    if ( Global::speed_counter > 0 ){
			    draw = true;
			    runCounter += Global::speed_counter * Global::LOGIC_MULTIPLIER;
			    while ( runCounter >= 1.0 ){
				ticker++;
				runCounter -= 1;
				// Keys
				if (fader.getState() == NOFADE){
				    if ( keyInputManager::keyState(keys::UP, true ) ||
					    /* for vi people like me */
					keyInputManager::keyState('k', true )){	
					    (*selectedOption)->setState(MenuOption::Deselected);
					    if ( selectedOption > menuOptions.begin() ){
						    selectedOption--;
						    optionLocation--;
					    } else { 
						selectedOption = menuOptions.end() -1;
						optionLocation = menuOptions.size() -1;
					    }
					    (*selectedOption)->setState(MenuOption::Selected);
					    //if(menuOptions.size() > 1)MenuGlobals::playSelectSound();
				    }

				    if ( keyInputManager::keyState(keys::DOWN, true ) ||
					    /* for vi people like me */
					keyInputManager::keyState('j', true )){
					    (*selectedOption)->setState(MenuOption::Deselected);
					    if ( selectedOption < menuOptions.begin()+menuOptions.size()-1 ){
						    selectedOption++;
						    optionLocation++;
					    } else {
						selectedOption = menuOptions.begin();
						optionLocation = 0;
					    }
					    (*selectedOption)->setState(MenuOption::Selected);
					    //if(menuOptions.size() > 1)MenuGlobals::playSelectSound();
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
					    // Set the fade state
					    fader.setState(FADEOUT);
				    }
				    
				    if ( keyInputManager::keyState(keys::ESC, true ) ){
					    endGame = done = true;
					    // Set the fade state
					    fader.setState(FADEOUT);
				    }
				}
				// Fader
				fader.act();
				
				// Options
				for( vector< MenuOption *>::iterator b = menuOptions.begin(); b != menuOptions.end(); ++b ){
					(*b)->logic();
				}
				
				// Backgrounds
				background->logic( 0, 0, 0, 0 );
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
			    // backgrounds
			    background->renderBack(0,0,DEFAULT_WIDTH,DEFAULT_HEIGHT,&workArea);
			    // Draw any misc stuff in the background of the menu of selected object 
			    (*selectedOption)->draw(work);
			    // Draw text
			    drawText(&workArea);
			    // Foregrounds
			    background->renderFront(0,0,DEFAULT_WIDTH,DEFAULT_HEIGHT,&workArea);
			    // Do fades
			    fader.draw(&workArea);
			    // Finally render to screen
			    workArea.Stretch(*work);
			    work->BlitToScreen();
		    }
    
		    while ( Global::speed_counter < 1 ){
			    Util::rest( 1 );
			    keyInputManager::update();
		    }
	    }
	    
	    // do we got an option to run, lets do it
	    if ((*selectedOption)->getState() == MenuOption::Run){
		   try{
			/*if (backSound != ""){
			    Sound * ok = Resource::getSound(okSound);
			    ok->play();
			}*/
			(*selectedOption)->run(endGame);
		    } catch ( const ReturnException & re ){
		    }
		    // Reset it's state
		    (*selectedOption)->setState(MenuOption::Selected);
		    /*if ( !music.empty() ){
			    MenuGlobals::setMusic(music);
		    }
		    if ( !selectSound.empty() ){
			    MenuGlobals::setSelectSound(selectSound);
		    }*/
		    
		    // reset the fade state
		    fader.setState(FADEIN);
	    }
/*
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
*/
	    if (endGame){
		    // Deselect selected entry
		    (*selectedOption)->setState(MenuOption::Deselected);
		    /*if (backSound != ""){
			Sound * back = Resource::getSound(backSound);
			back->play();
		    }*/
	    }
    }
    
    return;
}


void MugenMenu::cleanup(){
    
    //Backgrounds
    if (background) delete background;
    
    // Get rid of items
    for(std::vector <MenuOption *>::iterator b = menuOptions.begin();b!=menuOptions.end();++b){
	    if ((*b)){
		delete (*b);
	    }
    }
    
    // Get rid of sprites
    cleanupSprites();
}

void MugenMenu::cleanupSprites(){
    // Get rid of sprites
    for( std::map< unsigned int, std::map< unsigned int, MugenSprite * > >::iterator i = sprites.begin() ; i != sprites.end() ; ++i ){
      for( std::map< unsigned int, MugenSprite * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
	  if( j->second )delete j->second;
      }
    }
}

// Draw text
void MugenMenu::drawText(Bitmap *bmp){
    
    int xplacement = position.x;
    int yplacement = position.y;
    int visibleCounter = 0;
    int offset = optionLocation >= windowVisibleItems ? optionLocation - windowVisibleItems + 1 : 0;
    for( std::vector <MenuOption *>::iterator i = menuOptions.begin() + offset; i != menuOptions.end(); ++i){
	MenuOption *option = *i;
	if (option->getState() == MenuOption::Selected){
	    MugenFont *font = fonts[fontActive.index-1];
	    font->changeBank(fontActive.bank);
	    const int height = font->getHeight();
	    const int length = font->textLength(option->getText().c_str());
	    
	    if(showBoxCursor){
		boxCursorCoords.alpha += boxCursorCoords.alphaMove;
		if (boxCursorCoords.alpha <= 0){
		    boxCursorCoords.alpha = 0;
		    boxCursorCoords.alphaMove = 6;
		}
		else if (boxCursorCoords.alpha >= 128){
		    boxCursorCoords.alpha = 128;
		    boxCursorCoords.alphaMove = -6;
		}
		Bitmap::drawingMode(Bitmap::MODE_TRANS);
		Bitmap::transBlender(0,0,0,boxCursorCoords.alpha);
		bmp->rectangleFill(xplacement + boxCursorCoords.x1, yplacement + boxCursorCoords.y1, xplacement + boxCursorCoords.x2,yplacement + boxCursorCoords.y2,Bitmap::makeColor(255,255,255));
		Bitmap::drawingMode(Bitmap::MODE_SOLID);
	    }
	    
	    switch (fontActive.position){
		case -1:
		    font->printf(xplacement - length, yplacement - height, 0, *bmp, option->getText(),0);
		    break;
		case 1:
		    font->printf(xplacement, yplacement - height, 0, *bmp, option->getText(),0);
		    break;
		case 0:
		default:
		    font->printf(xplacement - (length/2), yplacement - height, 0, *bmp, option->getText(),0);
		    break;
	    }
	    xplacement += fontSpacing.x;
	    yplacement += fontSpacing.y;
	} else {
	    MugenFont *font = fonts[fontItem.index-1];
	    font->changeBank(fontItem.bank);
	    const int height = font->getHeight();
	    const int length = font->textLength(option->getText().c_str());
	    switch (fontActive.position){
		case -1:
		    font->printf(xplacement - length, yplacement - height, 0, *bmp, option->getText(),0);
		    break;
		case 1:
		    font->printf(xplacement, yplacement - height, 0, *bmp, option->getText(),0);
		    break;
		case 0:
		default:
		    font->printf(xplacement - (length/2), yplacement - height, 0, *bmp, option->getText(),0);
		    break;
	    }
	    xplacement += fontSpacing.x;
	    yplacement += fontSpacing.y;
	}
	
	// Visible counter
	visibleCounter++;
	if (visibleCounter >= windowVisibleItems)break;
    }
}



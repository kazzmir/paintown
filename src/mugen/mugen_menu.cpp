#include "util/bitmap.h"
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
#include "util/file-system.h"
#include "util/timedifference.h"
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

#include "input/input-manager.h"

#include "mugen_animation.h"
#include "mugen/background.h"
#include "character_select.h"
#include "character.h"
#include "mugen_item.h"
#include "mugen_item_content.h"
#include "mugen_section.h"
#include "mugen_sound.h"
#include "mugen_reader.h"
#include "mugen_sprite.h"
#include "mugen_util.h"
#include "mugen_font.h"
#include "mugen/storyboard.h"

#include "mugen/option_versus.h"
#include "ast/all.h"
#include "parser/all.h"

namespace PaintownUtil = ::Util;

using namespace std;

static const int DEFAULT_WIDTH = 320;
static const int DEFAULT_HEIGHT = 240;
static const int DEFAULT_SCREEN_X_AXIS = 160;
static const int DEFAULT_SCREEN_Y_AXIS = 0;

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

void MugenMenu::loadData() throw (MugenException){
     // Lets look for our def since some people think that all file systems are case insensitive
    std::string baseDir = Filesystem::find("mugen/data/" + Mugen::Util::getFileDir(location));
    const std::string ourDefFile = Mugen::Util::fixFileName( baseDir, Mugen::Util::stripDir(location) );
    // get real basedir
    //baseDir = Mugen::Util::getFileDir( ourDefFile );
    Global::debug(1) << baseDir << endl;
    
    if (ourDefFile.empty()){
        throw MugenException( "Cannot locate menu definition file for: " + location );
    }

    TimeDifference diff;
    diff.startTime();
    Ast::AstParse parsed((list<Ast::Section*>*) Mugen::Def::main(ourDefFile));
    diff.endTime();
    Global::debug(1) << "Parsed mugen file " + ourDefFile + " in" + diff.printTime("") << endl;
    
    /*
    MugenReader reader( ourDefFile );
    std::vector< MugenSection * > collection;
    collection = reader.getCollection();
    */

    for (Ast::AstParse::section_iterator section_it = parsed.getSections()->begin(); section_it != parsed.getSections()->end(); section_it++){
        Ast::Section * section = *section_it;
	std::string head = section->getName();
        /* this should really be head = Mugen::Util::fixCase(head) */
	head = Mugen::Util::fixCase(head);
        if (head == "info"){
            class InfoWalker: public Ast::Walker{
            public:
                InfoWalker(MugenMenu & menu):
                menu(menu){
                }

                MugenMenu & menu;

                virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                    if (simple == "name"){
                        menu.setName(simple.valueAsString());
                        Global::debug(1) << "Read name '" << menu.getName() << "'" << endl;
                    } else if (simple == "author"){
                        string temp;
                        simple >> temp;
                        Global::debug(1) << "Made by: '" << temp << "'" << endl;
                    } else {
                        throw MugenException("Unhandled option in Info Section: " + simple.toString(), __FILE__, __LINE__);
                    }
                }
            };

            InfoWalker walker(*this);
            section->walk(walker);
        } else if (head == "files"){
            class FileWalker: public Ast::Walker{
                public:
                    FileWalker(MugenMenu & menu, const string & baseDir):
                        menu(menu),
                        baseDir(baseDir){
                        }

                    MugenMenu & menu;
                    const string & baseDir;

                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "spr"){
                            simple >> menu.spriteFile;
                            Global::debug(1) << "Got Sprite File: '" << menu.spriteFile << "'" << endl;
                            Mugen::Util::readSprites(Mugen::Util::getCorrectFileLocation(baseDir, menu.spriteFile), "", menu.sprites);
                        } else if (simple == "snd"){
                            simple >> menu.soundFile;
                            Global::debug(1) << "Got Sound File: '" << menu.soundFile << "'" << endl;
                        } else if (simple == "logo.storyboard"){
                            try{
                                simple >> menu.logoFile;
				if (!menu.logoFile.empty()){
				    try{
					Global::debug(1) << "Logo file " << baseDir << "/" << menu.logoFile << endl;
					menu.logo = new Mugen::Storyboard(Mugen::Util::getCorrectFileLocation(baseDir, menu.logoFile));
					Global::debug(1) << "Got Logo Storyboard File: '" << menu.logoFile << "'" << endl;
				    } catch (const MugenException &ex){
					throw MugenException( "Error loading logo storyboard: " + ex.getReason(), __FILE__, __LINE__);
				    }
				}
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "intro.storyboard"){
                            try{
                                simple >> menu.introFile;
				if (!menu.introFile.empty()){
				    try{
					Global::debug(1) << "Intro file " << baseDir << "/" << menu.introFile << endl;
					menu.intro = new Mugen::Storyboard(Mugen::Util::getCorrectFileLocation(baseDir, menu.introFile));
					Global::debug(1) << "Got Intro Storyboard File: '" << menu.introFile << "'" << endl;
				    } catch (const MugenException &ex){
					throw MugenException( "Error loading intro storyboard: " + ex.getReason(), __FILE__, __LINE__);
				    }
				}
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "select"){
                            simple >> menu.selectFile;
                            Global::debug(1) << "Got Select File: '" << menu.selectFile << "'" << endl;
                        } else if (simple == "fight"){
                            simple >> menu.fightFile;
                            Global::debug(1) << "Got Fight File: '" << menu.fightFile << "'" << endl;
                        } else if (PaintownUtil::matchRegex(simple.idString(), "^font")){
                            string temp;
                            simple >> temp;
                            menu.fonts.push_back(new MugenFont(Mugen::Util::getCorrectFileLocation(baseDir, temp)));
                            Global::debug(1) << "Got Font File: '" << temp << "'" << endl;

                        } else {
                            throw MugenException("Unhandled option in Files Section: " + simple.toString(), __FILE__, __LINE__ );
                        }
                    }
            };
            
            FileWalker walker(*this, baseDir);
            section->walk(walker);
        } else if (head == "music"){
            /* FIXME! parse music here */
        } else if (head == "title info"){
            class TitleInfoWalker: public Ast::Walker{
            public:
                TitleInfoWalker(MugenMenu & menu):
                menu(menu){
                }

                MugenMenu & menu;

                virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                    if (simple == "fadein.time"){
                        int time;
                        simple >> time;
                        menu.fader.setFadeInTime(time);
                    } else if (simple == "fadein.color"){
                        int r,g,b;
                        simple >> r >> g >> b;
                        menu.fader.setFadeInColor(Bitmap::makeColor(r,g,b));
                    } else if (simple == "fadeout.time"){
                        int time;
                        simple >> time;
                        menu.fader.setFadeOutTime(time);
                    } else if (simple == "fadeout.color"){
                        int r,g,b;
                        simple >> r >> g >> b;
                        menu.fader.setFadeOutColor(Bitmap::makeColor(r,g,b));
                    } else if (simple == "menu.pos"){
                        simple >> menu.position.x;
                        simple >> menu.position.y;
                    } else if (simple == "menu.item.font"){
                        simple >> menu.fontItem.index;
                        simple >> menu.fontItem.bank;
                        simple >> menu.fontItem.position;
                    } else if (simple == "menu.item.active.font"){
                        simple >> menu.fontActive.index;
                        simple >> menu.fontActive.bank;
                        simple >> menu.fontActive.position;
                    } else if (simple == "menu.item.spacing"){
                        simple >> menu.fontSpacing.x;
                        simple >> menu.fontSpacing.y;
		    } else if (simple == "menu.itemname.arcade"){
                        try{
                            menu.addOption(new OptionDummy(simple.valueAsString()));
                        } catch (const Ast::Exception & e){
                        }
                    } else if (simple == "menu.itemname.versus"){
                        try{
                            menu.addOption(new MugenOptionVersus(simple.valueAsString()));
                        } catch (const Ast::Exception & e){
                        }
		   } else if (simple == "menu.itemname.teamarcade"){
                       try{
                           menu.addOption(new OptionDummy(simple.valueAsString()));
                       } catch (const Ast::Exception & e){
                       }
		   } else if (simple == "menu.itemname.teamversus"){
                       try{
                           menu.addOption(new OptionDummy(simple.valueAsString()));
                       } catch (const Ast::Exception & e){
                       }
		   } else if (simple == "menu.itemname.teamcoop"){
                       try{
                           menu.addOption(new OptionDummy(simple.valueAsString()));
                       } catch (const Ast::Exception & e){
                       }
		   } else if (simple == "menu.itemname.survival"){
                       try{
                           menu.addOption(new OptionDummy(simple.valueAsString()));
                       } catch (const Ast::Exception & e){
                       }
		   } else if (simple == "menu.itemname.survivalcoop"){
                       try{
                           menu.addOption(new OptionDummy(simple.valueAsString()));
                       } catch (const Ast::Exception & e){
                       }
		   } else if (simple == "menu.itemname.training"){
                       try{
                           menu.addOption(new OptionDummy(simple.valueAsString()));
                       } catch (const Ast::Exception & e){
                       }
		   } else if (simple == "menu.itemname.watch"){
                       try{
                           menu.addOption(new OptionDummy(simple.valueAsString()));
                       } catch (const Ast::Exception & e){
                       }
		   } else if (simple == "menu.itemname.options"){
                       try{
                           menu.addOption(new OptionDummy(simple.valueAsString()));
                       } catch (const Ast::Exception & e){
                       }
		   } else if (simple == "menu.itemname.exit"){
                       try{
                           menu.addOption(new OptionQuit(simple.valueAsString()));
                       } catch (const Ast::Exception & e){
                       }
                   } else if (simple == "menu.window.margins.x"){
                       simple >> menu.windowMarginX.x;
                       simple >> menu.windowMarginX.y;
                   } else if (simple == "menu.window.margins.y"){
                       simple >> menu.windowMarginY.x;
                       simple >> menu.windowMarginY.y;
                   } else if (simple == "menu.window.visibleitems"){
                       simple >> menu.windowVisibleItems;
                   } else if (simple == "menu.boxcursor.visible"){
                       simple >> menu.showBoxCursor;
                   } else if (simple == "menu.boxcursor.coords"){
                       simple >> menu.boxCursorCoords.x1;
                       simple >> menu.boxCursorCoords.y1;
                       simple >> menu.boxCursorCoords.x2;
                       simple >> menu.boxCursorCoords.y2;
                       menu.boxCursorCoords.alpha = 128;
                       menu.boxCursorCoords.alphaMove = -6;
                   } else if (simple == "cursor.move.snd"){
                       /* FIXME! parse cursor.move.snd */
                   } else if (simple == "cursor.done.snd"){
                       /* FIXME! parse cursor.done.snd */
                   } else if (simple == "cancel.snd"){
                       /* FIXME! parse cancel.snd */
                   } else {
                       throw MugenException("Unhandled option in Info Section: " + simple.toString(), __FILE__, __LINE__);
                   }
                }
	    };
            
            TitleInfoWalker walker(*this);
            section->walk(walker);
        } else if (PaintownUtil::matchRegex(head, "^titlebgdef")){
            Mugen::Background *manager = new Mugen::Background(ourDefFile, "titlebg");
	    background = manager;
	} else if (head == "select info"){ 
	    selectInfoFile = ourDefFile;
        } else if (head == "selectbgdef" ){ /* Ignore for now */ }
	else if (head.find("selectbg") != std::string::npos ){ /* Ignore for now */ }
	else if (head == "vs screen" ){ /* Ignore for now */ }
	else if (head == "versusbgdef" ){ /* Ignore for now */ }
	else if (head.find("versusbg" ) != std::string::npos ){ /* Ignore for now */ }
	else if (head == "demo mode" ){ /* Ignore for now */ }
	else if (head == "continue screen" ){ /* Ignore for now */ }
	else if (head == "game over screen" ){ /* Ignore for now */ }
	else if (head == "win screen" ){ /* Ignore for now */ }
	else if (head == "default ending" ){ /* Ignore for now */ }
	else if (head == "end credits" ){ /* Ignore for now */ }
	else if (head == "survival results screen" ){ /* Ignore for now */ }
	else if (head == "option info" ){ /* Ignore for now */ }
	else if (head == "optionbgdef" ){ /* Ignore for now */ }
	else if (head.find("optionbg") != std::string::npos ){ /* Ignore for now */ }
	else if (head == "music" ){ /* Ignore for now */ }
	else if (head.find("begin action") != std::string::npos ){ /* Ignore for now */ }
        else {
            //throw MugenException("Unhandled Section in '" + ourDefFile + "': " + head, __FILE__, __LINE__ ); 
        }
    }  
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
    
    // Set the fade state
    fader.setState(FADEIN);
    
    // Keys
    InputMap<Mugen::Keys> input = Mugen::getPlayer1MenuKeys();
  
    // Do we have logos or intros?
    // Logo run it no repeat
    if (logo){
        logo->setInput(input);
	logo->run( *work,false);
    }
    // Intro run it no repeat
    if (intro){
        intro->setInput(input);
	intro->run( *work,false);
    }
  
    double runCounter = 0;
    while( ! endGame ){
	Global::speed_counter = 0;
	Global::second_counter = 0;
	int game_time = 100;
    
	while ( ! done && (*selectedOption)->getState() != MenuOption::Run && fader.getState() != RUNFADE ){

	    bool draw = false;
	    
	    //input
	    InputManager::poll();

	    if ( Global::speed_counter > 0 ){
		draw = true;
		runCounter += Global::speed_counter * Global::LOGIC_MULTIPLIER;
		while ( runCounter >= 1.0 ){
		    ticker++;
		    runCounter -= 1;
		    // Keys
		    InputMap<Mugen::Keys>::Output out = InputManager::getMap(input);
		    
		    if (fader.getState() == NOFADE){
			if ( out[Mugen::Up]){	
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

			if (out[Mugen::Down]){
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
			
			if (out[Mugen::Left]){
			    if ( (*selectedOption)->leftKey()){
				/* ??? */
			    }
			}
			
			if (out[Mugen::Right]){
			    if ( (*selectedOption)->rightKey()){
				/* ??? */
			    }
			}
			
			if ( out[Mugen::Enter] ){
			    if((*selectedOption)->isRunnable())(*selectedOption)->setState( MenuOption::Run );
			    // Set the fade state
			    fader.setState(FADEOUT);
			}
			
                        if ( out[Mugen::Esc] ){
			    endGame = done = true;
			    // Set the fade state
			    fader.setState(FADEOUT);
			    (*selectedOption)->setState(MenuOption::Deselected);
                            InputManager::waitForRelease(input, Mugen::Esc);
			}
		    }
		    // Fader
		    fader.act();
		    
		    // Options
		    for( vector< MenuOption *>::iterator b = menuOptions.begin(); b != menuOptions.end(); ++b ){
			    (*b)->logic();
		    }
		    
		    // Backgrounds
		    background->act();
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
		background->renderBackground(0,0,workArea);
		// Draw any misc stuff in the background of the menu of selected object 
		(*selectedOption)->drawBelow(work);
		// Draw text
		renderText(&workArea);
		// Foregrounds
		background->renderForeground(0,0,workArea);
		// Draw any misc stuff in the foreground of the menu of selected object 
		(*selectedOption)->drawAbove(work);
		// Do fades
		fader.draw(workArea);
		// Finally render to screen
		workArea.Stretch(*work);
		work->BlitToScreen();
	    }

	    while ( Global::speed_counter < 1 ){
		Util::rest( 1 );
	    }              
	}
	    
	// do we got an option to run, lets do it
	if ((*selectedOption)->getState() == MenuOption::Run){
	    try{
		(*selectedOption)->run(endGame);
	    } catch ( const ReturnException & re ){
	    }
	    // Reset it's state
	    (*selectedOption)->setState(MenuOption::Selected);
	    fader.setState(FADEIN);
	}
	if (endGame){
	    // Deselect selected entry
	    (*selectedOption)->setState(MenuOption::Deselected);
	}
    }
    throw ReturnException();
}

void MugenMenu::copyBackground(Bitmap & copyTo){
    copyTo.resize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
    background->renderBackground(0, 0, copyTo);
}

void MugenMenu::cleanup(){
    
    //Backgrounds
    if (background) delete background;
    
    // Get rid of sprites
    cleanupSprites();
}

void MugenMenu::cleanupSprites(){
    // Get rid of sprites
    for( Mugen::SpriteMap::iterator i = sprites.begin() ; i != sprites.end() ; ++i ){
      for( std::map< unsigned int, MugenSprite * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
	  if( j->second )delete j->second;
      }
    }
}

// Draw text
void MugenMenu::renderText(Bitmap *bmp){
    
    int xplacement = position.x;
    int yplacement = position.y;
    int visibleCounter = 0;
    int offset = optionLocation >= windowVisibleItems ? optionLocation - windowVisibleItems + 1 : 0;
    for( std::vector <MenuOption *>::iterator i = menuOptions.begin() + offset; i != menuOptions.end(); ++i){
	MenuOption *option = *i;
	if (option->getState() == MenuOption::Selected){
	    MugenFont *font = fonts[fontActive.index-1];
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
	    font->render(xplacement, yplacement, fontActive.position, fontActive.bank, *bmp, option->getText());
	    xplacement += fontSpacing.x;
	    yplacement += fontSpacing.y;
	} else {
	    MugenFont *font = fonts[fontItem.index-1];
	    font->render(xplacement, yplacement, fontItem.position, fontItem.bank, *bmp, option->getText());
	    xplacement += fontSpacing.x;
	    yplacement += fontSpacing.y;
	}
	
	// Visible counter
	visibleCounter++;
	if (visibleCounter >= windowVisibleItems)break;
    }
}

#include "util/bitmap.h"
#include "mugen/menu.h"

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

#include "mugen/stage.h"

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

#include "mugen/animation.h"
#include "mugen/background.h"
#include "mugen/config.h"
#include "character-select.h"
#include "character.h"
#include "mugen/item.h"
#include "mugen/item-content.h"
#include "mugen/section.h"
#include "mugen/sound.h"
#include "mugen/reader.h"
#include "mugen/sprite.h"
#include "mugen/util.h"
#include "mugen/font.h"
#include "mugen/storyboard.h"

#include "mugen/option-arcade.h"
#include "mugen/option-options.h"
#include "mugen/option-versus.h"
#include "ast/all.h"
#include "parser/all.h"

namespace PaintownUtil = ::Util;

using namespace std;

static const int DEFAULT_WIDTH = 320;
static const int DEFAULT_HEIGHT = 240;
static const int DEFAULT_SCREEN_X_AXIS = 160;
static const int DEFAULT_SCREEN_Y_AXIS = 0;

CursorHandler::CursorHandler(){
    // It seems that mugen defaults to something
    cursor.x1 = -32;
    cursor.y1 = -10;
    cursor.x2 = 32;    
    cursor.y2 = 2;
    cursor.visible = true;
    cursor.alpha = 128;
    cursor.alphaMove = -6;
}

CursorHandler::~CursorHandler(){
}

void CursorHandler::act(){
    if(cursor.visible){
	cursor.alpha += cursor.alphaMove;
	if (cursor.alpha <= 0){
	    cursor.alpha = 0;
	    cursor.alphaMove = 6;
	}
	else if (cursor.alpha >= 128){
	    cursor.alpha = 128;
	    cursor.alphaMove = -6;
	}
    }
}

void CursorHandler::renderCursor(int x, int y, const Bitmap & bmp){
    if (cursor.visible){
	Bitmap::drawingMode(Bitmap::MODE_TRANS);
	Bitmap::transBlender(0,0,0,cursor.alpha);
	bmp.rectangleFill(x + cursor.x1, y + cursor.y1, x + cursor.x2, y + cursor.y2, Bitmap::makeColor(255,255,255));
	Bitmap::drawingMode(Bitmap::MODE_SOLID);
    }
}

void CursorHandler::renderText(int x, int y, bool active, const std::string & text, std::vector<MugenFont *> & fonts, const Bitmap & bmp){
    if(active){
	renderCursor(x, y, bmp);
	fonts[activeFont.index-1]->render(x, y, activeFont.position, activeFont.bank, bmp, text);
    } else {
	fonts[itemFont.index-1]->render(x, y, itemFont.position, itemFont.bank, bmp, text);
    }
}

int CursorHandler::getFontHeight(std::vector<MugenFont *> & fonts){
    return fonts[itemFont.index-1]->getHeight();
}

int CursorHandler::getCursorHeight() const {
    const int y1 = abs(cursor.y1);
    const int y2 = abs(cursor.y2);
    
    if (y2 > y1){
	return y2 - y1;
    } else if (y1 > y2){
	return y1 - y2;
    }
    
    return 0;
}
	

Mugen::ItemOption::ItemOption():
MenuOption(0){
}

Mugen::ItemOption::~ItemOption(){
}

void Mugen::ItemOption::logic(){
}
void Mugen::ItemOption::run(bool &endGame){
}

void Mugen::ItemOption::render(int x, int y, CursorHandler & handler,  std::vector<MugenFont *> & fonts, const Bitmap & bmp){
    handler.renderText(x, y, (getState() == MenuOption::Selected), getText(), fonts, bmp);
}

namespace Mugen {
class QuitOption : public ItemOption {
    public:
	QuitOption(const std::string & name){
	    this->setText(name);
	}
	virtual ~QuitOption(){
	}
	void executeOption(const Mugen::PlayerType & player, bool & endGame){
	    endGame = true;
	}
};

class DummyOption : public ItemOption {
    public:
	DummyOption(const std::string & name){
	    this->setText(name);
	}
	virtual ~DummyOption(){
	}
	void executeOption(const Mugen::PlayerType & player, bool & endGame){
	
	}
};
}

MugenMenu::MugenMenu(const std::string &filename):
optionLocation(0),
location(filename),
spriteFile(""),
soundFile(""),
logoFile(""),
introFile(""),
selectFile(""),
fightFile(""),
windowMargin(12,8),
windowVisibleItems(5),
ticker(0),
background(0),
logo(0),
intro(0){
}

void MugenMenu::loadData(){
     // Lets look for our def since some people think that all file systems are case insensitive
    std::string baseDir = Filesystem::find( Mugen::Data::getInstance().getDirectory() + Mugen::Util::getFileDir(location));
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
                            Mugen::Util::readSounds( Mugen::Util::getCorrectFileLocation(baseDir, menu.soundFile ), menu.sounds);
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
                        int index=0,bank=0,position=0;
			try{
			    simple >> index >> bank >> position;
			} catch (const Ast::Exception & e){
			}
                        menu.fontCursor.setItemFont(index,bank,position);
                    } else if (simple == "menu.item.active.font"){
			int index=0,bank=0,position=0;
			try{
			    simple >> index >> bank >> position;
			} catch (const Ast::Exception & e){
			}
                        menu.fontCursor.setActiveFont(index,bank,position);
                    } else if (simple == "menu.item.spacing"){
                        simple >> menu.fontSpacing.x;
                        simple >> menu.fontSpacing.y;
		    } else if (simple == "menu.itemname.arcade"){
                        try{
                            menu.addMenuOption(new Mugen::OptionArcade(simple.valueAsString()));
                        } catch (const Ast::Exception & e){
                        }
                    } else if (simple == "menu.itemname.versus"){
                        try{
                            menu.addMenuOption(new Mugen::OptionVersus(simple.valueAsString()));
                        } catch (const Ast::Exception & e){
                        }
		   } else if (simple == "menu.itemname.teamarcade"){
                       try{
                           menu.addMenuOption(new Mugen::DummyOption(simple.valueAsString()));
                       } catch (const Ast::Exception & e){
                       }
		   } else if (simple == "menu.itemname.teamversus"){
                       try{
                           menu.addMenuOption(new Mugen::DummyOption(simple.valueAsString()));
                       } catch (const Ast::Exception & e){
                       }
		   } else if (simple == "menu.itemname.teamcoop"){
                       try{
                           menu.addMenuOption(new Mugen::DummyOption(simple.valueAsString()));
                       } catch (const Ast::Exception & e){
                       }
		   } else if (simple == "menu.itemname.survival"){
                       try{
                           menu.addMenuOption(new Mugen::DummyOption(simple.valueAsString()));
                       } catch (const Ast::Exception & e){
                       }
		   } else if (simple == "menu.itemname.survivalcoop"){
                       try{
                           menu.addMenuOption(new Mugen::DummyOption(simple.valueAsString()));
                       } catch (const Ast::Exception & e){
                       }
		   } else if (simple == "menu.itemname.training"){
                       try{
                           menu.addMenuOption(new Mugen::DummyOption(simple.valueAsString()));
                       } catch (const Ast::Exception & e){
                       }
		   } else if (simple == "menu.itemname.watch"){
                       try{
                           menu.addMenuOption(new Mugen::DummyOption(simple.valueAsString()));
                       } catch (const Ast::Exception & e){
                       }
		   } else if (simple == "menu.itemname.options"){
                       try{
                           menu.addMenuOption(new Mugen::OptionOptions(simple.valueAsString()));
                       } catch (const Ast::Exception & e){
                       }
		   } else if (simple == "menu.itemname.exit"){
                       try{
                           menu.addMenuOption(new Mugen::QuitOption(simple.valueAsString()));
                       } catch (const Ast::Exception & e){
                       }
                   } else if (simple == "menu.window.margins.y"){
                       simple >> menu.windowMargin.x;
                       simple >> menu.windowMargin.y;
		       // Undocumented but it defaults to 5 if it's 0 or not specified
		       if (menu.windowMargin.y == 0){
			   menu.windowMargin.y = 5;
		       }
                   } else if (simple == "menu.window.visibleitems"){
                       simple >> menu.windowVisibleItems;
                   } else if (simple == "menu.boxcursor.visible"){
                       bool visible = false;
		       simple >> visible;
		       menu.fontCursor.setCursorVisible(visible);
                   } else if (simple == "menu.boxcursor.coords"){
		       int x1=0,y1=0,x2=0,y2=0;
			try{
			    simple >> x1 >> y1 >> x2 >> y2;
			} catch (const Ast::Exception & e){
			}
                        menu.fontCursor.setCursor(x1,y1,x2,y2);
                   } else if (simple == "cursor.move.snd"){
                       try{
                            simple >> menu.moveSound.x >> menu.moveSound.y;
                       } catch (const Ast::Exception & e){
                       }
                   } else if (simple == "cursor.done.snd"){
                       try{
                            simple >> menu.doneSound.x >> menu.doneSound.y;
                       } catch (const Ast::Exception & e){
                       }
                   } else if (simple == "cancel.snd"){
                       int g=0,s=0;
                       try{
                            simple >> menu.cancelSound.x >> menu.cancelSound.y;
                       } catch (const Ast::Exception & e){
                       }
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
    
    if ( options.empty() ){
	    return;
    }
    
    currentOption = options.begin();
    optionLocation = 0;
    options.front()->setState(MenuOption::Selected);
    
    // Set the fade state
    fader.setState(Mugen::FadeTool::FadeIn);
    
    // Keys
    InputMap<Mugen::Keys> player1Input = Mugen::getPlayer1Keys(20);
    InputMap<Mugen::Keys> player2Input = Mugen::getPlayer2Keys(20);
    
    // Selecting player
    Mugen::PlayerType selectingPlayer;
  
    // Do we have logos or intros?
    // Logo run it no repeat
    if (logo){
        logo->setInput(player1Input);
	logo->run( *work,false);
    }
    // Intro run it no repeat
    if (intro){
        intro->setInput(player1Input);
	intro->run( *work,false);
    }
  
    double runCounter = 0;
    while( ! endGame ){
	Global::speed_counter = 0;
	Global::second_counter = 0;
	int game_time = 100;
    
	while ( ! done && (*currentOption)->getState() != MenuOption::Run && fader.getState() != Mugen::FadeTool::RunFade ){

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
		    InputMap<Mugen::Keys>::Output out1 = InputManager::getMap(player1Input);
		    InputMap<Mugen::Keys>::Output out2 = InputManager::getMap(player2Input);
		    
		    if (fader.getState() == Mugen::FadeTool::NoFade){
			if ( out1[Mugen::Up] || out2[Mugen::Up]){	
			    (*currentOption)->setState(MenuOption::Deselected);
			    if ( currentOption > options.begin() ){
				    currentOption--;
				    optionLocation--;
			    } else { 
				currentOption = options.end() -1;
				optionLocation = options.size() -1;
			    }
			    (*currentOption)->setState(MenuOption::Selected);
			    if (sounds[moveSound.x][moveSound.y] != 0){
                                sounds[moveSound.x][moveSound.y]->play();
                            }
			}

			if (out1[Mugen::Down] || out2[Mugen::Down]){
			    (*currentOption)->setState(MenuOption::Deselected);
			    if ( currentOption < options.begin()+options.size()-1 ){
				    currentOption++;
				    optionLocation++;
			    } else {
				currentOption = options.begin();
				optionLocation = 0;
			    }
			    (*currentOption)->setState(MenuOption::Selected);
			    if (sounds[moveSound.x][moveSound.y] != 0){
                                sounds[moveSound.x][moveSound.y]->play();
                            }
			}
			
			if ( out1[Mugen::Enter] ){
			    if((*currentOption)->isRunnable())(*currentOption)->setState( MenuOption::Run );
			    // Set the fade state
			    fader.setState(Mugen::FadeTool::FadeOut);
                            if (sounds[doneSound.x][doneSound.y] != 0){
                                sounds[doneSound.x][doneSound.y]->play();
                            }
                            selectingPlayer = Mugen::Player1;
			}
			
			if ( out2[Mugen::Enter] ){
			    if((*currentOption)->isRunnable())(*currentOption)->setState( MenuOption::Run );
			    // Set the fade state
			    fader.setState(Mugen::FadeTool::FadeOut);
                            if (sounds[doneSound.x][doneSound.y] != 0){
                                sounds[doneSound.x][doneSound.y]->play();
                            }
                            selectingPlayer = Mugen::Player2;
			}
			
                        if ( out1[Mugen::Esc] || out2[Mugen::Esc] ){
			    endGame = done = true;
			    // Set the fade state
			    fader.setState(Mugen::FadeTool::FadeOut);
			    (*currentOption)->setState(MenuOption::Deselected);
                            InputManager::waitForRelease(player1Input, Mugen::Esc);
                            if (sounds[cancelSound.x][cancelSound.y] != 0){
                                sounds[cancelSound.x][cancelSound.y]->play();
                            }
			}
		    }
		    // Font Cursor
		    fontCursor.act();
		    
		    // Fader
		    fader.act();
		    
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
		(*currentOption)->drawBelow(work);
		// Draw text
		renderText(&workArea);
		// Foregrounds
		background->renderForeground(0,0,workArea);
		// Draw any misc stuff in the foreground of the menu of selected object 
		(*currentOption)->drawAbove(work);
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
	if ((*currentOption)->getState() == MenuOption::Run){
	    try{
		(*currentOption)->executeOption(selectingPlayer, endGame);
	    } catch ( const ReturnException & re ){
	    }
	    // Reset it's state
	    (*currentOption)->setState(MenuOption::Selected);
	    fader.setState(Mugen::FadeTool::FadeIn);
	}
	if (endGame){
	    // Deselect selected entry
	    (*currentOption)->setState(MenuOption::Deselected);
	}
    }
    throw ReturnException();
}

void MugenMenu::addMenuOption(Mugen::ItemOption * option){
    if (option){
	option->setParent(this);
	options.push_back(option);
    }
}

void MugenMenu::cleanup(){
    
    //Backgrounds
    if (background) delete background;
    
    // Get rid of sprites
    cleanupSprites();

    // Get rid of sounds
    for( std::map< unsigned int, std::map< unsigned int, MugenSound * > >::iterator i = sounds.begin() ; i != sounds.end() ; ++i ){
      for( std::map< unsigned int, MugenSound * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
	  if( j->second )delete j->second;
      }
    }
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
    
    // Top of the window
    const int top = position.y - windowMargin.x;
    // Pretty accurate tested with 5 different screen packs and different settings
    const int bottom = position.y + ((windowVisibleItems-1) * fontSpacing.y) + windowMargin.y;
    
    /* FIXME: avoid using clip rect */
    bmp->setClipRect(0, top, bmp->getWidth(), bottom);
    //bmp->rectangle(0,top,bmp->getWidth(),bottom,Bitmap::makeColor(255,255,255));
    
    int offset = optionLocation >= windowVisibleItems ? optionLocation - windowVisibleItems + 1 : 0;
    
    int xplacement = position.x;
    // Displace by the offset
    int yplacement = position.y - (offset * fontSpacing.y);
    
    for( std::vector <Mugen::ItemOption *>::iterator i = options.begin(); i != options.end(); ++i){
	
	Mugen::ItemOption *option = *i;
	// Render
	option->render(xplacement, yplacement, fontCursor, fonts, *bmp);
	
	// Displacement
	xplacement += fontSpacing.x;
	yplacement += fontSpacing.y;
    }
    
    bmp->setClipRect(0,0,bmp->getWidth(),bmp->getHeight());
}

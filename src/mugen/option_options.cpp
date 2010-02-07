#include "util/bitmap.h"
#include "mugen/option_options.h"
#include "mugen/game.h"
#include "mugen/mugen_menu.h"
#include "mugen/config.h"
#include "mugen/mugen_font.h"
#include "mugen/mugen_sound.h"
#include "mugen/background.h"

#include <ostream>

#include "ast/all.h"
#include "parser/all.h"
#include "init.h"
#include "util/funcs.h"
#include "util/file-system.h"
#include "util/timedifference.h"
#include "input/input-manager.h"
#include "return_exception.h"

#include "gui/box.h"

#include "globals.h"

namespace PaintownUtil = ::Util;
using namespace std;
using namespace Mugen;

const int DEFAULT_WIDTH = 320;
const int DEFAULT_HEIGHT = 240;

OptionOptions::OptionOptions(Token *token) throw (LoadException): 
MenuOption(token, Event){
    // notin
}
OptionOptions::OptionOptions( const std::string &name ) throw( LoadException ):
MenuOption(0, Event){
    if (name.empty()){
	throw LoadException("No name given to Options");
    }
    this->setText(name);
}

OptionOptions::~OptionOptions(){
	// Nothing
}

void OptionOptions::logic(){
}

void OptionOptions::run(bool &endGame){
    std::string systemFile = Mugen::Data::getInstance().getFileFromMotif(Mugen::Data::getInstance().getMotif());
    // Lets look for our def since some people think that all file systems are case insensitive
    std::string baseDir = Mugen::Util::getFileDir(systemFile);
    
    Global::debug(1) << baseDir << endl;
    
    if (systemFile.empty()){
        throw MugenException( "Cannot locate character select definition file for: " + systemFile );
    }

    TimeDifference diff;
    diff.startTime();
    Ast::AstParse parsed((list<Ast::Section*>*) Mugen::Def::main(systemFile));
    diff.endTime();
    Global::debug(1) << "Parsed mugen file " + systemFile + " in" + diff.printTime("") << endl;
    
    Mugen::Background * background = 0;
    std::vector< MugenFont *> fonts;
    Mugen::SoundMap sounds;
    
    Mugen::Point moveSound;
    Mugen::Point doneSound;
    Mugen::Point cancelSound;
    
    for (Ast::AstParse::section_iterator section_it = parsed.getSections()->begin(); section_it != parsed.getSections()->end(); section_it++){
        Ast::Section * section = *section_it;
	std::string head = section->getName();
	if (head == "Files"){
            class FileWalker: public Ast::Walker{
                public:
                    FileWalker(std::vector<MugenFont *> & fonts, Mugen::SoundMap & sounds, const string & baseDir):
                        fonts(fonts),
			sounds(sounds),
                        baseDir(baseDir){
                        }

                    std::vector<MugenFont *> & fonts;
		    Mugen::SoundMap & sounds;
                    const string & baseDir;

                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "snd"){
			    std::string file;
                            simple >> file;
                            Mugen::Util::readSounds( Mugen::Util::getCorrectFileLocation(baseDir, file ), sounds);
                            Global::debug(1) << "Got Sound File: '" << file << "'" << endl;
                        } else if (PaintownUtil::matchRegex(simple.idString(), "^font")){
                            std::string temp;
                            simple >> temp;
                            fonts.push_back(new MugenFont(Mugen::Util::getCorrectFileLocation(baseDir, temp)));
                            Global::debug(1) << "Got Font File: '" << temp << "'" << endl;
                        }
                    }
            };
            FileWalker walker(fonts, sounds, baseDir);
            section->walk(walker);
        } else if (head == "Option Info"){
	    class InfoWalker: public Ast::Walker{
                public:
                    InfoWalker(Mugen::Point & moveSound, Mugen::Point & doneSound, Mugen::Point & cancelSound):
                        moveSound(moveSound),
                        doneSound(doneSound),
			cancelSound(cancelSound){
                        }

                    Mugen::Point & moveSound;
		    Mugen::Point & doneSound;
		    Mugen::Point & cancelSound;

                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "cursor.move.snd"){
			    try{
				simple >> moveSound.x >> moveSound.y;
			    } catch (const Ast::Exception & e){
			    }
                        } else if (simple == "cursor.done.snd"){
			    try{
				simple >> doneSound.x >> doneSound.y;
			    } catch (const Ast::Exception & e){
			    }
                        } else if (simple == "cancel.snd"){
			    try{
				simple >> cancelSound.x >> cancelSound.y;
			    } catch (const Ast::Exception & e){
			    }
                        } 
                    }
            };
            InfoWalker walker(moveSound, doneSound, cancelSound);
            section->walk(walker);
	} else if (head == "OptionBGdef"){ 
	    /* Background management */
	    background = new Mugen::Background(systemFile, "optionbg");
	}
    }
    
    // Run options
    Bitmap workArea(DEFAULT_WIDTH,DEFAULT_HEIGHT);
    Bitmap screen(GFX_X, GFX_Y);
    bool done = false;
    bool escaped = false;
    
    int ticker = 0;
  
    double runCounter = 0;
    Global::speed_counter = 0;
    Global::second_counter = 0;
    int game_time = 100;
    
    // Set game keys temporary
    InputMap<Mugen::Keys> gameInput;
    gameInput.set(Keyboard::Key_ESC, 10, true, Mugen::Esc);
    
    // Our Font
    MugenFont * font = fonts[1];
    
    // Box
    Box optionArea;
    optionArea.position.width = 200;
    optionArea.position.height = 180;
    optionArea.position.x = (DEFAULT_WIDTH/2) - (optionArea.position.width/2);
    optionArea.position.y = (DEFAULT_HEIGHT/2) - (optionArea.position.height/2);
    optionArea.position.radius = 5;
    optionArea.position.body = Bitmap::makeColor(0,0,60);
    optionArea.position.bodyAlpha = 150;
    optionArea.position.border = Bitmap::makeColor(0,0,20);
    
    
    while (!done){
    
	bool draw = false;
	
	if ( Global::speed_counter > 0 ){
	    draw = true;
	    runCounter += Global::speed_counter * Global::LOGIC_MULTIPLIER;
	    while ( runCounter >= 1.0 ){
		// tick tock
		ticker++;
		
		runCounter -= 1;
		// Key handler
		InputManager::poll();
		
		InputMap<Mugen::Keys>::Output out = InputManager::getMap(gameInput);
		if (out[Mugen::Esc]){
		    done = escaped = true;
		    if (sounds[cancelSound.x][cancelSound.y]){
			sounds[cancelSound.x][cancelSound.y]->play();
		    }
		    InputManager::waitForRelease(gameInput, Mugen::Esc);
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
	    // render backgrounds
	    background->renderBackground(0,0,workArea);
	    
	    // render fonts
	    font->render(DEFAULT_WIDTH/2, 20, 0, 0, workArea, "OPTIONS" );
	    
	    optionArea.render(&workArea);
	    
	    doOptions(*font,optionArea.position.x,optionArea.position.y,workArea);
	    
	    // render Foregrounds
	    background->renderForeground(0,0,workArea);
	    
	    // Finally render to screen
	    workArea.Stretch(screen);
	    screen.BlitToScreen();
	}

	while ( Global::speed_counter < 1 ){
		PaintownUtil::rest( 1 );
	}
    }
    
    delete background;
    
    // Get rid of sprites
    for( std::vector<MugenFont *>::iterator i = fonts.begin() ; i != fonts.end() ; ++i ){
	if (*i){
	    delete *i;
	}
    }
    // Get rid of sounds
    for( std::map< unsigned int, std::map< unsigned int, MugenSound * > >::iterator i = sounds.begin() ; i != sounds.end() ; ++i ){
	for( std::map< unsigned int, MugenSound * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
	    if (j->second){
		delete j->second;
	    }
	}
    }
    
    // **FIXME Hack figure something out
    if (escaped){
	throw ReturnException();
    }
}

static std::string getString(int number){
    std::ostringstream str;
    str << number;
    return str.str();
}

void OptionOptions::doOptions(MugenFont & font, int x, int y, const Bitmap & bmp){
    const int rightX = x + 195;
    Mugen::Data & data = Mugen::Data::getInstance();
    font.render(x+5, y+30, 1, 0, bmp, "Difficulty" );
    font.render(rightX, y+30, -1, 0, bmp, getString(data.getDifficulty()));
    font.render(x+5, y+50, 1, 0, bmp, "Life" );
    font.render(rightX, y+50, -1, 0, bmp, getString(data.getLife()));
    font.render(x+5, y+70, 1, 0, bmp, "Time Limit" );
    font.render(rightX, y+70, -1, 0, bmp, getString(data.getTime()));
    font.render(x+5, y+90, 1, 0, bmp, "Game Speed" );
    font.render(rightX, y+90, -1, 0, bmp, getString(data.getSpeed()));
    font.render(x+5, y+110, 1, 0, bmp, "1P VS Team Advantage" );
    font.render(rightX, y+110, -1, 0, bmp, getString(data.getTeam1vs2Life()));
    font.render(x+5, y+130, 1, 0, bmp, "If player KOed" );
    font.render(rightX, y+130, -1, 0, bmp, getString(data.getTeamLoseOnKO()));
    font.render(x+5, y+150, 1, 0, bmp, "Return to Main Menu" );
    font.render(rightX, y+150, -1, 0, bmp, "(Esc)");
}

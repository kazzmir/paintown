#include "util/bitmap.h"
#include "mugen/option-options.h"
#include "mugen/game.h"
#include "mugen/menu.h"
#include "mugen/config.h"
#include "mugen/font.h"
#include "mugen/sound.h"
#include "mugen/background.h"

#include <ostream>
#include <vector>

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

static std::string getString(int number){
    std::ostringstream str;
    str << number;
    return str.str();
}

Option::Option():
selected(false),
alpha(0),
alphaMod(6){
}

Option::~Option(){
}

void Option::render(MugenFont & font, int x, int y, const Bitmap & bmp){
    const int rightX = x + 195;
    font.render(x+5, y, 1, 0, bmp, optionName);
    font.render(rightX, y, -1, 0, bmp, currentValue);
    if (selected){
	alpha += alphaMod;
	if (alpha <= 0){
	    alpha = 0;
	    alphaMod = 6;
	}
	else if (alpha >= 128){
	    alpha = 128;
	    alphaMod = -6;
	}
	Bitmap::drawingMode(Bitmap::MODE_TRANS);
	Bitmap::transBlender(0,0,0,alpha);
	bmp.rectangleFill(x+2, y-10, rightX+3, y+2,Bitmap::makeColor(255,255,255));
	Bitmap::drawingMode(Bitmap::MODE_SOLID);
    }
}

class Difficulty : public Mugen::Option {
    public:
	Difficulty(){
	    optionName = "Difficulty";
	    currentValue = getString(Mugen::Data::getInstance().getDifficulty());
	}
	~Difficulty(){
	}
	void next(){
	}
	void prev(){
	}
};

class Life : public Mugen::Option {
    public:
	Life(){
	    optionName = "Life";
	    currentValue = getString(Mugen::Data::getInstance().getLife());
	}
	~Life(){
	}
	void next(){
	}
	void prev(){
	}
};

class TimeLimit : public Mugen::Option {
    public:
	TimeLimit(){
	    optionName = "Time Limit";
	    currentValue = getString(Mugen::Data::getInstance().getTime());
	}
	~TimeLimit(){
	}
	void next(){
	    int time = Mugen::Data::getInstance().getTime();
	    if (time != -1){
		time+=20;
	    }
	    if (time == 100){
		time = 99;
	    } else if (time > 100){
		time = -1;
	    }
	    Mugen::Data::getInstance().setTime(time);
	    if (time == -1){
		currentValue = "None";
	    } else {
		currentValue = getString(time);
	    }
	}
	void prev(){
	    int time = Mugen::Data::getInstance().getTime();
	    if (time == -1){
		time = 99;
	    } else if (time >= 20 && time != 99){
		time-=20;
	    } else if (time == 99){
		time = 80;
	    }
	    if (time < 20){
		time = 20;
	    }
	    Mugen::Data::getInstance().setTime(time);
	    currentValue = getString(time);
	}
};

class Speed : public Mugen::Option {
    public:
	Speed(){
	    optionName = "Speed";
	    currentValue = getString(Mugen::Data::getInstance().getSpeed());
	}
	~Speed(){
	}
	void next(){
	}
	void prev(){
	}
};

class OneVsTeam : public Mugen::Option {
    public:
	OneVsTeam(){
	    optionName = "1P VS Team Advantage";
	    currentValue = getString(Mugen::Data::getInstance().getTeam1vs2Life());
	}
	~OneVsTeam(){
	}
	void next(){
	}
	void prev(){
	}
};

class TeamLoseOnKO : public Mugen::Option {
    public:
	TeamLoseOnKO(){
	    optionName = "If player KOed";
	    currentValue = getString(Mugen::Data::getInstance().getTeamLoseOnKO());
	}
	~TeamLoseOnKO(){
	}
	void next(){
	}
	void prev(){
	}
};

class Escape : public Mugen::Option {
    public:
	Escape(){
	    optionName = "Return to Main Menu";
	    currentValue = "(Esc)";
	}
	~Escape(){
	}
	void next(){
	}
	void prev(){
	}
};

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
    
    // Add options
    options.push_back(new Difficulty());
    options.push_back(new Life());
    options.push_back(new TimeLimit());
    options.push_back(new Speed());
    options.push_back(new OneVsTeam());
    options.push_back(new TeamLoseOnKO());
    options.push_back(new Escape());
    
    // Set first one
    options[0]->toggleSelected();
    selectedOption = options.begin();
}

OptionOptions::~OptionOptions(){
    for (std::vector<Mugen::Option *>::iterator i = options.begin(); i != options.end(); ++i){
	if (*i){
	    delete *i;
	}
    }
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
    InputMap<Mugen::Keys> player1Input = Mugen::getPlayer1Keys(20);
    InputMap<Mugen::Keys> player2Input = Mugen::getPlayer2Keys(20);
    
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
		
		InputMap<Mugen::Keys>::Output out1 = InputManager::getMap(player1Input);
		InputMap<Mugen::Keys>::Output out2 = InputManager::getMap(player2Input);
		if (out1[Mugen::Esc] || out2[Mugen::Esc]){
		    done = escaped = true;
		    if (sounds[cancelSound.x][cancelSound.y]){
			sounds[cancelSound.x][cancelSound.y]->play();
		    }
		    InputManager::waitForRelease(player1Input, Mugen::Esc);
		    InputManager::waitForRelease(player2Input, Mugen::Esc);
		}
		if (out1[Mugen::Up] || out2[Mugen::Up]){
		    (*selectedOption)->toggleSelected();
		    if (selectedOption > options.begin()){
			selectedOption--;
		    } else {
			selectedOption = options.begin() + options.size()-1;
		    }
		    (*selectedOption)->toggleSelected();
		    if (sounds[moveSound.x][moveSound.y]){
			sounds[moveSound.x][moveSound.y]->play();
		    }
		}
		if (out1[Mugen::Down] || out2[Mugen::Down]){
		    (*selectedOption)->toggleSelected();
		    selectedOption++;
		    if (selectedOption == options.end()){
			selectedOption = options.begin();
		    }
		    (*selectedOption)->toggleSelected();
		    if (sounds[moveSound.x][moveSound.y]){
			sounds[moveSound.x][moveSound.y]->play();
		    }
		}
		if (out1[Mugen::Left] || out2[Mugen::Left]){
		    (*selectedOption)->prev();
		}
		if (out1[Mugen::Right] || out2[Mugen::Right]){
		    (*selectedOption)->next();
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

void OptionOptions::doOptions(MugenFont & font, int x, int y, const Bitmap & bmp){
    int mod = 30;
    for (std::vector<Mugen::Option *>::iterator i = options.begin(); i != options.end(); ++i){
	Mugen::Option * option = *i;
	option->render(font, x, y+mod, bmp);
	mod+=20;
    }
}

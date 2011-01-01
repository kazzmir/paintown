#include "util/bitmap.h"
#include "util/trans-bitmap.h"
#include "util/debug.h"
#include "configuration.h"
#include "options.h"
#include "game.h"
#include "menu.h"
#include "config.h"
#include "font.h"
#include "sound.h"
#include "background.h"
#include "parse-cache.h"

#include <ostream>
#include <vector>

#include "ast/all.h"
#include "parser/all.h"
#include "util/init.h"
#include "util/funcs.h"
#include "util/file-system.h"
#include "util/timedifference.h"
#include "util/input/input-manager.h"
#include "exceptions/exception.h"

#include "gui/box.h"

#include "globals.h"

namespace PaintownUtil = ::Util;
using namespace std;
using namespace Gui;

namespace Mugen{

static const int DEFAULT_WIDTH = 320;
static const int DEFAULT_HEIGHT = 240;

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

void Option::enter(){
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
	// Bitmap::drawingMode(Bitmap::MODE_TRANS);
	Bitmap::transBlender(0,0,0,alpha);
	bmp.translucent().rectangleFill(x+2, y-10, rightX+2, y+2,Bitmap::makeColor(255,255,255));
	// Bitmap::drawingMode(Bitmap::MODE_SOLID);
    }
}

class Difficulty: public Option {
    public:
	Difficulty(){
	    optionName = "Difficulty";
            int difficulty = Data::getInstance().getDifficulty();
            if (difficulty < 1){
                difficulty = 1;
                Data::getInstance().setDifficulty(difficulty);
            } else if (difficulty > 8){
                difficulty = 8;
                Data::getInstance().setDifficulty(difficulty);
            }
	    currentValue = getDifficultyName(difficulty);
	}
	~Difficulty(){
	}
	void next(){
            int difficulty = Data::getInstance().getDifficulty() + 1;
            if (difficulty > 8){
                difficulty = 8;
            }
            Data::getInstance().setDifficulty(difficulty);
            currentValue = getDifficultyName(difficulty);
        }
	void prev(){
            int difficulty = Data::getInstance().getDifficulty() - 1;
            if (difficulty < 1){
                difficulty = 1;
            }
            Data::getInstance().setDifficulty(difficulty);
            currentValue = getDifficultyName(difficulty);
	}
        
        std::string getDifficultyName(int difficulty){
            
            switch (difficulty){
                case 1:
                case 2:
                    return "Easy " + getString(difficulty);
                    break;
                case 3:
                case 4:
                case 5:
                    return "Medium " + getString(difficulty);
                    break;
                case 6:
                case 7:
                case 8:
                    return "Hard " + getString(difficulty);
                    break;
                default:
                    break;
            }
            return std::string();
        }
};

class Life : public Option {
    public:
	Life(){
	    optionName = "Life";
            int life = Data::getInstance().getLife();
            if (life < 30){
                life = 30;
                Data::getInstance().setLife(life);
            } else if (life > 300){
                life = 300;
                Data::getInstance().setLife(life);
            }
	    currentValue = getString(life)+"%%";
	}
	~Life(){
	}
	void next(){
            int life = Data::getInstance().getLife()+10;
            if (life > 300){
                life = 300;
            }
            Data::getInstance().setLife(life);
	    currentValue = getString(life)+"%%";
        }
	void prev(){
            int life = Data::getInstance().getLife()-10;
            if (life < 30){
                life = 30;
            }
            Data::getInstance().setLife(life);
	    currentValue = getString(life)+"%%";
	}
};

class TimeLimit : public Option {
    public:
	TimeLimit(){
	    optionName = "Time Limit";
	    currentValue = getString(Data::getInstance().getTime());
	}
	~TimeLimit(){
	}
	void next(){
	    int time = Data::getInstance().getTime();
	    if (time != -1){
		time+=20;
	    }
	    if (time == 100){
		time = 99;
	    } else if (time > 100){
		time = -1;
	    }
	    Data::getInstance().setTime(time);
	    if (time == -1){
		currentValue = "None";
	    } else {
		currentValue = getString(time);
	    }
	}
	void prev(){
	    int time = Data::getInstance().getTime();
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
	    Data::getInstance().setTime(time);
	    currentValue = getString(time);
	}
};

class Speed : public Option {
    public:
	Speed(){
	    optionName = "Speed";
            int speed = Data::getInstance().getSpeed();
            if (speed < -9){
                speed = -9;
                Data::getInstance().setSpeed(speed);
            } else if (speed > 9){
                speed = 9;
                Data::getInstance().setSpeed(speed);
            }
	    currentValue = getSpeedName(speed);
	}
	~Speed(){
	}
	void next(){
            int speed = Data::getInstance().getSpeed() + 1;
            if (speed > 9){
                speed = 9;
            }
            Data::getInstance().setSpeed(speed);
	    currentValue = getSpeedName(speed);
	}
	void prev(){
            int speed = Data::getInstance().getSpeed() - 1;
            if (speed < -9){
                speed = -9;
            }
            Data::getInstance().setSpeed(speed);
	    currentValue = getSpeedName(speed);
	}

        std::string getSpeedName(int speed){
            if (speed == 0){
                return "Normal";
            } else if (speed < 0){
                return "Slow " + getString(abs(speed));
            } else if (speed > 0){
                return "Fast " + getString(speed);
            }
            return std::string();
        }
};

class OneVsTeam : public Option {
    public:
	OneVsTeam(){
	    optionName = "1P VS Team Advantage";
	    currentValue = getString(Data::getInstance().getTeam1vs2Life());
	}
	~OneVsTeam(){
	}
	void next(){
	}
	void prev(){
	}
};

class TeamLoseOnKO : public Option {
    public:
	TeamLoseOnKO(){
	    optionName = "If player KOed";
	    currentValue = getString(Data::getInstance().getTeamLoseOnKO());
	}
	~TeamLoseOnKO(){
	}
	void next(){
	}
	void prev(){
	}
};

class Escape: public Option {
public:
    Escape(){
        optionName = "Return to Main Menu";
        currentValue = "(Esc)";
    }

    virtual ~Escape(){
    }

    void next(){
    }

    void prev(){
    }

    void enter(){
        // **FIXME Hack figure something out
        throw Exception::Return(__FILE__, __LINE__);
    }
};

OptionOptions::OptionOptions( const std::string &name ) throw( LoadException ){
    if (name.empty()){
	throw LoadException(__FILE__, __LINE__, "No name given to Options");
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
    for (vector<class Option *>::iterator i = options.begin(); i != options.end(); ++i){
	if (*i){
	    delete *i;
	}
    }
}

void OptionOptions::executeOption(const PlayerType & player, bool &endGame){
    Filesystem::AbsolutePath systemFile = Data::getInstance().getFileFromMotif(Data::getInstance().getMotif());
    // Lets look for our def since some people think that all file systems are case insensitive
    Filesystem::AbsolutePath baseDir = systemFile.getDirectory();
    
    Global::debug(1) << baseDir.path() << endl;
    
    if (systemFile.isEmpty()){
        throw MugenException( "Cannot locate character select definition file for: " + systemFile.path());
    }

    TimeDifference diff;
    diff.startTime();
    Ast::AstParse parsed(Util::parseDef(systemFile.path()));
    diff.endTime();
    Global::debug(1) << "Parsed mugen file " + systemFile.path() + " in" + diff.printTime("") << endl;
    
    Background * background = 0;
    std::vector< MugenFont *> fonts;
    SoundMap sounds;
    
    Point moveSound;
    Point doneSound;
    Point cancelSound;
    
    for (Ast::AstParse::section_iterator section_it = parsed.getSections()->begin(); section_it != parsed.getSections()->end(); section_it++){
        Ast::Section * section = *section_it;
	std::string head = section->getName();
	if (head == "Files"){
            class FileWalker: public Ast::Walker {
                public:
                    FileWalker(std::vector<MugenFont *> & fonts, SoundMap & sounds, const Filesystem::AbsolutePath & baseDir):
                        fonts(fonts),
			sounds(sounds),
                        baseDir(baseDir){
                        }

                    std::vector<MugenFont *> & fonts;
		    SoundMap & sounds;
                    const Filesystem::AbsolutePath & baseDir;

                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "snd"){
			    std::string file;
                            simple >> file;
                            Util::readSounds(Util::getCorrectFileLocation(baseDir, file ), sounds);
                            Global::debug(1) << "Got Sound File: '" << file << "'" << endl;
                        } else if (PaintownUtil::matchRegex(simple.idString(), "^font")){
                            std::string temp;
                            simple >> temp;
                            fonts.push_back(new MugenFont(Util::getCorrectFileLocation(baseDir, temp)));
                            Global::debug(1) << "Got Font File: '" << temp << "'" << endl;
                        }
                    }
            };
            FileWalker walker(fonts, sounds, baseDir);
            section->walk(walker);
        } else if (head == "Option Info"){
	    class InfoWalker: public Ast::Walker{
                public:
                    InfoWalker(Point & moveSound, Point & doneSound, Point & cancelSound):
                        moveSound(moveSound),
                        doneSound(doneSound),
			cancelSound(cancelSound){
                        }

                    Point & moveSound;
		    Point & doneSound;
		    Point & cancelSound;

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
	    background = new Background(systemFile, "optionbg");
	}
    }
    
    // Run options
    Bitmap workArea(DEFAULT_WIDTH,DEFAULT_HEIGHT);
    Bitmap screen(Configuration::getScreenWidth(), Configuration::getScreenHeight());
    bool done = false;
    bool escaped = false;
    
    int ticker = 0;
  
    double runCounter = 0;
    Global::speed_counter = 0;
    Global::second_counter = 0;
    int game_time = 100;
    
    // Set game keys temporary
    InputMap<Keys> player1Input = getPlayer1Keys(20);
    InputMap<Keys> player2Input = getPlayer2Keys(20);
    
    // Our Font
    MugenFont * font = fonts[1];
    
    // Box
    Box optionArea;
    
    //optionArea.location.setDimensions(Gui::AbsolutePoint(260),210);
    optionArea.location.setPosition(Gui::AbsolutePoint((DEFAULT_WIDTH/2) - (100), (DEFAULT_HEIGHT/2) - (90)));
    optionArea.location.setPosition2(Gui::AbsolutePoint(260,210));
    
    optionArea.location.setRadius(5);
    optionArea.colors.body = Bitmap::makeColor(0,0,60);
    optionArea.colors.bodyAlpha = 150;
    optionArea.colors.border = Bitmap::makeColor(0,0,20);
    
    
    while (!done){
    
	bool draw = false;
	
	if ( Global::speed_counter > 0 ){
	    draw = true;
	    runCounter += Global::speed_counter * Global::LOGIC_MULTIPLIER;
	    Global::speed_counter = 0;
	    while (runCounter >= 1.0){
		// tick tock
		ticker++;
		
		runCounter -= 1;
		// Key handler
		InputManager::poll();
		
		// InputMap<Keys>::Output out1 = InputManager::getMap(player1Input);
		// InputMap<Keys>::Output out2 = InputManager::getMap(player2Input);

                vector<InputMap<Mugen::Keys>::InputEvent> out1 = InputManager::getEvents(player1Input);
                vector<InputMap<Mugen::Keys>::InputEvent> out2 = InputManager::getEvents(player2Input);
                out1.insert(out1.end(), out2.begin(), out2.end());
                for (vector<InputMap<Mugen::Keys>::InputEvent>::iterator it = out1.begin(); it != out1.end(); it++){
                    const InputMap<Mugen::Keys>::InputEvent & event = *it;
                    if (!event.enabled){
                        continue;
                    }


                    if (event[Esc]){
                        done = escaped = true;
                        if (sounds[cancelSound.x][cancelSound.y]){
                            sounds[cancelSound.x][cancelSound.y]->play();
                        }
                        InputManager::waitForRelease(player1Input, Esc);
                        InputManager::waitForRelease(player2Input, Esc);
                    }

                    if (event[Up]){
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
                    if (event[Down]){
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
                    if (event[Left]){
                        (*selectedOption)->prev();
                    }
                    if (event[Right]){
                        (*selectedOption)->next();
                    }
                    if (event[Start]){
                        (*selectedOption)->enter();
                    }
                    // Backgrounds
                    background->act();
                }
            }
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
	    
	    optionArea.render(workArea);
	    
	    doOptions(*font,optionArea.location.getX(),optionArea.location.getY(),workArea);
	    
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
	throw Exception::Return(__FILE__, __LINE__);
    }
}

void OptionOptions::doOptions(MugenFont & font, int x, int y, const Bitmap & bmp){
    int mod = 30;
    for (vector<class Option *>::iterator i = options.begin(); i != options.end(); ++i){
	class Option * option = *i;
	option->render(font, x, y+mod, bmp);
	mod+=20;
    }
}

}

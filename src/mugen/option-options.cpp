#include "util/bitmap.h"
#include "util/trans-bitmap.h"
#include "util/stretch-bitmap.h"
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
#include "search.h"

#include <ostream>
#include <vector>

#include "ast/all.h"
#include "parser/all.h"
#include "util/events.h"
#include "util/init.h"
#include "util/funcs.h"
#include "util/file-system.h"
#include "util/timedifference.h"
#include "util/input/input-manager.h"
#include "exceptions/exception.h"

#include "util/gui/box.h"

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

void Option::render(MugenFont & font, int x, int y, const Graphics::Bitmap & bmp){
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
        Graphics::Bitmap::transBlender(0,0,0,alpha);
	bmp.translucent().rectangleFill(x+2, y-10, rightX+2, y+2, Graphics::makeColor(255,255,255));
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

OptionOptions::OptionOptions( const std::string &name ){
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
        throw MugenException("Cannot locate character select definition file for: " + systemFile.path());
    }

    TimeDifference diff;
    diff.startTime();
    Ast::AstParse parsed(Util::parseDef(systemFile.path()));
    diff.endTime();
    Global::debug(1) << "Parsed mugen file " + systemFile.path() + " in" + diff.printTime("") << endl;
    
    Background * background = NULL;
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
                            Util::readSounds(Util::findFile(Filesystem::RelativePath(file)), sounds);
                            Global::debug(1) << "Got Sound File: '" << file << "'" << endl;
                        } else if (PaintownUtil::matchRegex(simple.idString(), "^font")){
                            std::string temp;
                            simple >> temp;
                            fonts.push_back(new MugenFont(Util::findFile(Filesystem::RelativePath(temp))));
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

    /* FIXME: do all other cleanup here too */
    if (background == NULL){
        throw MugenException("OptionBGDef was not specified");
    }
    
    // Run options
    bool done = false;
    bool escaped = false;
    
    int ticker = 0;
  
    double runCounter = 0;
    // Global::speed_counter = 0;
    Global::second_counter = 0;
    // int game_time = 100;
    
    // Set game keys temporary
    // InputMap<Keys> player1Input = getPlayer1Keys(20);
    // InputMap<Keys> player2Input = getPlayer2Keys(20);
    
    // Our Font
    MugenFont * font = fonts[1];
    
    // Box
    
    class Logic: public PaintownUtil::Logic {
    public:
        Logic(bool & escaped, MugenSound * cancel, MugenSound * move, const vector<class Option*> & options, vector<class Option *>::const_iterator & selectedOption, Background * background):
        escaped(escaped),
        logic_done(false),
        cancelSound(cancel),
        moveSound(move),
        background(background),
        options(options),
        selectedOption(selectedOption){
            player1Input = getPlayer1Keys(20);
            player2Input = getPlayer2Keys(20);
        }
    
        InputMap<Keys> player1Input;
        InputMap<Keys> player2Input;

        bool & escaped;
        bool logic_done;

        MugenSound * cancelSound;
        MugenSound * moveSound;
    
        Background * background;

        const vector<class Option *> & options;
        vector<class Option *>::const_iterator & selectedOption;

        double ticks(double system){
	    return Util::gameTicks(system);
        }

        void run(){
            vector<InputMap<Mugen::Keys>::InputEvent> out1 = InputManager::getEvents(player1Input);
            vector<InputMap<Mugen::Keys>::InputEvent> out2 = InputManager::getEvents(player2Input);
            out1.insert(out1.end(), out2.begin(), out2.end());
            for (vector<InputMap<Mugen::Keys>::InputEvent>::iterator it = out1.begin(); it != out1.end(); it++){
                const InputMap<Mugen::Keys>::InputEvent & event = *it;
                if (!event.enabled){
                    continue;
                }


                if (event[Esc]){
                    logic_done = escaped = true;
                    if (cancelSound != NULL){
                        cancelSound->play();
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
                    if (moveSound != NULL){
                        moveSound->play();
                    }
                }
                if (event[Down]){
                    (*selectedOption)->toggleSelected();
                    selectedOption++;
                    if (selectedOption == options.end()){
                        selectedOption = options.begin();
                    }
                    (*selectedOption)->toggleSelected();
                    if (moveSound != NULL){
                        moveSound->play();
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
            }

            // Backgrounds
            background->act();
        }

        bool done(){
            return logic_done;
        }
    };

    class Draw: public PaintownUtil::Draw {
    public:
        Draw(Background * background, MugenFont * font, const vector<class Option *> & options):
        background(background),
        font(font),
        options(options){
            //optionArea.location.setDimensions(Gui::AbsolutePoint(260),210);
            optionArea.location.setPosition(Gui::AbsolutePoint((DEFAULT_WIDTH/2) - (100), (DEFAULT_HEIGHT/2) - (90)));
            optionArea.location.setPosition2(Gui::AbsolutePoint(260,210));

            optionArea.transforms.setRadius(5);
            optionArea.colors.body = Graphics::makeColor(0,0,60);
            optionArea.colors.bodyAlpha = 150;
            optionArea.colors.border = Graphics::makeColor(0,0,20);
        }

        Background * background;
        MugenFont * font;
        Box optionArea;
        const vector<class Option *> & options;

        void doOptions(MugenFont & font, int x, int y, Graphics::Bitmap & where){
            /* where do the numbers 30 and 20 come from? */
            int mod = 30;
            for (vector<class Option *>::const_iterator i = options.begin(); i != options.end(); ++i){
                class Option * option = *i;
                option->render(font, x, y+mod, where);
                mod += 20;
            }
        }

        void draw(const Graphics::Bitmap & screen){
            Graphics::StretchedBitmap workArea(DEFAULT_WIDTH, DEFAULT_HEIGHT, screen);
            workArea.start();
            // render backgrounds
	    background->renderBackground(0, 0, workArea);
	    
	    // render fonts
	    font->render(DEFAULT_WIDTH/2, 20, 0, 0, workArea, "OPTIONS" );
	    
	    optionArea.render(workArea);
	    
	    doOptions(*font, optionArea.location.getX(), optionArea.location.getY(), workArea);
	    
	    // render Foregrounds
	    background->renderForeground(0, 0, workArea);
	    
	    // Finally render to screen
            workArea.finish();
	    // workArea.Stretch(screen);
	    screen.BlitToScreen();
        }
    };

    MugenSound * cancel = NULL;
    if (sounds[cancelSound.x][cancelSound.y]){
        cancel = sounds[cancelSound.x][cancelSound.y];
    }
    MugenSound * move = NULL;
    if (sounds[moveSound.x][moveSound.y]){
        move = sounds[moveSound.x][moveSound.y];
    }
    Logic logic(escaped, cancel, move, options, selectedOption, background);
    Draw draw(background, font, options);
    PaintownUtil::standardLoop(logic, draw);
    
#if 0
    while (!done){
    
	bool draw = false;
	
	if ( Global::speed_counter > 0 ){
	    draw = true;
	    runCounter += Util::gameTicks(Global::speed_counter);
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
                }

                // Backgrounds
                background->act();
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
#endif
    
    delete background;
    
    // Get rid of sprites
    for (std::vector<MugenFont *>::iterator i = fonts.begin() ; i != fonts.end() ; ++i){
	if (*i){
	    delete *i;
	}
    }
    // Get rid of sounds
    for (std::map< unsigned int, std::map< unsigned int, MugenSound * > >::iterator i = sounds.begin() ; i != sounds.end() ; ++i){
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

/*
void OptionOptions::doOptions(MugenFont & font, int x, int y, const Graphics::Bitmap & bmp){
    int mod = 30;
    for (vector<class Option *>::iterator i = options.begin(); i != options.end(); ++i){
	class Option * option = *i;
	option->render(font, x, y+mod, bmp);
	mod+=20;
    }
}
*/

OptionArcade::OptionArcade(const string & name, Searcher & searcher):
searcher(searcher){
    if (name.empty()){
	throw LoadException(__FILE__, __LINE__, "No name given to versus");
    }

    this->setText(name);
}

OptionArcade::~OptionArcade(){
	// Nothing
}

static void runGame(const PlayerType & player, GameType kind, Searcher & searcher){
    Game versus(player, kind, Data::getInstance().getFileFromMotif(Data::getInstance().getMotif()));
    versus.run(searcher);
}

void OptionArcade::executeOption(const Mugen::PlayerType & player, bool &endGame){
    /* Get default motif system.def */
    /*
    Game versus(player, Arcade, Data::getInstance().getFileFromMotif(Data::getInstance().getMotif()));
    versus.run();
    */
    runGame(player, Arcade, searcher);
}

OptionVersus::OptionVersus(const std::string &name, Searcher & searcher):
searcher(searcher){
    if (name.empty()){
	throw LoadException(__FILE__, __LINE__, "No name given to versus");
    }
    this->setText(name);
}

OptionVersus::~OptionVersus(){
	// Nothing
}

void OptionVersus::executeOption(const Mugen::PlayerType & player, bool &endGame){
    /* Get motif system.def */
    /*
    Game versus(player, Versus, Data::getInstance().getFileFromMotif(Data::getInstance().getMotif()));;
    versus.run();
    */

    runGame(player, Versus, searcher);
}

}

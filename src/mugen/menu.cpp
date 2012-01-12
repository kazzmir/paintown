#include "util/bitmap.h"
#include "util/trans-bitmap.h"
#include "util/stretch-bitmap.h"
#include "menu.h"

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

#include "stage.h"

#include "util/exceptions/exception.h"
#include "util/init.h"
#include "util/resource.h"
#include "util/funcs.h"
#include "util/thread.h"
#include "util/file-system.h"
#include "util/timedifference.h"
#include "util/music.h"
#include "util/events.h"
#include "util/system.h"
#include "util/parameter.h"

// #include "game/console.h"
/*
#include "object/animation.h"
#include "object/object.h"
#include "object/character.h"
#include "object/object_attack.h"
#include "object/player.h"
*/
#include "globals.h"
#include "factory/font_render.h"
#include "parse-cache.h"

/*
#include "menu/menu_option.h"
#include "menu/options.h"
*/

#include "util/input/input-manager.h"
#include "util/input/input-source.h"

#include "animation.h"
#include "background.h"
#include "config.h"
#include "character-select.h"
#include "character.h"
#include "item.h"
#include "item-content.h"
#include "section.h"
#include "sound.h"
#include "reader.h"
#include "sprite.h"
#include "util.h"
#include "font.h"
#include "storyboard.h"

#include "options.h"
#include "game.h"
#include "ast/all.h"
#include "parser/all.h"
#include "util/loading.h"

#include "parse-cache.h"

namespace PaintownUtil = ::Util;

using namespace std;

static const int DEFAULT_WIDTH = 320;
static const int DEFAULT_HEIGHT = 240;
static const int DEFAULT_SCREEN_X_AXIS = 160;
static const int DEFAULT_SCREEN_Y_AXIS = 0;

static const int CURSOR_ALPHA_MOVE = 3;
CursorHandler::CursorHandler(){
    // It seems that mugen defaults to something
    cursor.x1 = -32;
    cursor.y1 = -10;
    cursor.x2 = 32;    
    cursor.y2 = 2;
    cursor.visible = true;
    cursor.alpha = 128;
    cursor.alphaMove = -CURSOR_ALPHA_MOVE;
}

CursorHandler::~CursorHandler(){
}

void CursorHandler::act(){
    if (cursor.visible){
	cursor.alpha += cursor.alphaMove;
	if (cursor.alpha <= 20){
	    cursor.alpha = 20;
	    cursor.alphaMove = CURSOR_ALPHA_MOVE;
	}
	else if (cursor.alpha >= 64){
	    cursor.alpha = 64;
	    cursor.alphaMove = -CURSOR_ALPHA_MOVE;
	}
    }
}

void CursorHandler::renderCursor(int x, int y, const Graphics::Bitmap & bmp){
    if (cursor.visible){
        Graphics::Bitmap::transBlender(0, 0, 0, cursor.alpha);
	bmp.translucent().rectangleFill(x + cursor.x1, y + cursor.y1, x + cursor.x2, y + cursor.y2, Graphics::makeColor(255,255,255));
    }
}

void CursorHandler::renderText(int x, int y, bool active, const std::string & text, std::vector<MugenFont *> & fonts, const Graphics::Bitmap & bmp){
    if (active){
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
currentState(Deselected),
runnable(true){
}

Mugen::ItemOption::~ItemOption(){
}

void Mugen::ItemOption::logic(){
}

void Mugen::ItemOption::render(int x, int y, CursorHandler & handler,  std::vector<MugenFont *> & fonts, const Graphics::Bitmap & bmp){
    handler.renderText(x, y, (getState() == Selected), getText(), fonts, bmp);
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

class WatchOption: public ItemOption {
public:
    WatchOption(const std::string & name){
        this->setText(name);
    }

    virtual ~WatchOption(){
    }

    void executeOption(const Mugen::PlayerType & player, bool & endGame){
        /*
        Mugen::Game watch(player, Mugen::Watch, Mugen::Data::getInstance().getFileFromMotif(Mugen::Data::getInstance().getMotif()));
        watch.run(searcher);
        */
        throw StartGame(player, Mugen::Watch);
    }
};

class TrainingOption: public ItemOption {
public:
    TrainingOption(const std::string & name){
        this->setText(name);
    }

    virtual ~TrainingOption(){
    }

    void executeOption(const Mugen::PlayerType & player, bool & endGame){
        /*
        Mugen::Game game(player, Mugen::Training, Mugen::Data::getInstance().getFileFromMotif(Mugen::Data::getInstance().getMotif()));;
        game.run(searcher);
        */
        throw StartGame(player, Mugen::Training);
    }
};

}

MugenMenu::MugenMenu(const Filesystem::RelativePath &filename):
optionLocation(0),
location(filename),
spriteFile(""),
soundFile(""),
logoFile(""),
introFile(""),
selectFile(""),
fightFile(""),
currentMenuPosition(0),
menuScrollWait(4),
fontSpacing(0,13),
windowMargin(12,8),
windowVisibleItems(5),
ticker(0),
background(0),
logo(0),
intro(0),
characterSelect(0),
moveText(true){
}

void MugenMenu::loadData(){
    Filesystem::AbsolutePath baseDir = Storage::instance().find(Mugen::Data::getInstance().getDirectory().join(location.getDirectory()));
    const Filesystem::AbsolutePath ourDefFile = Mugen::Util::fixFileName(baseDir, location.getFilename().path());
    // get real basedir
    //baseDir = Mugen::Util::getFileDir( ourDefFile );
    Global::debug(1) << baseDir.path() << endl;

    if (ourDefFile.isEmpty()){
        throw MugenException( "Cannot locate menu definition file for: " + location.path(), __FILE__, __LINE__);
    }

    try{
        TimeDifference diff;
        diff.startTime();
        AstRef parsed(Mugen::Util::parseDef(ourDefFile.path()));
        diff.endTime();
        Global::debug(1) << "Parsed mugen file " + ourDefFile.path() + " in" + diff.printTime("") << endl;

        /*
           MugenReader reader( ourDefFile );
           std::vector< MugenSection * > collection;
           collection = reader.getCollection();
           */

        for (Ast::AstParse::section_iterator section_it = parsed->getSections()->begin(); section_it != parsed->getSections()->end(); section_it++){
            Ast::Section * section = *section_it;
            std::string head = section->getName();
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
                            try{
                                menu.setName(simple.valueAsString());
                                Global::debug(1) << "Read name '" << menu.getName() << "'" << endl;
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "author"){
                            try{
                                string temp;
                                simple.view() >> temp;
                                Global::debug(1) << "Made by: '" << temp << "'" << endl;
                            } catch (const Ast::Exception & e){
                            }
                        } else {
                            //throw MugenException("Unhandled option in Info Section: " + simple.toString(), __FILE__, __LINE__);
                            Global::debug(0) << "Unhandled option in Info Section: " << simple.toString() << __FILE__ << __LINE__ << endl;
                        }
                    }
                };

                InfoWalker walker(*this);
                section->walk(walker);
            } else if (head == "files"){
                class FileWalker: public Ast::Walker{
                public:
                    FileWalker(MugenMenu & menu, const Filesystem::AbsolutePath & baseDir):
                        menu(menu),
                        baseDir(baseDir){
                        }

                    MugenMenu & menu;
                    const Filesystem::AbsolutePath & baseDir;

                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "spr"){
                            simple.view() >> menu.spriteFile;
                            Global::debug(1) << "Got Sprite File: '" << menu.spriteFile << "'" << endl;
                            Mugen::Util::readSprites(Mugen::Util::findFile(Filesystem::RelativePath(menu.spriteFile)), Filesystem::AbsolutePath(), menu.sprites, false);
                            Mugen::Util::destroyRaw(menu.sprites);
                        } else if (simple == "snd"){
                            simple.view() >> menu.soundFile;
                            Mugen::Util::readSounds(Mugen::Util::findFile(Filesystem::RelativePath(menu.soundFile)), menu.sounds);
                            Global::debug(1) << "Got Sound File: '" << menu.soundFile << "'" << endl;
                        } else if (simple == "logo.storyboard"){
                            try{
                                simple.view() >> menu.logoFile;
                                if (!menu.logoFile.empty()){
                                    try{
                                        Global::debug(1) << "Logo file " << baseDir.path() << "/" << menu.logoFile << endl;
                                        menu.logo = new Mugen::Storyboard(Mugen::Util::findFile(Filesystem::RelativePath(menu.logoFile)), false);
                                        Global::debug(1) << "Got Logo Storyboard File: '" << menu.logoFile << "'" << endl;
                                    } catch (const MugenException &ex){
                                        Global::debug(0) << "Error loading logo storyboard " << menu.logoFile << ": " << ex.getFullReason() << std::endl;
                                    }
                                }
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "intro.storyboard"){
                            try{
                                simple.view() >> menu.introFile;
                                if (!menu.introFile.empty()){
                                    try{
                                        Global::debug(1) << "Intro file " << baseDir.path() << "/" << menu.introFile << endl;
                                        menu.intro = new Mugen::Storyboard(Mugen::Util::findFile(Filesystem::RelativePath(menu.introFile)), true);
                                        Global::debug(1) << "Got Intro Storyboard File: '" << menu.introFile << "'" << endl;
                                    } catch (const MugenException &ex){
                                        // throw MugenException("Error loading intro storyboard " + menu.introFile + ": " + ex.getFullReason(), __FILE__, __LINE__);
                                        Global::debug(0) << "Error loading intro storyboard " << menu.introFile << ": " << ex.getFullReason() << std::endl;
                                    }
                                }
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "select"){
                            simple.view() >> menu.selectFile;
                            Global::debug(1) << "Got Select File: '" << menu.selectFile << "'" << endl;
                        } else if (simple == "fight"){
                            simple.view() >> menu.fightFile;
                            Global::debug(1) << "Got Fight File: '" << menu.fightFile << "'" << endl;
                        } else if (PaintownUtil::matchRegex(simple.idString(), "^font[0-9]*")){
                            string temp;
                            simple.view() >> temp;
                            try{
                                Filesystem::AbsolutePath path = Mugen::Util::findFont(Filesystem::RelativePath(temp));

                                if (true){
                                    menu.fonts.push_back(new MugenFont(path));
                                    Global::debug(1) << "Got Font File: '" << temp << "'" << endl;
                                }
                            } catch (const Filesystem::NotFound & fail){
                                Global::debug(0) << "Could not find font '" << temp << "' " << fail.getTrace() << endl;
                            } catch (const LoadException & fail){
                                Global::debug(0) << "Could not load font '" << temp << "' " << fail.getTrace() << endl;
                            }

                        } else {
                            //throw MugenException("Unhandled option in Files Section: " + simple.toString(), __FILE__, __LINE__ );
                            Global::debug(0) << "Unhandled option in Files Section: " << simple.toString() << " at " << __FILE__ << ":" << __LINE__ << endl;
                        }
                    }
                };

                FileWalker walker(*this, baseDir);
                section->walk(walker);
            } else if (head == "music"){
                class MusicInfoWalker: public Ast::Walker{
		public:
		    MusicInfoWalker(MugenMenu & menu):
			menu(menu){
			}
		    MugenMenu & menu;
		    
		    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "title.bgm"){
                            try{
                                simple.view() >> menu.titleMusic;
                            } catch (const Ast::Exception & fail){
                            }
                        } else if (simple == "title.bgm.loop"){
                        } else if (simple == "select.bgm"){
                            try{
                                simple.view() >> menu.selectMusic;
                            } catch (const Ast::Exception & fail){
                            }
                        } else if (simple == "select.bgm.loop"){
                        } else if (simple == "vs.bgm"){
                            try{
                                simple.view() >> menu.versusMusic;
                            } catch (const Ast::Exception & fail){
                            }
                        } else if (simple == "vs.bgm.loop"){
                        } else {
                            //throw MugenException("Unhandled option in Files Section: " + simple.toString(), __FILE__, __LINE__ );
                            Global::debug(0) << "Unhandled option in Music Section: " << simple.toString() << " at " << __FILE__ << ":" << __LINE__ << endl;
                        }
		    }
		};
		MusicInfoWalker walker(*this);
		section->walk(walker);
            } else if (head == "title info"){
                class TitleInfoWalker: public Ast::Walker{
                public:
                    TitleInfoWalker(MugenMenu & menu):
                        menu(menu){
                        }

                    MugenMenu & menu;

                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "fadein.time"){
                            try{
                                int time;
                                simple.view() >> time;
                                menu.fader.setFadeInTime(time);
                            } catch (const Ast::Exception & fail){
                            }
                        } else if (simple == "fadein.color"){
                            try{
                                int r,g,b;
                                simple.view() >> r >> g >> b;
                                menu.fader.setFadeInColor(Graphics::makeColor(r,g,b));
                            } catch (const Ast::Exception & fail){
                            }
                        } else if (simple == "fadeout.time"){
                            try{
                                int time;
                                simple.view() >> time;
                                menu.fader.setFadeOutTime(time);
                            } catch (const Ast::Exception & fail){
                            }
                        } else if (simple == "fadeout.color"){
                            int r,g,b;
                            simple.view() >> r >> g >> b;
                            menu.fader.setFadeOutColor(Graphics::makeColor(r,g,b));
                        } else if (simple == "menu.pos"){
                            simple.view() >> menu.position.x >> menu.position.y;
                        } else if (simple == "menu.item.font"){
                            int index=0,bank=0,position=0;
                            try{
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                            }
                            menu.fontCursor.setItemFont(index,bank,position);
                        } else if (simple == "menu.item.active.font"){
                            int index=0,bank=0,position=0;
                            try{
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                            }
                            menu.fontCursor.setActiveFont(index,bank,position);
                        } else if (simple == "menu.item.spacing"){
                            try{
                                simple.view() >> menu.fontSpacing.x >> menu.fontSpacing.y;
                            } catch (const Ast::Exception & e){
                            }
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
                                menu.addMenuOption(new Mugen::TrainingOption(simple.valueAsString()));
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "menu.itemname.watch"){
                            try{
                                menu.addMenuOption(new Mugen::WatchOption(simple.valueAsString()));
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
                            simple.view() >> menu.windowMargin.x >> menu.windowMargin.y;
                            // Undocumented but it defaults to 5 if it's 0 or not specified
                            if (menu.windowMargin.y == 0){
                                menu.windowMargin.y = 5;
                            }
                        } else if (simple == "menu.window.visibleitems"){
                            simple.view() >> menu.windowVisibleItems;
                        } else if (simple == "menu.boxcursor.visible"){
                            bool visible = false;
                            simple.view() >> visible;
                            menu.fontCursor.setCursorVisible(visible);
                        } else if (simple == "menu.boxcursor.coords"){
                            int x1=0,y1=0,x2=0,y2=0;
                            try{
                                simple.view() >> x1 >> y1 >> x2 >> y2;
                            } catch (const Ast::Exception & e){
                            }
                            menu.fontCursor.setCursor(x1,y1,x2,y2);
                        } else if (simple == "cursor.move.snd"){
                            try{
                                simple.view() >> menu.moveSound.x >> menu.moveSound.y;
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "cursor.done.snd"){
                            try{
                                simple.view() >> menu.doneSound.x >> menu.doneSound.y;
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "cancel.snd"){
                            int g=0, s=0;
                            try{
                                simple.view() >> menu.cancelSound.x >> menu.cancelSound.y;
                            } catch (const Ast::Exception & e){
                            }
                        } else {
                            //throw MugenException("Unhandled option in Info Section: " + simple.toString(), __FILE__, __LINE__);
                            Global::debug(0) << "Unhandled option in Title Info Section: " << simple.toString() << __FILE__ << __LINE__;
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
            else if (head.find("begin action") != std::string::npos ){ /* Ignore for now */ }
            else {
                //throw MugenException("Unhandled Section in '" + ourDefFile + "': " + head, __FILE__, __LINE__ );
                ostringstream context;
                context << __FILE__ << ":" << __LINE__;
                Global::debug(0, context.str()) << "Unhandled Section: " << head << endl;
            }
        } 

        // Set defaults
        currentMenuPosition = position.y;
        menuRange.y = windowVisibleItems-1;

    } catch (const Mugen::Def::ParseException & e){
        ostringstream out;
        out << "Error loading data " << ourDefFile.path() << ": " << e.getReason();
        throw MugenException(out.str(), __FILE__, __LINE__);
    } catch (const Exception::Base & fail){
        ostringstream out;
        out << "Error loading data " << ourDefFile.path() << ": " << fail.getTrace();
        throw MugenException(out.str(), __FILE__, __LINE__);
    }

    if (fonts.size() == 0){
        throw MugenException("No fonts specified", __FILE__, __LINE__);
    }
}

MugenMenu::~MugenMenu(){
    // cleanup
    cleanup();
}

/*
bool MugenMenu::doInput(InputMap<Mugen::Keys> & input, Mugen::PlayerType & chosenPlayer, Mugen::PlayerType type){
    bool quit = false;
    vector<InputMap<Mugen::Keys>::InputEvent> out1 = InputManager::getEvents(input);
    for (vector<InputMap<Mugen::Keys>::InputEvent>::iterator it = out1.begin(); it != out1.end(); it++){
        const InputMap<Mugen::Keys>::InputEvent & event = *it;
        if (!event.enabled){
            continue;
        }

        switch (event.out){
            case Mugen::Up: {
                moveMenuUp();
                break;
            }
            case Mugen::Down: {
                moveMenuDown();
                break;
            }
            case Mugen::Enter:
            case Mugen::Right:
            case Mugen::Left: {
                break;
            }
            case Mugen::A:
            case Mugen::B:
            case Mugen::C:
            case Mugen::X:
            case Mugen::Y:
            case Mugen::Z:
            case Mugen::Start: {
                if ((*currentOption)->isRunnable()){
                    (*currentOption)->setState(MenuOption::Run);
                }
                // Set the fade state
                fader.setState(Gui::FadeTool::FadeOut);
                playSound(Done);
                chosenPlayer = type;
                break;
            }
            case Mugen::Esc: {
                quit = true;
                // Set the fade state
                fader.setState(Gui::FadeTool::FadeOut);
                (*currentOption)->setState(MenuOption::Deselected);
                InputManager::waitForRelease(input, Mugen::Esc);
                playSound(Cancel);
                break;
            }
        }
    }

    return quit;
}
*/

void MugenMenu::run(){
    bool done = false;
    bool endGame = false;

    /*
    class StartStop{
    public:
        StartStop(Mugen::Searcher & searcher):
        searcher(searcher){
            searcher.start();
        }

        Mugen::Searcher & searcher;

        ~StartStop(){
            searcher.pause();
        }
    };

    StartStop check(searcher);
    */
    
    if (options.empty()){
        return;
    }
    
    currentOption = options.begin();
    optionLocation = 0;
    options.front()->setState(Mugen::ItemOption::Selected);
    
    // Set the fade state
    fader.setState(Gui::FadeTool::FadeIn);
    
    // Keys
    InputMap<Mugen::Keys> player1Input = Mugen::getPlayer1Keys(20);
    InputMap<Mugen::Keys> player2Input = Mugen::getPlayer2Keys(20);
    
    // Selecting player
    // Mugen::PlayerType selectingPlayer;
  
    // Do we have logos or intros?
    // Logo run it no repeat
    if (logo){
        logo->setInput(player1Input);
	logo->run(false);
    }

    // Intro run it no repeat
    if (intro){
        intro->setInput(player1Input);
	intro->run(false);
    }

    class Logic: public PaintownUtil::Logic {
    public:
        Logic(bool & endGame, vector<Mugen::ItemOption *>::iterator & currentOption, Gui::FadeTool & fader, int & ticker, InputMap<Mugen::Keys> & player1Input, InputMap<Mugen::Keys> & player2Input, MugenMenu & menu, Mugen::Background * background):
        is_done(false),
        endGame(endGame),
        currentOption(currentOption),
        fader(fader),
        ticker(ticker),
        player1Input(player1Input),
        player2Input(player2Input),
        menu(menu),
        background(background){
        }

        bool is_done;
        bool & endGame;
	vector<Mugen::ItemOption *>::iterator & currentOption;
	Gui::FadeTool & fader;
        int & ticker;
    
        InputMap<Mugen::Keys> & player1Input;
        InputMap<Mugen::Keys> & player2Input;
    
        Mugen::PlayerType selectingPlayer;
                
        MugenMenu & menu;

        Mugen::Background * background;

        Mugen::PlayerType getSelectingPlayer() const {
            return selectingPlayer;
        }

        bool doInput(InputMap<Mugen::Keys> & input, Mugen::PlayerType & chosenPlayer, Mugen::PlayerType type){
            bool quit = false;
            InputSource source;
            vector<InputMap<Mugen::Keys>::InputEvent> out1 = InputManager::getEvents(input, source);
            for (vector<InputMap<Mugen::Keys>::InputEvent>::iterator it = out1.begin(); it != out1.end(); it++){
                const InputMap<Mugen::Keys>::InputEvent & event = *it;
                if (!event.enabled){
                    continue;
                }

                switch (event.out){
                    case Mugen::Up: {
                        menu.moveMenuUp();
                        break;
                    }
                    case Mugen::Down: {
                        menu.moveMenuDown();
                        break;
                    }
                    case Mugen::Enter:
                    case Mugen::Right:
                    case Mugen::Left: {
                        break;
                    }
                    case Mugen::A:
                    case Mugen::B:
                    case Mugen::C:
                    case Mugen::X:
                    case Mugen::Y:
                    case Mugen::Z:
                    case Mugen::Start: {
                        if ((*currentOption)->isRunnable()){
                            (*currentOption)->setState(Mugen::ItemOption::Run);
                        }
                        // Set the fade state
                        fader.setState(Gui::FadeTool::FadeOut);
                        menu.playSound(Done);
                        chosenPlayer = type;
                        break;
                    }
                    case Mugen::Esc: {
                        quit = true;
                        // Set the fade state
                        fader.setState(Gui::FadeTool::FadeOut);
                        (*currentOption)->setState(Mugen::ItemOption::Deselected);
                        InputManager::waitForRelease(input, source, Mugen::Esc);
                        menu.playSound(Cancel);
                        break;
                    }
                }
            }

            return quit;
        }

        bool done(){
            return is_done ||
                   ((*currentOption)->getState() == Mugen::ItemOption::Run) ||
                   (fader.getState() == Gui::FadeTool::EndFade);
        }

        void run(){
            ticker++;

            if (fader.getState() == Gui::FadeTool::NoFade){
                bool quit = false;
                quit = quit || doInput(player1Input, selectingPlayer, Mugen::Player1);
                quit = quit || doInput(player2Input, selectingPlayer, Mugen::Player2);
                endGame = is_done = quit;
            }

            // Update menu position
            menu.doMenuMovement(); 

            // Font Cursor
            menu.fontCursor.act();

            // Fader
            fader.act();

            // Backgrounds
            background->act();
        }

        double ticks(double system){
            return Mugen::Util::gameTicks(system);
        }
    };

    class Draw: public PaintownUtil::Draw {
    public:
        Draw(MugenMenu & menu, Mugen::Background * background, Gui::FadeTool & fader):
        menu(menu),
        // work(Global::getScreenWidth(), Global::getScreenHeight()),
        // workArea(DEFAULT_WIDTH, DEFAULT_HEIGHT),
        background(background),
        fader(fader){
        }

        MugenMenu & menu;

        // Graphics::Bitmap work;
        // Graphics::Bitmap workArea;

        Mugen::Background * background;

        Gui::FadeTool & fader;

        void draw(const Graphics::Bitmap & screen){
            Graphics::StretchedBitmap work(DEFAULT_WIDTH, DEFAULT_HEIGHT, screen);
            work.start();
            background->renderBackground(0, 0, work);

            // Draw any misc stuff in the background of the menu of selected object 
            //(*currentOption)->drawBelow(&work);
            // Draw text
            menu.renderText(&work);
            // Foregrounds
            background->renderForeground(0, 0, work);
            // Draw any misc stuff in the foreground of the menu of selected object 
            //(*currentOption)->drawAbove(&work);
            // Do fades
            fader.draw(work);
            // Finally render to screen
            // workArea.Stretch(work);
            work.finish();
            screen.BlitToScreen();


            /*
            // backgrounds
            background->renderBackground(0, 0, workArea);

            // Draw any misc stuff in the background of the menu of selected object 
            //(*currentOption)->drawBelow(&work);
            // Draw text
            menu.renderText(&workArea);
            // Foregrounds
            background->renderForeground(0, 0, workArea);
            // Draw any misc stuff in the foreground of the menu of selected object 
            //(*currentOption)->drawAbove(&work);
            // Do fades
            fader.draw(workArea);
            // Finally render to screen
            workArea.Stretch(work);
            work.BlitToScreen();
            */
        }
    };
  
    // double runCounter = 0;
    
    // Set move position info
    moveText = true;
    movePosition.x = 0;
    movePosition.y = DEFAULT_WIDTH;

    while (! endGame){
	// Global::speed_counter = 0;
	// Global::second_counter = 0;
	// int game_time = 100;
	
	/* Start music NOTE load select and versus screen bgm */
        try{
            if (titleMusic != ""){
                Music::loadSong(Mugen::Util::findFile(Filesystem::RelativePath(titleMusic)).path());
                Music::pause();
                Music::play();
            }
        } catch (const Filesystem::NotFound & fail){
            Global::debug(0) << "Warning: could not load music because " << fail.getTrace() << endl;
        }

        Logic logic(endGame, currentOption, fader, ticker, player1Input, player2Input, *this, background);
        Draw draw(*this, background, fader);
        /* run the menu */
        Util::standardLoop(logic, draw);
       
	// do we got an option to run, lets do it
	if ((*currentOption)->getState() == Mugen::ItemOption::Run){
	    try{
		(*currentOption)->executeOption(logic.getSelectingPlayer(), endGame);
	    } catch (const Exception::Return & re){
	    }
	    // Reset it's state
	    (*currentOption)->setState(Mugen::ItemOption::Selected);
	    fader.setState(Gui::FadeTool::FadeIn);
	}

	if (endGame){
	    // Deselect selected entry
	    (*currentOption)->setState(Mugen::ItemOption::Deselected);
	}
    }

    throw Exception::Return(__FILE__, __LINE__);
}

void MugenMenu::addMenuOption(Mugen::ItemOption * option){
    if (option){
	//option->setParent(this);
	options.push_back(option);
    }
}

void MugenMenu::cleanup(){
    
    //Backgrounds
    if (background){
        delete background;
    }
    
    // Get rid of sprites
    cleanupSprites();

    // Get rid of sounds
    for( std::map< unsigned int, std::map< unsigned int, MugenSound * > >::iterator i = sounds.begin() ; i != sounds.end() ; ++i ){
      for( std::map< unsigned int, MugenSound * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
	  if( j->second )delete j->second;
      }
    }
	
    for (vector<MugenFont *>::iterator it = fonts.begin(); it != fonts.end(); it++){
        delete *it;
    }

    for (vector<Mugen::ItemOption *>::iterator it = options.begin(); it != options.end(); it++){
        delete *it;
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

// Move menu up
void MugenMenu::moveMenuUp(){
    (*currentOption)->setState(Mugen::ItemOption::Deselected);
    if ( currentOption > options.begin() ){
        currentOption--;
        optionLocation--;
        if (optionLocation < menuRange.x){
            menuRange.x--;
            menuRange.y--;
        }
    } else { 
        currentOption = options.end() -1;
        optionLocation = options.size() -1;
        menuRange.x = options.size() - (windowVisibleItems);
        menuRange.y = options.size() - 1;
    }
    (*currentOption)->setState(Mugen::ItemOption::Selected);
    playSound(Move);
}

void MugenMenu::playSound(SoundType sound){
    switch (sound){
        case Move: playSound(moveSound.x, moveSound.y); break;
        case Cancel: playSound(cancelSound.x, cancelSound.y); break;
        case Done: playSound(doneSound.x, doneSound.y); break;
    }
}

void MugenMenu::playSound(int group, int item){
    if (sounds[group][item] != 0){
        sounds[group][item]->play();
    }
}

// Move menu down
void MugenMenu::moveMenuDown(){
    (*currentOption)->setState(Mugen::ItemOption::Deselected);
    if ( currentOption < options.begin()+options.size()-1 ){
        currentOption++;
        optionLocation++;
        if (optionLocation > menuRange.y){
            menuRange.x++;
            menuRange.y++;
        }
    } else {
        currentOption = options.begin();
        optionLocation = 0;
        menuRange.x = 0;
        menuRange.y = windowVisibleItems-1;
    }
    (*currentOption)->setState(Mugen::ItemOption::Selected);
    playSound(Move);
}

void MugenMenu::doMenuMovement(){
    //const int offset = optionLocation >= windowVisibleItems ? optionLocation - windowVisibleItems + 1 : 0;
    //const int endPosition = position.y - (offset * fontSpacing.y);
    const int endPosition = position.y - (menuRange.x * fontSpacing.y);
    if (currentMenuPosition == endPosition){
	menuScrollWait = 2;
    } else {
	if (menuScrollWait <= 0){
	    currentMenuPosition = (currentMenuPosition + endPosition)/2;
	    menuScrollWait = 2;
	} else {
	    menuScrollWait--;
	}
    }
}

// Draw text
void MugenMenu::renderText(Graphics::Bitmap *bmp){
    
    // Top of the window
    const int top = position.y - windowMargin.x;
    // Pretty accurate tested with 5 different screen packs and different settings
    const int bottom = position.y + ((windowVisibleItems-1) * fontSpacing.y) + windowMargin.y;

    const Graphics::Bitmap area(*bmp, 0, top, bmp->getWidth(), bottom - top);
    /* FIXME: avoid using clip rect */
    // bmp->setClipRect(0, top, bmp->getWidth(), bottom);
    // bmp->rectangle(0, top, bmp->getWidth() - 1, bottom - 1, Bitmap::makeColor(255,255,255));
    
    int xplacement = position.x;
    
    // Displace by the offset
    int yplacement = currentMenuPosition - top;
    
    bool alternate = false;
    
    for (std::vector <Mugen::ItemOption *>::iterator i = options.begin(); i != options.end(); ++i){
	
	Mugen::ItemOption *option = *i;
	
	// Render
	if (moveText){
	    if (!alternate){
		option->render(movePosition.x, yplacement, fontCursor, fonts, area);
	    } else {
		option->render(movePosition.y, yplacement, fontCursor, fonts, area);
	    }
	    // Displacement
	    movePosition.x += fontSpacing.x;
	    movePosition.y += fontSpacing.x;
	} else {
	    option->render(xplacement, yplacement, fontCursor, fonts, area);
	}
	// Displacement
	xplacement += fontSpacing.x;
	yplacement += fontSpacing.y;
	
	alternate = !alternate;
    }
    
    // *FIXME This should be moved into its own function out of render
    if (moveText){
        int speed = bmp->getWidth() / 15;
	if (movePosition.x < position.x){
	    movePosition.x += speed;
	    if (movePosition.x > position.x){
		movePosition.x = position.x;
	    }
	}

	if (movePosition.y > position.x){
	    movePosition.y -= speed;
	    if (movePosition.y < position.x){
		movePosition.y = position.x;
	    }
	}

	if (movePosition.x == position.x && movePosition.y == position.x){
	    moveText = false;
	}
    }
    
    // bmp->setClipRect(0,0,bmp->getWidth(),bmp->getHeight());
}

namespace Mugen{

static void runMenu(){
    // Load er up and throw up a load box to inform the user
    class Context: public Loader::LoadingContext {
    public:
        Context():
        menu(Mugen::Data::getInstance().getMotif()),
        exception(NULL){
        }

        virtual void load(){
            try{
                menu.loadData();
            } catch (const MugenException & e){
                exception = (MugenException*) e.copy();
            } catch (const LoadException & e){
                exception = new LoadException(e);
            } catch (const Filesystem::NotFound & e){
                exception = new Filesystem::NotFound(e);
            }
        }

        virtual void failure(){
            if (exception != NULL){
                exception->throwSelf();
            }
        }

        virtual ~Context(){
            delete exception;
        }

        MugenMenu & getMenu(){
            return menu;
        }

        MugenMenu menu;
        Exception::Base * exception;
    };
    
    Loader::Info info;
    info.setLoadingMessage("Loading M.U.G.E.N");
    Context menuLoader;
    Loader::loadScreen(menuLoader, info);

    menuLoader.failure();
    menuLoader.getMenu().run();
}

void run(){
    Searcher searcher;
    searcher.start();
    try{
        while (true){
            try{
                runMenu();
            } catch (const Mugen::StartGame & game){
                try{
                    Game versus(game.player, game.game, Data::getInstance().getFileFromMotif(Data::getInstance().getMotif()));
                    versus.run(searcher);
                } catch (const Exception::Return & re){
                    /* */
                }
            }
        }
    } catch (const Exception::Return & re){
        // Say what?
        // Do not quit game
        // Make waffles?
    } catch (const MugenException & ex){
        string m("Problem with loading MUGEN: ");
        m += ex.getFullReason();
        throw LoadException(__FILE__, __LINE__, m);
    }
}

}

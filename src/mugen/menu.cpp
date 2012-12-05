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

#include "globals.h"
#include "factory/font_render.h"
#include "parse-cache.h"

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

static Filesystem::AbsolutePath findSound(const Filesystem::RelativePath & music){
    /* First search by prepending sound to the path */
    Filesystem::AbsolutePath path = Mugen::Data::getInstance().getMotifDirectory().getDirectory().join(Filesystem::RelativePath("sound")).join(music);
    if (Storage::instance().exists(path)){
        return path;
    } else {
        path = Mugen::Data::getInstance().getMotifDirectory().getDirectory().join(music);
        if (Storage::instance().exists(path)){
            return path;
        } else {
            throw MugenException("Cannot find music", __FILE__, __LINE__);
        }
    }
}

static void changeMusic(const Filesystem::RelativePath & music){
    try {
        Music::loadSong(findSound(Filesystem::RelativePath(music)).path());
        Music::pause();
        Music::play();
    } catch (const MugenException & ex){
    } catch (const Filesystem::NotFound & fail){
        Global::debug(0) << "Could not load music: " << fail.getTrace() << endl;
    }
}

namespace Mugen{

class Item : public ListItem{
public:
    class TypeException : public std::exception{
    public:
        TypeException(const std::string & name, const Mugen::GameType & type):
        name(name),
        type(type){
        }
        ~TypeException() throw(){
        }
        const std::string & getName() const{
            return name;
        }
        const Mugen::GameType & getType() const{
            return type;
        }
    private:
        std::string name;
        Mugen::GameType type;
    };
    Item(const std::string & name, const Mugen::GameType & type):
    name(name),
    type(type){
    }
    
    bool next(){
        return false;
    }

    bool previous(){
        return false;
    }
    
    bool isRunnable() const{
        return true;
    }

    void run(){
        throw TypeException(name, type);
    }
    
    void render(int x, int y, const Graphics::Bitmap & work, const FontSystem::Font & font, int left, int right) const{
        font.draw(x, y, 0, name, work);
    }
    
    int getWidth(const FontSystem::Font & font) const {
        return (font.getWidth(name));
    }
    
    std::string name;
    Mugen::GameType type;
};

Menu::Menu(const Filesystem::AbsolutePath & path, Searcher & searcher):
list(PaintownUtil::ReferenceCount<ScrollAction>(new ScrollAction())),
fadeEnabled(true),
state(Intros),
ticks(0),
demoEnabled(false),
startDemoTime(0),
demoCycles(0),
nextIntroCycle(0),
searcher(searcher),
done(false){
    Filesystem::AbsolutePath baseDir = path.getDirectory();
    const Filesystem::AbsolutePath ourDefFile = Mugen::Util::fixFileName(baseDir, path.getFilename().path());
    
    Global::debug(1) << baseDir.path() << endl;

    if (ourDefFile.isEmpty()){
        throw MugenException( "Cannot locate menu definition file for: " + path.path(), __FILE__, __LINE__);
    }

    try{
        TimeDifference diff;
        diff.startTime();
        AstRef parsed(Mugen::Util::parseDef(ourDefFile));
        diff.endTime();
        Global::debug(1) << "Parsed mugen file " + ourDefFile.path() + " in" + diff.printTime("") << endl;
        
        for (Ast::AstParse::section_iterator section_it = parsed->getSections()->begin(); section_it != parsed->getSections()->end(); section_it++){
            Ast::Section * section = *section_it;
            std::string head = section->getName();
            head = Mugen::Util::fixCase(head);
            if (head == "info"){
                class InfoWalker: public Ast::Walker{
                public:
                    InfoWalker(Menu & menu):
                        menu(menu){
                        }

                    Menu & menu;

                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "name"){
                            try{
                                menu.name = simple.valueAsString();
                                Global::debug(1) << "Read name '" << menu.name << "'" << endl;
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
                            Global::debug(1) << "Unhandled option in Info Section: " << simple.toString() << __FILE__ << __LINE__ << endl;
                        }
                    }
                };

                InfoWalker walker(*this);
                section->walk(walker);
            } else if (head == "files"){
                class FileWalker: public Ast::Walker{
                public:
                    FileWalker(Menu & menu, const Filesystem::AbsolutePath & baseDir):
                        menu(menu),
                        baseDir(baseDir){
                        }

                    Menu & menu;
                    const Filesystem::AbsolutePath & baseDir;

                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "spr"){
                            std::string spriteFile;
                            simple.view() >> spriteFile;
                            Global::debug(1) << "Got Sprite File: '" << spriteFile << "'" << endl;
                            Mugen::Util::readSprites(Mugen::Util::findFile(Filesystem::RelativePath(spriteFile)), Filesystem::AbsolutePath(), menu.sprites, false);
                        } else if (simple == "snd"){
                            std::string soundFile;
                            simple.view() >> soundFile;
                            menu.sounds.init(soundFile);
                        } else if (simple == "logo.storyboard"){
                            try{
                                std::string logoFile;
                                simple.view() >> logoFile;
                                if (!logoFile.empty()){
                                    try{
                                        Global::debug(1) << "Logo file " << baseDir.path() << "/" << logoFile << endl;
                                        menu.logo = PaintownUtil::ReferenceCount<Mugen::Storyboard>(new Mugen::Storyboard(Mugen::Util::findFile(Filesystem::RelativePath(logoFile)), false));
                                        Global::debug(1) << "Got Logo Storyboard File: '" << logoFile << "'" << endl;
                                    } catch (const MugenException &ex){
                                        Global::debug(0) << "Error loading logo storyboard " << logoFile << ": " << ex.getFullReason() << std::endl;
                                    } catch (const Filesystem::NotFound & ex){
                                        Global::debug(0) << "Error loading logo storyboard " << logoFile << ": " << ex.getTrace() << std::endl;
                                    }
                                }
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "intro.storyboard"){
                            try{
                                std::string introFile;
                                simple.view() >> introFile;
                                if (!introFile.empty()){
                                    try{
                                        Global::debug(1) << "Intro file " << baseDir.path() << "/" << introFile << endl;
                                        menu.intro = PaintownUtil::ReferenceCount<Mugen::Storyboard>(new Mugen::Storyboard(Mugen::Util::findFile(Filesystem::RelativePath(introFile)), true));
                                        Global::debug(1) << "Got Intro Storyboard File: '" << introFile << "'" << endl;
                                    } catch (const MugenException &ex){
                                        Global::debug(0) << "Error loading intro storyboard " << introFile << ": " << ex.getFullReason() << std::endl;
                                    } catch (const Filesystem::NotFound & ex){
                                        Global::debug(0) << "Error loading intro storyboard " << introFile << ": " << ex.getTrace() << std::endl;
                                    }
                                }
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "select"){
                            //simple.view() >> menu.selectFile;
                            //Global::debug(1) << "Got Select File: '" << menu.selectFile << "'" << endl;
                        } else if (simple == "fight"){
                            //simple.view() >> menu.fightFile;
                            //Global::debug(1) << "Got Fight File: '" << menu.fightFile << "'" << endl;
                        } else if (PaintownUtil::matchRegex(simple.idString(), PaintownUtil::Regex("^font[0-9]*"))){
                            string temp;
                            simple.view() >> temp;
                            menu.fonts.add(temp);
                        } else {
                            Global::debug(1) << "Unhandled option in Files Section: " << simple.toString() << " at " << __FILE__ << ":" << __LINE__ << endl;
                        }
                    }
                };

                FileWalker walker(*this, baseDir);
                section->walk(walker);
            } else if (head == "music"){
                class MusicInfoWalker: public Ast::Walker{
                public:
                    MusicInfoWalker(Menu & menu):
                    menu(menu){
                    }
                    Menu & menu;
                    
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
                        } else if (simple == "intro.bgm"){
                            try{
                                simple.view() >> menu.introMusic;
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
                            Global::debug(1) << "Unhandled option in Music Section: " << simple.toString() << " at " << __FILE__ << ":" << __LINE__ << endl;
                        }
                    }
                };
                MusicInfoWalker walker(*this);
                section->walk(walker);
            } else if (head == "title info"){
                class TitleInfoWalker: public Ast::Walker{
                public:
                    TitleInfoWalker(Menu & menu):
                        menu(menu){
                        }

                    Menu & menu;

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
                            simple.view() >> menu.x >> menu.y;
                        } else if (simple == "menu.item.font"){
                            int index=0,bank=0,position=0;
                            try{
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                            }
                            menu.list->setListFont(menu.fonts.getFont(index, bank, position));
                        } else if (simple == "menu.item.active.font"){
                            int index=0,bank=0,position=0;
                            try{
                                simple.view() >> index >> bank >> position;
                            } catch (const Ast::Exception & e){
                            }
                            menu.list->setActiveFont(menu.fonts.getFont(index, bank, position));
                        } else if (simple == "menu.item.spacing"){
                            int x=0,y=0;
                            try{
                                simple.view() >> x >> y;
                            } catch (const Ast::Exception & e){
                            }
                            menu.list->setSpacing(x,y);
                        } else if (simple == "menu.itemname.arcade"){
                            try{
                                menu.listItems.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Item(simple.valueAsString(), Mugen::Arcade)));
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "menu.itemname.versus"){
                            try{
                                menu.listItems.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Item(simple.valueAsString(), Mugen::VersusSelect)));
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "menu.itemname.teamarcade"){
                            try{
                                menu.listItems.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Item(simple.valueAsString(), Mugen::Undefined)));
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "menu.itemname.teamversus"){
                            try{
                                menu.listItems.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Item(simple.valueAsString(), Mugen::Undefined)));
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "menu.itemname.teamcoop"){
                            try{
                                menu.listItems.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Item(simple.valueAsString(), Mugen::Undefined)));
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "menu.itemname.survival"){
                            try{
                                menu.listItems.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Item(simple.valueAsString(), Mugen::Survival)));
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "menu.itemname.survivalcoop"){
                            try{
                                menu.listItems.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Item(simple.valueAsString(), Mugen::Undefined)));
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "menu.itemname.training"){
                            try{
                                menu.listItems.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Item(simple.valueAsString(), Mugen::Training)));
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "menu.itemname.watch"){
                            try{
                                menu.listItems.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Item(simple.valueAsString(), Mugen::Watch)));
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "menu.itemname.options"){
                            try{
                                menu.listItems.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Item(simple.valueAsString(), Mugen::Options)));
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "menu.itemname.exit"){
                            try{
                                menu.listItems.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Item(simple.valueAsString(), Mugen::Quit)));
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "menu.window.margins.y"){
                            int x=0,y=0;
                            try{
                                simple.view() >> x >> y;
                                // Undocumented but it defaults to 5 if it's 0 or not specified
                                if (y == 0){
                                    y = 5;
                                }
                            } catch (const Ast::Exception & ex){
                            }
                            menu.list->setMargins(x,y);
                        } else if (simple == "menu.window.visibleitems"){
                            int items=0;
                            simple.view() >> items;
                            menu.list->setVisibleItems(items);
                        } else if (simple == "menu.boxcursor.visible"){
                            bool visible = false;
                            simple.view() >> visible;
                            menu.list->setShowCursor(visible);
                        } else if (simple == "menu.boxcursor.coords"){
                            int x1=0,y1=0,x2=0,y2=0;
                            try{
                                simple.view() >> x1 >> y1 >> x2 >> y2;
                            } catch (const Ast::Exception & e){
                            }
                            menu.list->setCursorCoords(x1,y1,x2,y2);
                        } else if (simple == "cursor.move.snd"){
                            int group=0, sound=0;
                            try{
                                simple.view() >> group >> sound;
                                menu.sounds.set(Menu::Move, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "cursor.done.snd"){
                            int group=0, sound=0;
                            try{
                                simple.view() >> group >> sound;
                                menu.sounds.set(Menu::Done, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else if (simple == "cancel.snd"){
                            int group=0, sound=0;
                            try{
                                simple.view() >> group >> sound;
                                menu.sounds.set(Menu::Cancel, group, sound);
                            } catch (const Ast::Exception & e){
                            }
                        } else {
                            Global::debug(1) << "Unhandled option in Title Info Section: " << simple.toString() << __FILE__ << __LINE__;
                        }
                    }
                };

                TitleInfoWalker walker(*this);
                section->walk(walker);
            } else if (PaintownUtil::matchRegex(head, PaintownUtil::Regex("^titlebgdef"))){
                background = PaintownUtil::ReferenceCount<Background>(new Background(ourDefFile, "titlebg"));
            } else if (head == "select info"){ 
                //selectInfoFile = ourDefFile;
            } else if (head == "selectbgdef" ){ /* Ignore for now */ }
            else if (head.find("selectbg") != std::string::npos ){ /* Ignore for now */ }
            else if (head == "vs screen" ){ /* Ignore for now */ }
            else if (head == "versusbgdef" ){ /* Ignore for now */ }
            else if (head.find("versusbg" ) != std::string::npos ){ /* Ignore for now */ }
            else if (head == "demo mode" ){
                class DemoWalker: public Ast::Walker{
                public:
                    DemoWalker(Menu & menu):
                    menu(menu){
                    }
                    Menu & menu;
                    
                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "enabled"){
                            try{
                                simple.view() >> menu.demoEnabled;
                            } catch (const Ast::Exception & fail){
                            }
                        } else if (simple == "title.waittime"){
                            try{
                                simple.view() >> menu.startDemoTime;
                            } catch (const Ast::Exception & fail){
                            }
                        } else if (simple == "intro.waitcycles"){
                            try{
                                simple.view() >> menu.nextIntroCycle;
                            } catch (const Ast::Exception & fail){
                            }
                        } else {
                            Global::debug(1) << "Unhandled option in Music Section: " << simple.toString() << " at " << __FILE__ << ":" << __LINE__ << endl;
                        }
                    }
                };
                DemoWalker walker(*this);
                section->walk(walker);
            } else if (head == "continue screen" ){ /* Ignore for now */ }
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
                Global::debug(1, context.str()) << "Unhandled Section: " << head << endl;
            }
        } 
    } catch (const Mugen::Def::ParseException & e){
        ostringstream out;
        out << "Error loading data " << ourDefFile.path() << ": " << e.getReason();
        throw MugenException(out.str(), __FILE__, __LINE__);
    } catch (const Exception::Base & fail){
        ostringstream out;
        out << "Error loading data " << ourDefFile.path() << ": " << fail.getTrace();
        throw MugenException(out.str(), __FILE__, __LINE__);
    }

    if (fonts.empty()){
        throw MugenException("No fonts specified", __FILE__, __LINE__);
    }
   
   list->setLocation(x, 0);
   list->addItems(listItems);
}

Menu::Menu(const Menu & copy, const std::vector< PaintownUtil::ReferenceCount<Gui::ScrollItem> > & items):
name(copy.name),
x(copy.x),
y(copy.y),
list(PaintownUtil::ReferenceCount<ScrollAction>(new ScrollAction())),
background(copy.background),
sounds(copy.sounds),
fadeEnabled(false),
state(MainMenu),
ticks(0),
demoEnabled(copy.demoEnabled),
startDemoTime(copy.startDemoTime),
demoCycles(copy.demoCycles),
nextIntroCycle(copy.nextIntroCycle),
searcher(copy.searcher),
done(false){
    list->setListFont(copy.list->getFont());
    list->setActiveFont(copy.list->getActiveFont());
    list->setSpacing(copy.list->getSpacingX(), copy.list->getSpacingY());
    list->setMargins(copy.list->getTopMargin(), copy.list->getBottomMargin());
    list->setVisibleItems(copy.list->getVisibleItems());
    list->setShowCursor(copy.list->getShowCursor());
    list->setCursorCoords(copy.list->getCursorCoordsX1(),copy.list->getCursorCoordsY1(),copy.list->getCursorCoordsX2(),copy.list->getCursorCoordsY2());
    list->setLocation(x, 0);
    list->addItems(items);
}

Menu::~Menu(){
}

void Menu::act(){
    switch (state){
        case Intros:{
            if (logo != NULL){
                logo->setInput(Mugen::getPlayer1Keys(20));
                logo->run(false);
            }

            // Intro run it no repeat
            if (intro != NULL){
                if (!introMusic.empty()){
                    changeMusic(Filesystem::RelativePath(introMusic));
                }
                intro->setInput(Mugen::getPlayer1Keys(20));
                intro->run(false);
            }
            state = MainMenu;
            break;
        }
        case MainMenu:{
            background->act();
            
            list->act();
            
            // Check demo
            if (demoEnabled){
                ticks++;
                if (ticks >= startDemoTime){
                    // Set state
                    state = DemoMode;
                    fader.setState(Gui::FadeTool::FadeOut);
                }
            }
            break;
        }
        case DemoMode:{
            if (fader.getState() == Gui::FadeTool::EndFade){
                try{
                    Game demo(Mugen::Player1, Mugen::Demo, Data::getInstance().getMotif());
                    demo.run(searcher);
                } catch (const Exception::Return & re){
                    /* */
                }
                demoCycles++;
                if (demoCycles > nextIntroCycle){
                    demoCycles = 0;
                    state = Intros;
                } else {
                    state = MainMenu;
                }
                ticks = 0;
                fader.setState(Gui::FadeTool::FadeIn);
            }
            break;
        }
        default:
            break;
    }
    
    fader.act();
}

void Menu::draw(const Graphics::Bitmap & work){
    Graphics::StretchedBitmap workArea(320, 240, work, Graphics::qualityFilterName(::Configuration::getQualityFilter()));
    workArea.start();
    
    // Backgrounds
    background->renderBackground(0, 0, workArea);
    
    // Top of the window
    const int top = y - list->getTopMargin();
    // Pretty accurate tested with 5 different screen packs and different settings
    const int bottom = y + ((list->getVisibleItems()-1) * list->getSpacingY()) + list->getBottomMargin();
    Graphics::Bitmap temp(workArea, 0, top, 320, bottom - top);
    list->render(temp, ::Font::getDefaultFont());
    
    // Foregrounds
    background->renderForeground(0, 0, workArea);
    
    // Fader
    if (fadeEnabled){
        fader.draw(workArea);
    }
    
    workArea.finish();
}

void Menu::up(){
    if (list->previous()){
        sounds.play(Move);
        ticks = 0;
    }
}

void Menu::down(){
    if (list->next()){
        sounds.play(Move);
        ticks = 0;
    }
}

void Menu::enter(){
    if (list->getCurrent()->isRunnable()){
        ticks = 0;
        sounds.play(Done);
        list->getCurrent()->run();
    }
}

void Menu::cancel(){
    if (!done){
        ticks = 0;
        sounds.play(Cancel);
        if (fadeEnabled){
            fader.setState(Gui::FadeTool::FadeOut);
        }
        // Just in case we were in preparation for demo mode
        state = MainMenu;
        done = true;
    }
}

bool Menu::isDone(){
    if (state == DemoMode){
        return false;
    } else {
        if (fadeEnabled){
            return (fader.getState() == Gui::FadeTool::EndFade);
        }
    }
    return done;
}

static void runNewMenu(Searcher & searcher){
    // Load er up and throw up a load box to inform the user
    class Context: public Loader::LoadingContext {
    public:
        Context(Searcher & searcher):
        searcher(searcher),
        exception(NULL){
        }
        
        Searcher & searcher;

        virtual void load(){
            try{
                menu = PaintownUtil::ReferenceCount<Menu>(new Menu(Mugen::Data::getInstance().getMotif(), searcher));
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

        PaintownUtil::ReferenceCount<Menu> getMenu(){
            return menu;
        }

        PaintownUtil::ReferenceCount<Menu> menu;
        Exception::Base * exception;
    };
    
    Loader::Info info;
    info.setLoadingMessage("Loading M.U.G.E.N");
    Context menuLoader(searcher);
    Loader::loadScreen(menuLoader, info);

    menuLoader.failure();
    
    class Draw: public PaintownUtil::Draw {
    public:
        Draw(PaintownUtil::ReferenceCount<Menu> menu):
        menu(menu){
        }

        PaintownUtil::ReferenceCount<Menu> menu;
        
        void draw(const Graphics::Bitmap & screen){
            menu->draw(screen);
            screen.BlitToScreen();
        }
    };
    
    // Box
    class Logic: public PaintownUtil::Logic {
    public:
        Logic(PaintownUtil::ReferenceCount<Menu> menu):
        menu(menu),
        escaped(false){
            player1Input = getPlayer1Keys(20);
            player2Input = getPlayer2Keys(20);
        }
        
        PaintownUtil::ReferenceCount<Menu> menu;

        InputMap<Keys> player1Input;
        InputMap<Keys> player2Input;

        bool escaped;
        
        double ticks(double system){
            return Util::gameTicks(system);
        }

        void run(){
            InputSource input1;
            InputSource input2;
            vector<InputMap<Mugen::Keys>::InputEvent> out1 = InputManager::getEvents(player1Input, input1);
            vector<InputMap<Mugen::Keys>::InputEvent> out2 = InputManager::getEvents(player2Input, input2);
            for (vector<InputMap<Mugen::Keys>::InputEvent>::iterator it = out1.begin(); it != out1.end(); it++){
                const InputMap<Mugen::Keys>::InputEvent & event = *it;
                if (!event.enabled){
                    continue;
                }


                if (event[Esc]){
                    if (!escaped){
                        escaped = true;
                        menu->cancel();
                        InputManager::waitForRelease(player1Input, input1, Esc);
                        InputManager::waitForRelease(player2Input, input2, Esc);
                    }
                }

                if (event[Up]){
                    menu->up();
                }
                if (event[Down]){
                    menu->down();
                }
                if (event[Start]){
                    try {
                        menu->enter();
                    } catch (const Item::TypeException & ex){
                        handleException(Mugen::Player1, ex);
                    }
                }
            }
            
            for (vector<InputMap<Mugen::Keys>::InputEvent>::iterator it = out2.begin(); it != out2.end(); it++){
                const InputMap<Mugen::Keys>::InputEvent & event = *it;
                if (!event.enabled){
                    continue;
                }


                if (event[Esc]){
                    if (!escaped){
                        escaped = true;
                        menu->cancel();
                        InputManager::waitForRelease(player1Input, input1, Esc);
                        InputManager::waitForRelease(player2Input, input2, Esc);
                    }
                }

                if (event[Up]){
                    menu->up();
                }
                if (event[Down]){
                    menu->down();
                }
                if (event[Start]){
                    try {
                        menu->enter();
                    } catch (const Item::TypeException & ex){
                        handleException(Mugen::Player2, ex);
                    }
                }
            }
            
            // Act out
            menu->act();
        }
        
        void handleException(const Mugen::PlayerType & player, const Item::TypeException & ex){
            // Handle option for player 1
            switch (ex.getType()){
                case Arcade:
                case Survival:
                case Training:
                case Watch:
                case Versus:
                case NetworkVersusServer:
                case NetworkVersusClient:
                    throw StartGame(player, ex.getType());
                    break;
                case VersusSelect:{
                    try {
                        std::vector< PaintownUtil::ReferenceCount<Gui::ScrollItem> > items;
                        items.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Item("PLAY LOCALLY", Mugen::Versus)));
                        /* FIXME: fix networking for mugen */
                        /*
                        items.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Item("CREATE SERVER", Mugen::NetworkVersusServer)));
                        items.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Item("CONNECT TO SERVER", Mugen::NetworkVersusClient)));
                        */
                        PaintownUtil::ReferenceCount<Menu> versus = PaintownUtil::ReferenceCount<Menu>(new Menu(*menu, items));
                        Logic logic(versus);
                        Draw draw(versus);
                        PaintownUtil::standardLoop(logic, draw);
                    } catch (const Item::TypeException & ex){
                        handleException(player, ex);
                    }
                    break;
                }
                case Options:{
                    try {
                        OptionsMenu::run(ex.getName());
                    } catch (const Exception::Return & ex){
                    }
                    break;
                }
                case Quit:
                    throw  Exception::Return(__FILE__, __LINE__);
                    break;
                case SurvivalCoop:
                case TeamArcade:
                case TeamVersus:
                case TeamCoop:
                case Undefined:
                default:
                    break;
            }
        }

        bool done(){
            return menu->isDone();
        }
    };
    
    Logic logic(menuLoader.getMenu());
    Draw draw(menuLoader.getMenu());
    PaintownUtil::standardLoop(logic, draw);
    throw  Exception::Return(__FILE__, __LINE__);
}

void run(){
    Searcher searcher;
    searcher.start();
    try{
        while (true){
            try{
                //runMenu();
                // Refactored menu
                runNewMenu(searcher);
            } catch (const Mugen::StartGame & game){
                try{
                    Game versus(game.player, game.game, Data::getInstance().getMotif());
                    versus.run(searcher);
                } catch (const Exception::Return & re){
                    /* */
                }
            } catch (const ReloadMugenException & ex){
                continue;
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

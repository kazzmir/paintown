#include "util/bitmap.h"
#include "util/stretch-bitmap.h"
#include "game.h"

#include <stdio.h>
#include <ostream>
#include <sstream>
#include "util/font.h"
#include "util/token.h"
#include "configuration.h"
#include "globals.h"
#include "util/init.h"
#include "util/input/keyboard.h"
#include "util/input/input-manager.h"
#include "util/input/input-source.h"
#include "util/input/input-map.h"
#include "util/funcs.h"
#include "util/system.h"
#include "util/timedifference.h"
#include "util/events.h"
#include "util/console.h"
#include "util/message-queue.h"
#include "util/gui/fadetool.h"
#include "factory/font_render.h"
#include "util/exceptions/shutdown_exception.h"
#include "util/exceptions/exception.h"
#include "util/network/network.h"
#include "search.h"
#include "util/loading.h"

#include "character.h"
#include "util.h"
#include "font.h"
#include "menu.h"
#include "stage.h"
#include "character-select.h"
#include "characterhud.h"
#include "storyboard.h"
#include "behavior.h"
#include "network-behavior.h"
#include "parse-cache.h"
#include "util/music.h"
#include "config.h"

#include "options.h"

#include "versus.h"

namespace PaintownUtil = ::Util;

using namespace Mugen;
using std::string;
using std::ostringstream;

static const int DEFAULT_WIDTH = 320;
static const int DEFAULT_HEIGHT = 240;

Game::Game(const PlayerType & playerType, const GameType & gameType, const Filesystem::AbsolutePath & systemFile):
playerType(playerType),
gameType(gameType),
systemFile(systemFile),
motifDirectory(systemFile.getDirectory()){
}

Game::~Game(){
}

void Game::run(Searcher & searcher){
    ParseCache cache;

    try{
        switch (gameType){
            default:
            case Arcade: {
                doArcade(searcher);
                break;
            }
            case Versus: {
                doVersus(searcher);
                break;
            }
            case NetworkVersusServer: {
                doNetworkVersus(true, searcher);
                break;
            }
            case NetworkVersusClient: {
                doNetworkVersus(false, searcher);
                break;
            }
            case TeamArcade: {
                //select.run("Team Arcade" , 1, true, &screen);
                startDemo(searcher);
                break;
            }
            case TeamVersus: {
                //gameInfo = select.run("Team Versus" , 2, true, &screen);
                break;
            }
            case TeamCoop: {
                //gameInfo = select.run("Team Cooperative" , 1, true, &screen);
                break;
            }
            case Survival: {
                doSurvival(searcher);
                break;
            }
            case SurvivalCoop: {
                //gameInfo = select.run("Survival Cooperative" , 1, true, &screen);
                break;
            }
            case Training: {
                doTraining(searcher);
                break;
            }
            case Watch: {
                doWatch(searcher);
                break;
            }
            case Demo: {
                startDemo(searcher);
                break;
            }
        }
    } catch (const MugenException e){
        std::ostringstream out;
        out << "Press ENTER to continue\n";
        out << "\n";
        out << "We are very sorry but an error has occured while trying to load MUGEN.";
        Graphics::Bitmap screen(*Graphics::screenParameter.current());
        PaintownUtil::showError(screen, e, out.str());
        InputManager::waitForKeys(Keyboard::Key_ENTER, Keyboard::Key_ESC, InputSource());
    }
}

class QuitGameException: public MugenException {
public:
    QuitGameException():
        MugenException("Quit game", __FILE__, __LINE__){
        }

    virtual ~QuitGameException() throw (){
    }
};

class RunMatchOptions{
public:
    RunMatchOptions():
    ticker(0),
    endTime(-1),
    demoMode(false),
    player1Behavior(NULL),
    player2Behavior(NULL){
        fader.setState(Gui::FadeTool::EndFade);
    }
    
    // For demo mode
    RunMatchOptions(int endTime):
    ticker(0),
    endTime(endTime),
    demoMode(endTime != -1),
    player1Behavior(NULL),
    player2Behavior(NULL){
        fader.setFadeInTime(1);
        fader.setFadeOutTime(60);
    }
    
    RunMatchOptions(const RunMatchOptions & copy):
    ticker(copy.ticker),
    endTime(copy.endTime),
    demoMode(copy.demoMode),
    player1Behavior(copy.player1Behavior),
    player2Behavior(copy.player2Behavior){
        fader.setFadeInTime(1);
        fader.setFadeOutTime(60);
    }
    
    const RunMatchOptions & operator=(const RunMatchOptions & copy){
        ticker = copy.ticker;
        endTime = copy.endTime;
        demoMode = copy.demoMode;
        player1Behavior = copy.player1Behavior;
        player2Behavior = copy.player2Behavior;
        fader.setFadeInTime(1);
        fader.setFadeOutTime(60);
        return *this;
    }
    
    void act(){
        if (demoMode){
            if (InputManager::anyInput() && fader.getState() != Gui::FadeTool::FadeOut){
                ticker = endTime;
                fader.setState(Gui::FadeTool::FadeOut);
            }
            if (ticker < endTime){
                ticker++;
            }
            if (ticker == endTime && fader.getState() != Gui::FadeTool::FadeOut){
                fader.setState(Gui::FadeTool::FadeOut);
            }
            fader.act();
            if (fader.getState() == Gui::FadeTool::EndFade){
                throw QuitGameException();
            }
        }
    }
    
    void draw(const Graphics::Bitmap & work){
        if (demoMode){
            fader.draw(work);
        }
    }
    
    bool isDemoMode() const{
        return demoMode;
    }
    
    void setBehavior(HumanBehavior * player1, HumanBehavior * player2){
        player1Behavior = player1;
        player2Behavior = player2;
    }
    
    HumanBehavior * getPlayer1Behavior() const {
        return player1Behavior;
    }
    
    HumanBehavior * getPlayer2Behavior() const {
        return player2Behavior;
    }
protected:
    int ticker;
    int endTime;
    bool demoMode;
    Gui::FadeTool fader;
    HumanBehavior * player1Behavior;
    HumanBehavior * player2Behavior;
};

enum MugenInput{
    SlowDown,
    SpeedUp,
    Pause,
    ForwardFrame,
    NormalSpeed,
    ToggleDebug,
    QuitGame,
    SetHealth,
    ShowFps,
    ToggleConsole,
};

class EscapeMenu{
public:
    class ResumeException : public std::exception{
    public:
        ResumeException(){
        }
        ~ResumeException() throw(){
        }
    };
    EscapeMenu(RunMatchOptions & options):
    enabled(false),
    options(options){
        class Resume : public BaseMenuItem {
        public:
            
            Resume(){
                optionName = "Resume";
                currentValue = "(Esc)";
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
                throw ResumeException();
            }
        };
        class Exit : public BaseMenuItem {
        public:
            Exit(PaintownUtil::ReferenceCount<OptionMenu> menu):
            menu(menu){
                optionName = "Exit Match";
                currentValue = "(Enter)";
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
                if (menu->confirmDialog("Are you sure you want to exit?", false, Graphics::makeColor(0,0,0), 128, true)){
                    throw QuitGameException();
                }
            }
            
            PaintownUtil::ReferenceCount<OptionMenu> menu;
        };
        std::vector< PaintownUtil::ReferenceCount<Gui::ScrollItem> > list;
    
        // Add empty list for now
        menu = PaintownUtil::ReferenceCount<OptionMenu>(new OptionMenu(list));
        
        // Create list
        list.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Resume()));
        if (options.getPlayer1Behavior() != NULL){
            list.push_back(OptionMenu::getPlayerKeys(0, "Player 1", menu));
        }
        if (options.getPlayer2Behavior() != NULL){
            list.push_back(OptionMenu::getPlayerKeys(1, "Player 2", menu));
        }
        list.push_back(PaintownUtil::ReferenceCount<Gui::ScrollItem>(new Exit(menu)));
        
        // Now update it
        menu->updateList(list);
        
        menu->setName("PAUSED");
        menu->setRenderBackground(false);
        menu->setClearAlpha(128);
    }
    ~EscapeMenu(){
    }
    void act(){
        menu->act();
    }
    
    void draw(const Graphics::Bitmap & work){
        if (enabled){
            menu->draw(work);
        }
    }
    
    void drawFirst(const Graphics::Bitmap & work){
        PaintownUtil::ReferenceCount<Graphics::Bitmap> copy = PaintownUtil::ReferenceCount<Graphics::Bitmap>(new Graphics::Bitmap(work, true));
        menu->updateScreenCapture(copy);
    }
    
    void toggle(){
        if (!options.isDemoMode()){
            enabled = !enabled;
            if (!enabled){
                menu->reset();
            }
        }
    }
    
    bool isActive() const {
        return enabled;
    }
    
    void up(){
        if (enabled){
            menu->up();
        }
    }
    void down(){
        if (enabled){
            menu->down();
        }
    }
    void left(){
        if (enabled){
            menu->left();
        }
    }
    void right(){
        if (enabled){
            menu->right();
        }
    }
    void enter(){
        if (enabled){
            try {
                menu->enter();
            } catch (const OptionMenu::KeysChangedException & ex){
                switch (ex.getType()){
                    case Player1:
                        if (options.getPlayer1Behavior() != NULL){
                            options.getPlayer1Behavior()->updateKeys(Mugen::getPlayer1Keys(), Mugen::getPlayer1InputLeft());
                        }
                        break;
                    case Player2:
                        if (options.getPlayer2Behavior() != NULL){
                            options.getPlayer2Behavior()->updateKeys(Mugen::getPlayer2Keys(), Mugen::getPlayer2InputLeft());
                        }
                        break;
                    default:
                        break;
                }
            }
        }
    }
    
protected:
    bool enabled;
    PaintownUtil::ReferenceCount<OptionMenu> menu;
    RunMatchOptions & options;
};

static void runEscape(EscapeMenu & menu){
    class Logic : public PaintownUtil::Logic{
    public:
        Logic(EscapeMenu & menu):
        isDone(false),
        menu(menu),
        player1Input(Mugen::getPlayer1Keys(20)),
        player2Input(Mugen::getPlayer2Keys(20)){
        }
        ~Logic(){
        }
        
        bool isDone;
        EscapeMenu & menu;
        InputMap<Keys> player1Input;
        InputMap<Keys> player2Input;
    
        double ticks(double system){
            return Mugen::Util::gameTicks(system);
        }

        void run(){
            InputSource input1;
            InputSource input2;
            std::vector<InputMap<Mugen::Keys>::InputEvent> out1 = InputManager::getEvents(player1Input, input1);
            std::vector<InputMap<Mugen::Keys>::InputEvent> out2 = InputManager::getEvents(player2Input, input2);
            out1.insert(out1.end(), out2.begin(), out2.end());
            for (std::vector<InputMap<Mugen::Keys>::InputEvent>::iterator it = out1.begin(); it != out1.end(); it++){
                const InputMap<Mugen::Keys>::InputEvent & event = *it;
                if (!event.enabled){
                    continue;
                }

                if (event[Esc]){
                    isDone = true;
                }

                if (event[Up]){
                    menu.up();
                }
                if (event[Down]){
                    menu.down();
                }
                if (event[Left]){
                    menu.left();
                }
                if (event[Right]){
                    menu.right();
                }
                if (event[Start]){
                    try {
                        menu.enter();
                    } catch (const EscapeMenu::ResumeException & ex){
                        isDone = true;
                    }
                }
            }
            
            // Act out
            menu.act();
        }

        bool done(){
            return isDone;
        }
    };
    
    class Draw : public PaintownUtil::Draw {
    public:
        Draw(EscapeMenu & menu):
        menu(menu){
        }

        EscapeMenu & menu;
        
        void drawFirst(const Graphics::Bitmap & screen){
            menu.drawFirst(screen);
        }
        
        void draw(const Graphics::Bitmap & screen){
            menu.draw(screen);
            screen.BlitToScreen();
        }
    };
    
    menu.toggle();
    Logic logic(menu);
    Draw draw(menu);
    PaintownUtil::standardLoop(logic, draw);
    menu.toggle();
}

class LogicDraw: public PaintownUtil::Logic, public PaintownUtil::Draw {
    public:
        LogicDraw(Mugen::Stage * stage, bool & show_fps, Console::Console & console, RunMatchOptions & options):
        endMatch(false),
        gameSpeed(Data::getInstance().getGameSpeed()),
        stage(stage),
        show_fps(show_fps),
        console(console),
        gameTicks(0),
        options(options),
        show(true),
        showGameSpeed(0),
        escapeMenu(options){
            gameInput.set(Keyboard::Key_F1, SlowDown);
            gameInput.set(Keyboard::Key_F2, SpeedUp);
            gameInput.set(Keyboard::Key_F3, NormalSpeed);
            gameInput.set(Keyboard::Key_F4, ToggleDebug);
            gameInput.set(Keyboard::Key_F6, Pause);
            gameInput.set(Keyboard::Key_F7, ForwardFrame);
            gameInput.set(Keyboard::Key_ESC, QuitGame);
            gameInput.set(::Configuration::config(0).getJoystickQuit(), QuitGame);
            gameInput.set(Keyboard::Key_F5, SetHealth);
            gameInput.set(Keyboard::Key_F9, ShowFps);
            gameInput.set(Keyboard::Key_TILDE, ToggleConsole);

            Global::registerInfo(&messages);
        }

        virtual ~LogicDraw(){
            Global::unregisterInfo(&messages);
        }

        InputMap<MugenInput> gameInput;
        bool endMatch;
        double gameSpeed;
        Mugen::Stage * stage;
        bool & show_fps;
        Console::Console & console;
        /* global info messages will appear in the console */
        MessageQueue messages;

        double gameTicks;
        RunMatchOptions & options;
        bool show;
        int showGameSpeed;
        
        EscapeMenu escapeMenu;

        void doInput(){
            class Handler: public InputHandler<MugenInput> {
                public:
                    Handler(LogicDraw & logic):
                        logic(logic){
                        }

                    LogicDraw & logic;

                    void release(const MugenInput & out, Keyboard::unicode_t code){
                    }

                    void press(const MugenInput & out, Keyboard::unicode_t code){
                        const int DISPLAY_GAME_SPEED_TIME = 100;
                        switch (out){
                            case SlowDown: {
                                logic.showGameSpeed = DISPLAY_GAME_SPEED_TIME;
                                logic.gameSpeed -= 0.1;
                                if (logic.gameSpeed < 0.1){
                                    logic.gameSpeed = 0.1;
                                }
                                break;
                            }
                            case SpeedUp: {
                                logic.showGameSpeed = DISPLAY_GAME_SPEED_TIME;
                                logic.gameSpeed += 0.1;
                                break;
                            }
                            case ForwardFrame: {
                                logic.showGameSpeed = DISPLAY_GAME_SPEED_TIME;
                                logic.gameSpeed = 0;
                                logic.gameTicks = 1;
                                break;
                            }
                            case Pause: {
                                logic.showGameSpeed = DISPLAY_GAME_SPEED_TIME;
                                logic.gameSpeed = 0;
                                break;
                            }
                            case NormalSpeed: {
                                logic.showGameSpeed = DISPLAY_GAME_SPEED_TIME;
                                logic.gameSpeed = 1;
                                break;
                            }
                            case ToggleDebug: {
                                logic.stage->toggleDebug(0);
                                break;
                            }
                            case QuitGame: {
                                if (!logic.options.isDemoMode()){
                                    runEscape(logic.escapeMenu);
                                }
                                break;
                            }
                            case SetHealth: {
                                logic.stage->setPlayerHealth(1);
                                break;
                            }
                            case ShowFps: {
                                logic.show_fps = ! logic.show_fps;
                                break;
                            }
                            case ToggleConsole: {
                                logic.console.toggle();
                                break;
                            }
                        }
                    }
            };

            Handler handler(*this);
            InputManager::handleEvents(gameInput, InputSource(), handler);
        }

        virtual void run(){
            gameTicks += gameSpeed;
            // Do stage logic catch match exception to handle the next match
            while (gameTicks > 0){
                gameTicks -= 1;
                stage->logic();
            }
            while (messages.hasAny()){
                console.addLine(messages.get());
            }
            console.act();
            endMatch = stage->isMatchOver();

            if (showGameSpeed > 0){
                showGameSpeed -= 1;
            }

            if (console.isActive()){
                try{
                    console.doInput();
                } catch (Exception::Return & r){
                    throw QuitGameException();
                }
            } else {
                doInput();
            }

            options.act();
        }

        virtual bool done(){
            return endMatch;
        }

        virtual double ticks(double system){
            return Mugen::Util::gameTicks(system);
        }

        virtual void draw(const Graphics::Bitmap & screen){
            if (show_fps){
                if (Global::second_counter % 2 == 0){
                    if (show){
                        Global::debug(0) << "FPS: " << getFps() << std::endl;
                        show = false;
                    }
                } else {
                    show = true;
                }
            }

            Graphics::StretchedBitmap work(DEFAULT_WIDTH, DEFAULT_HEIGHT, screen, Graphics::qualityFilterName(::Configuration::getQualityFilter()));
            work.start();
            stage->render(&work);
            
            options.draw(work);
            
            work.finish();
            FontRender * render = FontRender::getInstance();
            render->render(&screen);
            console.draw(screen);
            if (showGameSpeed > 0){
                const ::Font & font = ::Font::getDefaultFont(15, 15);
                font.printf(1, screen.getHeight() - font.getHeight() - 5, Graphics::makeColor(255, 255, 255), screen, "Game speed %f", 0, gameSpeed);
            }
            screen.BlitToScreen();
        }
};

static void runMatch(Mugen::Stage * stage, const std::string & musicOverride = "", RunMatchOptions options = RunMatchOptions()){
    //Music::changeSong();
    // *NOTE according to bgs.txt they belong in sound directory
    Filesystem::AbsolutePath file;
    try{
        if (musicOverride.empty()){
            Music::loadSong(Storage::instance().find(Filesystem::RelativePath(Mugen::Data::getInstance().getDirectory().path() + "/sound/" + stage->getMusic())).path());
        } else {
            Music::loadSong(Storage::instance().find(Filesystem::RelativePath(Mugen::Data::getInstance().getDirectory().path() + "/sound/" + musicOverride)).path());
        }
        Music::pause();
        Music::play();
    } catch (const Filesystem::NotFound & fail){
        Global::debug(0) << "Could not load music because " << fail.getTrace() << std::endl;
        Music::changeSong();
    }
    /* Ignore volume for now */
    //Music::setVolume(stage->getMusicVolume());
    /*
    Music::pause();
    Music::play();
    */

    Console::Console console(150);
    {
        class CommandQuit: public Console::Command {
        public:
            CommandQuit(){
            }

            string act(const string & line){
                throw ShutdownException();
            }

            string getDescription() const {
                return "quit - quit the game entirely";
            }
        };

        class CommandMemory: public Console::Command {
        public:
            CommandMemory(){
            }

            string getDescription() const {
                return "memory - current memory usage";
            }

            string act(const string & line){
                ostringstream out;
                out << "Memory usage: " << PaintownUtil::niceSize(System::memoryUsage()) << "\n";
                return out.str();
            }
        };

        class CommandHelp: public Console::Command {
        public:
            CommandHelp(const Console::Console & console):
            console(console){
            }

            const Console::Console & console;

            string getDescription() const {
                return "help - Show this help";
            }

            string act(const string & line){
                ostringstream out;
                const std::vector<PaintownUtil::ReferenceCount<Console::Command> > commands = console.getCommands();
                for (std::vector<PaintownUtil::ReferenceCount<Console::Command> >::const_iterator it = commands.begin(); it != commands.end(); it++){
                    PaintownUtil::ReferenceCount<Console::Command> command = *it;
                    out << command->getDescription() << "\n";
                }
                return out.str();
            }
        };

        class CommandDebug: public Console::Command {
        public:
            CommandDebug(Mugen::Stage * stage):
            stage(stage){
            }

            Mugen::Stage * stage;

            string getDescription() const {
                return "debug [#] - Enable/disable debug for player # starting from 0";
            }

            string act(const string & line){
                std::istringstream data(line);
                int number = -1;
                string ignore;
                data >> ignore >> number;
                stage->toggleDebug(number);
                std::ostringstream out;
                out << "Enabled debug for " << number;
                return out.str();
            }
        };

        class CommandChangeState: public Console::Command {
        public:
            CommandChangeState(Mugen::Stage * stage):
            stage(stage){
            }

            Mugen::Stage * stage;

            string getDescription() const {
                return "change-state [player #] [state #] - Change player to a state";
            }

            string act(const string & line){
                std::istringstream input(line);
                string command;
                int character = 0;
                int state = 0;
                input >> command >> character >> state;
                std::vector<Character*> players = stage->getPlayers();
                int count = 0;
                if (character < players.size()){
                    Character * player = players[character];
                    player->changeState(*stage, state);
                    std::ostringstream out;
                    out << "Changed state for " << player->getDisplayName() << " to " << state;
                    return out.str();
                }
                std::ostringstream out;
                out << "No such player " << character;
                return out.str();
            }
        };

        class CommandRecord: public Console::Command {
        public:
            CommandRecord(Mugen::Stage * stage):
            stage(stage){
            }

            Mugen::Stage * stage;

            string getDescription() const {
                return "record - Record moves to a file"; 
            }

            string act(const string & line){
                std::vector<Character*> players = stage->getPlayers();
                int count = 0;
                for (std::vector<Character*>::iterator it = players.begin(); it != players.end(); it++){
                    count += 1;
                    Character * player = *it;
                    player->startRecording(count);
                }
                return "Recording";
            }
        };

        console.addCommand("quit", PaintownUtil::ReferenceCount<Console::Command>(new CommandQuit()));
        console.addAlias("exit", "quit");
        console.addCommand("help", PaintownUtil::ReferenceCount<Console::Command>(new CommandHelp(console)));
        console.addCommand("memory", PaintownUtil::ReferenceCount<Console::Command>(new CommandMemory()));
        console.addCommand("record", PaintownUtil::ReferenceCount<Console::Command>(new CommandRecord(stage)));
        console.addCommand("debug", PaintownUtil::ReferenceCount<Console::Command>(new CommandDebug(stage)));
        console.addCommand("change-state", PaintownUtil::ReferenceCount<Console::Command>(new CommandChangeState(stage)));
    }

    bool show_fps = false;

    LogicDraw all(stage, show_fps, console, options);

    PaintownUtil::standardLoop(all, all);
}

/*
static Filesystem::AbsolutePath maybeFindRandom(const std::string & name, std::vector<Filesystem::AbsolutePath> & all){
    if (name == "_"){
        if (all.size() > 0){
            Filesystem::AbsolutePath front = all[0];
            all.erase(all.begin());
            return front;
        }
        throw MugenException("No def files found", __FILE__, __LINE__);
    } else {
        return Filesystem::findInsensitive(Filesystem::RelativePath("mugen/chars/" + name + "/" + name + ".def"));
    }
}
*/

static Character * doLoad(const Filesystem::AbsolutePath & path){
    TimeDifference timer;

    Filesystem::AbsolutePath use = path;
    if (path.getExtension() == "zip"){
        Storage::instance().addOverlay(path, path.getDirectory());
        use = Filesystem::AbsolutePath(Path::removeExtension(path.path()) + ".def");
    }

    try{
        Character * guy = new Character(use, Stage::Player1Side);
        Global::stream_type & out = Global::debug(0);
        out << "Loading player " << use.path();
        out.flush();
        timer.startTime();
        guy->load();
        timer.endTime();
        out << timer.printTime(" took") << std::endl;
        return guy;
    } catch (const MugenException & fail){
        Storage::instance().removeOverlay(path, path.getDirectory());
        throw;
    }

    return NULL;
}

static Character * makeCharacter(const std::string & name, bool random, std::vector<Filesystem::AbsolutePath> & all){
    if (random){
        while (all.size() > 0){
            int choice = PaintownUtil::rnd(all.size());
            try{
                return doLoad(all[choice]);
            } catch (const MugenException & failed){
                Global::debug(0) << "Failed to load " << all[choice].path() << " because " << failed.getReason() << std::endl;
                all.erase(all.begin() + choice);
            } catch (const Filesystem::NotFound & failed){
                Global::debug(0) << "Failed to load " << all[choice].path() << " because " << failed.getTrace() << std::endl;
                all.erase(all.begin() + choice);
            }
        }
        throw MugenException("No characters left to choose from!", __FILE__, __LINE__);
    } else {
        try{
            Filesystem::AbsolutePath zip = Storage::instance().find(Filesystem::RelativePath("mugen/chars/" + name + ".zip"));
            Storage::instance().addOverlay(zip, zip.getDirectory());
        } catch (const Filesystem::NotFound & fail){
        }
        Filesystem::AbsolutePath path = Storage::instance().findInsensitive(Filesystem::RelativePath("mugen/chars/" + name + "/" + name + ".def"));
        return doLoad(path);
    }
}

/*! Wrap characters for teams by type for use by futures and game modes */
class CharacterTeam{
public:
    CharacterTeam(const Mugen::ArcadeData::CharacterCollection & collection, const Stage::teams & side):
    collection(collection),
    side(side){
        loaded[0] = loaded[1] = loaded[2] = loaded[3] = false;
        switch (collection.getType()){
            case Mugen::ArcadeData::CharacterCollection::Turns4:
                fourth = PaintownUtil::ReferenceCount<Character>(new Character(collection.getFourth().getDef(), side));
            case Mugen::ArcadeData::CharacterCollection::Turns3:
                third = PaintownUtil::ReferenceCount<Character>(new Character(collection.getThird().getDef(), side));
            case Mugen::ArcadeData::CharacterCollection::Turns2:
            case Mugen::ArcadeData::CharacterCollection::Simultaneous:
                second = PaintownUtil::ReferenceCount<Character>(new Character(collection.getSecond().getDef(), side));
            default:
            case Mugen::ArcadeData::CharacterCollection::Single:
                first = PaintownUtil::ReferenceCount<Character>(new Character(collection.getFirst().getDef(), side));
                break;
        }
    }
    
    void load(){
        switch (collection.getType()){
            case Mugen::ArcadeData::CharacterCollection::Turns4:
                if (!loaded[3]){
                    fourth->load(collection.getFourth().getAct());
                    loaded[3] = true;
                }
            case Mugen::ArcadeData::CharacterCollection::Turns3:
                if (!loaded[2]){
                    third->load(collection.getThird().getAct());
                    loaded[2] = true;
                }
            case Mugen::ArcadeData::CharacterCollection::Turns2:
            case Mugen::ArcadeData::CharacterCollection::Simultaneous:
                if (!loaded[1]){
                    second->load(collection.getSecond().getAct());
                    loaded[1] = true;
                }
            default:
            case Mugen::ArcadeData::CharacterCollection::Single:
                if (!loaded[0]){
                    first->load(collection.getFirst().getAct());
                    loaded[0] = true;
                }
                break;
        }
    }
    
    Character & getFirst(){
        return *first;
    }
    
    Character & getSecond(){
        return *second;
    }
    
    Character & getThird(){
        return *third;
    }
    
    Character & getFourth(){
        return *fourth;
    }
    
protected:
    const Mugen::ArcadeData::CharacterCollection & collection;
    const Stage::teams & side;
    bool loaded[4];
    PaintownUtil::ReferenceCount<Character> first;
    PaintownUtil::ReferenceCount<Character> second;
    PaintownUtil::ReferenceCount<Character> third;
    PaintownUtil::ReferenceCount<Character> fourth;
};

class PlayerLoader: public PaintownUtil::Future<int> {
public:
    PlayerLoader(CharacterTeam & player1, CharacterTeam & player2):
        player1(player1),
        player2(player2){
            /* compute is a virtual function, is the virtual table set up
                * by the time start() tries to call it? or is that a race condition?
                */
            // start();
    }

    CharacterTeam & player1;
    CharacterTeam & player2;

    virtual void compute(){
        // Load player 1
        player1.load();
        // Load player 2
        player2.load();
        // NOTE is this needed anymore?
#ifdef WII
        /* FIXME: this is a hack, im not sure why its even required but fopen() will hang on sfp_lock_acquire
            * in another thread without locking and unlocking the sfp lock.
            * related things
            *  newlib/libc/stdio/fopen.c -- calls sfp_lock_acquire
            *  newlib/libc/sys/linux/sys/libc-lock.h - uses pthread's as the lock implementation
            */
        /*
        __sfp_lock_acquire();
        __sfp_lock_release();
        */
#endif
        set(0);
    }
};

static PaintownUtil::ReferenceCount<PlayerLoader> preLoadCharacters(CharacterTeam & player1, CharacterTeam & player2){
    
    PaintownUtil::ReferenceCount<PlayerLoader> playerLoader =  PaintownUtil::ReferenceCount<PlayerLoader>(new PlayerLoader(player1, player2));
    playerLoader->start();
    
    return playerLoader;
}

void prepareStage(PaintownUtil::ReferenceCount<PlayerLoader> playerLoader, Mugen::Stage & stage){
    try{
        Loader::Info info;
        Graphics::Bitmap background(*Graphics::screenParameter.current(), true);
        info.setBackground(&background);
        info.setLoadingMessage("Loading...");
        info.setPosition(25, 25);

        class Context: public Loader::LoadingContext {
        public:
            Context(PaintownUtil::ReferenceCount<PlayerLoader> playerLoader, Mugen::Stage & stage):
                playerLoader(playerLoader),
                stage(stage),
                fail(NULL){
                }

            virtual void maybeFail(){
                if (fail != NULL){
                    fail->throwSelf();
                }
            }

            virtual ~Context(){
                delete fail;
            }

            virtual void load(){
                try{
                    /* future */
                    int ok = playerLoader->get();

                    
                    /*! FIXME 
                    * this is only handling one on one matches, it will need to accomodate teams at a later point 
                    */
                    // Load stage
                    stage.addPlayer1(&playerLoader->player1.getFirst());
                    stage.addPlayer2(&playerLoader->player2.getFirst());
                    stage.load();
                } catch (const MugenException & fail){
                    this->fail = new MugenException(fail);
                } catch (const LoadException & fail){
                    this->fail = new LoadException(fail);
                } catch (const Filesystem::NotFound & fail){
                    this->fail = new Filesystem::NotFound(fail);
                }
            }

            PaintownUtil::ReferenceCount<PlayerLoader> playerLoader;
            Mugen::Stage & stage;
            Exception::Base * fail;
        };

        Context context(playerLoader, stage);
        /* FIXME: the wii has problems loading stuff in a background thread
         * while the load screen is going on.
         */
#ifdef WII
        context.load();
#else
        Loader::loadScreen(context, info);
#endif
        context.maybeFail();
    } catch (const MugenException & e){
        throw e;
    }
}

/* Does the setup work to start a game (arcade, watch, training, etc)
 */
class StartGameMode{
public:
    StartGameMode(const std::string & player1Name, const std::string & player2Name, const std::string & stageName){
        initialize(player1Name, player2Name, stageName);
    }

    virtual void initialize(const std::string & player1Name, const std::string & player2Name, const std::string & stageName){
        std::vector<Filesystem::AbsolutePath> allCharacters = Storage::instance().getFilesRecursive(Storage::instance().find(Filesystem::RelativePath("mugen/chars/")), "*.def");
        std::random_shuffle(allCharacters.begin(), allCharacters.end());
        bool random1 = player1Name == "_";
        bool random2 = player2Name == "_";

        player1 = makeCharacter(player1Name, random1, allCharacters);
        player2 = makeCharacter(player2Name, random2, allCharacters);

        stage = new Stage(Storage::instance().find(Filesystem::RelativePath("mugen/stages/" + stageName + ".def")));
        {
            TimeDifference timer;
            std::ostream & out = Global::debug(0);
            out << "Loading stage " << stageName;
            out.flush();
            timer.startTime();
            stage->load();
            timer.endTime();
            out << timer.printTime(" took") << std::endl;
        }
        stage->addPlayer1(player1.raw());
        stage->addPlayer2(player2.raw());
    }

    virtual void run() = 0;

    virtual ~StartGameMode(){
    }

    ParseCache cache;
    PaintownUtil::ReferenceCount<Character> player1;
    PaintownUtil::ReferenceCount<Character> player2;
    PaintownUtil::ReferenceCount<Stage> stage;
};

class StartArcade: public StartGameMode {
public:
    StartArcade(const std::string & player1Name,
                const std::string & player2Name,
                const std::string & stageName):
    StartGameMode(player1Name, player2Name, stageName){
    }

    virtual void run(){
        HumanBehavior player1Behavior(getPlayer1Keys(), getPlayer1InputLeft());
        LearningAIBehavior player2Behavior(30);
        /*
        player1->setRegeneration(true);
        player2->setRegeneration(true);
        */
        player1->setBehavior(&player1Behavior);
        player2->setBehavior(&player2Behavior);

        RunMatchOptions options;
        options.setBehavior(&player1Behavior, NULL);

        stage->reset();

        runMatch(stage.raw(), "", options);
    }
};

void Game::startArcade(const std::string & player1Name, const std::string & player2Name, const std::string & stageName){
    /* This has its own parse cache because its started by the main menu and not
     * by Game::run()
     */
    StartArcade arcade(player1Name, player2Name, stageName);
    arcade.run();
    /*
    ParseCache cache;
    std::vector<Filesystem::AbsolutePath> allCharacters = Storage::instance().getFilesRecursive(Storage::instance().find(Filesystem::RelativePath("mugen/chars/")), "*.def");
    std::random_shuffle(allCharacters.begin(), allCharacters.end());
    bool random1 = player1Name == "_";
    bool random2 = player2Name == "_";
    PaintownUtil::ReferenceCount<Character> player1;
    PaintownUtil::ReferenceCount<Character> player2;

    player1 = makeCharacter(player1Name, random1, allCharacters);
    player2 = makeCharacter(player2Name, random2, allCharacters);

    HumanBehavior player1Behavior(getPlayer1Keys(), getPlayer1InputLeft());
    LearningAIBehavior player2Behavior(30);
    // Set regenerative health
    player1->setRegeneration(true);
    player2->setRegeneration(true);
    player1->setBehavior(&player1Behavior);
    player2->setBehavior(&player2Behavior);
    
    RunMatchOptions options;
    options.setBehavior(&player1Behavior, NULL);

    Mugen::Stage stage(Storage::instance().find(Filesystem::RelativePath("mugen/stages/" + stageName + ".def")));
    {
        TimeDifference timer;
        std::ostream & out = Global::debug(0);
        out << "Loading stage " << stageName;
        out.flush();
        timer.startTime();
        stage.load();
        timer.endTime();
        out << timer.printTime(" took") << std::endl;
    }
    stage.addPlayer1(player1.raw());
    stage.addPlayer2(player2.raw());
    stage.reset();
    runMatch(&stage, "", options);
    */

}

void Game::startNetworkVersus(const string & player1Name, const string & player2Name, const string & stageName, bool server, int port){
    /* This has its own parse cache because its started by the main menu and not
     * by Game::run()
     */
    ParseCache cache;
    std::vector<Filesystem::AbsolutePath> allCharacters = Storage::instance().getFilesRecursive(Storage::instance().find(Filesystem::RelativePath("mugen/chars/")), "*.def");
    std::random_shuffle(allCharacters.begin(), allCharacters.end());
    bool random1 = player1Name == "_";
    bool random2 = player2Name == "_";
    PaintownUtil::ReferenceCount<Character> player1;
    PaintownUtil::ReferenceCount<Character> player2;

    player1 = makeCharacter(player1Name, random1, allCharacters);
    player2 = makeCharacter(player2Name, random2, allCharacters);

    Mugen::Stage stage(Storage::instance().find(Filesystem::RelativePath("mugen/stages/" + stageName + ".def")));
    {
        TimeDifference timer;
        std::ostream & out = Global::debug(0);
        out << "Loading stage " << stageName;
        out.flush();
        timer.startTime();
        stage.load();
        timer.endTime();
        out << timer.printTime(" took") << std::endl;
    }

    Network::Socket socket = 0;
#ifdef HAVE_NETWORKING
    if (server){
        Network::Socket remote = Network::open(port);
        Network::listen(remote);
        Global::debug(0) << "Waiting for a connection on port " << port << std::endl;
        socket = Network::accept(remote);
        Network::close(remote);
        Global::debug(0) << "Got a connection" << std::endl;
    } else {
        Global::debug(0) << "Connecting to 127.0.0.1 on port " << port << std::endl;
        socket = Network::connect("127.0.0.1", port); 
        Global::debug(0) << "Connected" << std::endl;
    }
#endif

    HumanBehavior local1Behavior(getPlayer1Keys(), getPlayer1InputLeft());
    NetworkLocalBehavior player1Behavior(&local1Behavior, socket);
    NetworkRemoteBehavior player2Behavior(socket);
    // Set regenerative health
    player1->setRegeneration(false);
    player2->setRegeneration(false);
    if (server){
        player1->setBehavior(&player1Behavior);
        player2->setBehavior(&player2Behavior);
    } else {
        player2->setBehavior(&player1Behavior);
        player1->setBehavior(&player2Behavior);
    }
    
    RunMatchOptions options;
    
    options.setBehavior(&local1Behavior, NULL);

    stage.addPlayer1(player1.raw());
    stage.addPlayer2(player2.raw());
    stage.reset();
    int time = Mugen::Data::getInstance().getTime();
    Mugen::Data::getInstance().setTime(-1);

#ifdef HAVE_NETWORKING
    if (server){
        int sync = Network::read16(socket);
        Network::send16(socket, sync);
    } else {
        Network::send16(socket, 0);
        Network::read16(socket);
    }

    if (!Network::blocking(socket, false)){
        Global::debug(0) << "Could not set socket to be non-blocking" << std::endl;
    }
#endif

    player1Behavior.begin();
    player2Behavior.begin();

    /*
    if (!Network::noDelay(socket, true)){
        Global::debug(0) << "Could not set no delay!" << std::endl;
    }
    */

    try{
        runMatch(&stage, "", options);
    } catch (const QuitGameException & ex){
    }
    Mugen::Data::getInstance().setTime(time);

#ifdef HAVE_NETWORKING
    Network::close(socket);
#endif
    
    throw QuitGameException();

}

void Game::startTraining(const std::string & player1Name, const std::string & player2Name, const std::string & stageName){
    /* This has its own parse cache because its started by the main menu and not
     * by Game::run()
     */
    ParseCache cache;
    std::vector<Filesystem::AbsolutePath> allCharacters = Storage::instance().getFilesRecursive(Storage::instance().find(Filesystem::RelativePath("mugen/chars/")), "*.def");
    std::random_shuffle(allCharacters.begin(), allCharacters.end());
    bool random1 = player1Name == "_";
    bool random2 = player2Name == "_";
    PaintownUtil::ReferenceCount<Character> player1;
    PaintownUtil::ReferenceCount<Character> player2;

    player1 = makeCharacter(player1Name, random1, allCharacters);
    player2 = makeCharacter(player2Name, random2, allCharacters);

    HumanBehavior player1Behavior(getPlayer1Keys(), getPlayer1InputLeft());
    DummyBehavior dummyBehavior;
    // Set regenerative health
    player1->setRegeneration(true);
    player2->setRegeneration(true);
    player1->setBehavior(&player1Behavior);
    player2->setBehavior(&dummyBehavior);
    
    RunMatchOptions options;
    
    options.setBehavior(&player1Behavior, NULL);

    Mugen::Stage stage(Storage::instance().find(Filesystem::RelativePath("mugen/stages/" + stageName + ".def")));
    {
        TimeDifference timer;
        std::ostream & out = Global::debug(0);
        out << "Loading stage " << stageName;
        out.flush();
        timer.startTime();
        stage.load();
        timer.endTime();
        out << timer.printTime(" took") << std::endl;
    }
    stage.addPlayer1(player1.raw());
    stage.addPlayer2(player2.raw());
    stage.reset();
    int time = Mugen::Data::getInstance().getTime();
    Mugen::Data::getInstance().setTime(-1);
    try{
        runMatch(&stage, "", options);
    } catch (const QuitGameException & ex){
    }
    Mugen::Data::getInstance().setTime(time);
    
    throw QuitGameException();
}

void Game::startWatch(const std::string & player1Name, const std::string & player2Name, const std::string & stageName){
    /* This has its own parse cache because its started by the main menu and not
     * by Game::run()
     */
    ParseCache cache;
    std::vector<Filesystem::AbsolutePath> allCharacters = Storage::instance().getFilesRecursive(Storage::instance().find(Filesystem::RelativePath("mugen/chars/")), "*.def");
    std::vector<Filesystem::AbsolutePath> zipCharacters = Storage::instance().getFilesRecursive(Storage::instance().find(Filesystem::RelativePath("mugen/chars/")), "*.zip");
    allCharacters.insert(allCharacters.end(), zipCharacters.begin(), zipCharacters.end());
    std::random_shuffle(allCharacters.begin(), allCharacters.end());
    bool random1 = player1Name == "_";
    bool random2 = player2Name == "_";
    PaintownUtil::ReferenceCount<Character> player1;
    PaintownUtil::ReferenceCount<Character> player2;

    player1 = makeCharacter(player1Name, random1, allCharacters);
    player2 = makeCharacter(player2Name, random2, allCharacters);

    LearningAIBehavior player1Behavior(30);
    LearningAIBehavior player2Behavior(30);

    // Set regenerative health
    player1->setBehavior(&player1Behavior);
    player2->setBehavior(&player2Behavior);

    Mugen::Stage stage(Storage::instance().find(Filesystem::RelativePath("mugen/stages/" + stageName + ".def")));
    {
        TimeDifference timer;
        std::ostream & out = Global::debug(0);
        out << "Loading stage " << stageName;
        out.flush();
        timer.startTime();
        stage.load();
        timer.endTime();
        out << timer.printTime(" took") << std::endl;
    }
    stage.addPlayer1(player1.raw());
    stage.addPlayer2(player2.raw());
    stage.reset();
    runMatch(&stage);
}

void Game::startScript(const std::string & player1Name, const string & player1Script, const std::string & player2Name, const string & player2Script, const std::string & stageName){
    /* This has its own parse cache because its started by the main menu and not
     * by Game::run()
     */
    ParseCache cache;
    std::vector<Filesystem::AbsolutePath> allCharacters = Storage::instance().getFilesRecursive(Storage::instance().find(Filesystem::RelativePath("mugen/chars/")), "*.def");
    std::random_shuffle(allCharacters.begin(), allCharacters.end());
    bool random1 = player1Name == "_";
    bool random2 = player2Name == "_";
    PaintownUtil::ReferenceCount<Character> player1;
    PaintownUtil::ReferenceCount<Character> player2;

    player1 = makeCharacter(player1Name, random1, allCharacters);
    player2 = makeCharacter(player2Name, random2, allCharacters);

    Filesystem::AbsolutePath player1Path(player1Script);
    Filesystem::AbsolutePath player2Path(player2Script);
    ScriptedBehavior player1Behavior(player1Path);
    ScriptedBehavior player2Behavior(player2Path);

    // Set regenerative health
    player1->setBehavior(&player1Behavior);
    player2->setBehavior(&player2Behavior);

    Mugen::Stage stage(Storage::instance().find(Filesystem::RelativePath("mugen/stages/" + stageName + ".def")));
    {
        TimeDifference timer;
        std::ostream & out = Global::debug(0);
        out << "Loading stage " << stageName;
        out.flush();
        timer.startTime();
        stage.load();
        timer.endTime();
        out << timer.printTime(" took") << std::endl;
    }
    stage.addPlayer1(player1.raw());
    stage.addPlayer2(player2.raw());
    stage.reset();
    runMatch(&stage);
}

void Game::doTraining(Searcher & searcher){
    int time = Mugen::Data::getInstance().getTime();
    Mugen::Data::getInstance().setTime(-1);
    while (true){
        Mugen::CharacterSelect select(systemFile);
        select.init();
        if (playerType == Mugen::Player1){
            select.setMode(Mugen::Training, Mugen::CharacterSelect::Player1);
        } else {
            select.setMode(Mugen::Training, Mugen::CharacterSelect::Player2);
        }
        InputMap<Mugen::Keys> keys1 = Mugen::getPlayer1Keys();
        InputMap<Mugen::Keys> keys2 = Mugen::getPlayer2Keys();
        HumanBehavior behavior = HumanBehavior(keys1, keys2);
        if (playerType == Player1){
            behavior = HumanBehavior(keys1, getPlayer1InputLeft());
        } else {
            behavior = HumanBehavior(keys2, getPlayer2InputLeft());
        }
        DummyBehavior dummyBehavior;
        PaintownUtil::ReferenceCount<PaintownUtil::Logic> logic = select.getLogic(keys1, keys2, searcher);
        PaintownUtil::ReferenceCount<PaintownUtil::Draw> draw = select.getDraw();
        PaintownUtil::standardLoop(*logic, *draw);
        
        if (select.wasCanceled()){
            Mugen::Data::getInstance().setTime(time);
            return;
        }
        
        Mugen::ArcadeData::CharacterCollection player1Collection(Mugen::ArcadeData::CharacterCollection::Single);
        Mugen::ArcadeData::CharacterCollection player2Collection(Mugen::ArcadeData::CharacterCollection::Single);
        
        if (playerType == Mugen::Player1){
            player1Collection = select.getPlayer1().getCollection();
            player2Collection = select.getPlayer1().getOpponentCollection();
        } else {
            player2Collection = select.getPlayer2().getCollection();
            player1Collection = select.getPlayer2().getOpponentCollection();
        }
        // Prepares futures
        CharacterTeam player1(player1Collection, Stage::Player1Side);
        CharacterTeam player2(player2Collection, Stage::Player2Side);
        PaintownUtil::ReferenceCount<PlayerLoader> loader = preLoadCharacters(player1, player2);
        
        {
            VersusMenu versus(systemFile);
            versus.init(player1Collection, player2Collection);
            PaintownUtil::ReferenceCount<PaintownUtil::Logic> logic = versus.getLogic(keys1, keys2);
            PaintownUtil::ReferenceCount<PaintownUtil::Draw> draw = versus.getDraw();
            PaintownUtil::standardLoop(*logic, *draw);
            if (versus.wasCanceled()){
                continue;
            }
        }
        
        RunMatchOptions options;
        
        if (playerType == Player1){
            player1.getFirst().setBehavior(&behavior);
            player2.getFirst().setBehavior(&dummyBehavior);
            options.setBehavior(&behavior, NULL);
        } else {
            player1.getFirst().setBehavior(&dummyBehavior);
            player2.getFirst().setBehavior(&behavior);
            options.setBehavior(NULL, &behavior);
        }
        player1.getFirst().setRegeneration(true);
        player2.getFirst().setRegeneration(true);
        Mugen::Stage stage(select.getStage());
        // Prepares stage
        prepareStage(loader, stage);
        stage.reset();
        try {
            runMatch(&stage, "", options);
        } catch (const Exception::Return & ex){
        } catch (const QuitGameException & ex){
        }
    }
    Mugen::Data::getInstance().setTime(time);
}

void Game::doWatch(Searcher & searcher){
    while (true){
        Mugen::CharacterSelect select(systemFile);
        select.init();
        if (playerType == Mugen::Player1){
            select.setMode(Mugen::Watch, Mugen::CharacterSelect::Player1);
        } else {
            select.setMode(Mugen::Watch, Mugen::CharacterSelect::Player2);
        }
        InputMap<Mugen::Keys> keys1 = Mugen::getPlayer1Keys();
        InputMap<Mugen::Keys> keys2 = Mugen::getPlayer2Keys();
        LearningAIBehavior player1AIBehavior(Mugen::Data::getInstance().getDifficulty());
        LearningAIBehavior player2AIBehavior(Mugen::Data::getInstance().getDifficulty());
        PaintownUtil::ReferenceCount<PaintownUtil::Logic> logic = select.getLogic(keys1, keys2, searcher);
        PaintownUtil::ReferenceCount<PaintownUtil::Draw> draw = select.getDraw();
        PaintownUtil::standardLoop(*logic, *draw);
        
        if (select.wasCanceled()){
            return;
        }
        //! FIXME Get first for now, later support team/turns
        Mugen::ArcadeData::CharacterCollection player1Collection(Mugen::ArcadeData::CharacterCollection::Single);
        Mugen::ArcadeData::CharacterCollection player2Collection(Mugen::ArcadeData::CharacterCollection::Single);
        if (playerType == Mugen::Player1){
            player1Collection = select.getPlayer1().getCollection();
            player2Collection = select.getPlayer1().getOpponentCollection();
        } else {
            player2Collection = select.getPlayer2().getCollection();
            player1Collection = select.getPlayer2().getOpponentCollection();
        }
        
        // Prepares futures
        CharacterTeam player1(player1Collection, Stage::Player1Side);
        CharacterTeam player2(player2Collection, Stage::Player2Side);
        PaintownUtil::ReferenceCount<PlayerLoader> loader = preLoadCharacters(player1, player2);
        
        {
            VersusMenu versus(systemFile);
            versus.init(player1Collection, player2Collection);
            PaintownUtil::ReferenceCount<PaintownUtil::Logic> logic = versus.getLogic(keys1, keys2);
            PaintownUtil::ReferenceCount<PaintownUtil::Draw> draw = versus.getDraw();
            PaintownUtil::standardLoop(*logic, *draw);
            if (versus.wasCanceled()){
                continue;
            }
        }
        
        player1.getFirst().setBehavior(&player1AIBehavior);
        player2.getFirst().setBehavior(&player2AIBehavior);
        Mugen::Stage stage(select.getStage());
        // Prepare stage
        prepareStage(loader, stage);
        stage.reset();
        try {
            runMatch(&stage);
        } catch (const Exception::Return & ex){
        } catch (const QuitGameException & ex){
        }
    }
}

void Game::doArcade(Searcher & searcher){
    
    InputMap<Mugen::Keys> keys1 = Mugen::getPlayer1Keys();
    InputMap<Mugen::Keys> keys2 = Mugen::getPlayer2Keys();
    InputMap<Mugen::Keys> playerKeys;
    HumanBehavior behavior(keys1, keys2);
    LearningAIBehavior AIBehavior(Mugen::Data::getInstance().getDifficulty());
    
    Mugen::ArcadeData::CharacterCollection player1Collection(Mugen::ArcadeData::CharacterCollection::Single);
    Mugen::ArcadeData::CharacterCollection player2Collection(Mugen::ArcadeData::CharacterCollection::Single);
    
    PaintownUtil::ReferenceCount<CharacterTeam> player1 = PaintownUtil::ReferenceCount<CharacterTeam>(NULL);
    PaintownUtil::ReferenceCount<CharacterTeam> player2 = PaintownUtil::ReferenceCount<CharacterTeam>(NULL);
    
    bool playerLoaded = false;
    
    // Match data
    Mugen::ArcadeData::MatchPath match;
    
    // Parsed
    AstRef parsed(Mugen::Util::parseDef(systemFile));
    
    RunMatchOptions options;
    
    // Scoped so it doesn't persist
    {
        Mugen::CharacterSelect select(systemFile);
        select.init();
        if (playerType == Mugen::Player1){
            select.setMode(Mugen::Arcade, Mugen::CharacterSelect::Player1);
        } else {
            select.setMode(Mugen::Arcade, Mugen::CharacterSelect::Player2);
        }
        PaintownUtil::ReferenceCount<PaintownUtil::Logic> logic = select.getLogic(keys1, keys2, searcher);
        PaintownUtil::ReferenceCount<PaintownUtil::Draw> draw = select.getDraw();
        PaintownUtil::standardLoop(*logic, *draw);
        
        if (select.wasCanceled()){
            return;
        }
    
        if (playerType == Mugen::Player1){
            player1Collection = select.getPlayer1().getCollection();
            playerKeys = keys1;
            behavior = HumanBehavior(getPlayer1Keys(), getPlayer1InputLeft());
            options.setBehavior(&behavior, NULL);
        } else {
            player2Collection = select.getPlayer2().getCollection();
            playerKeys = keys2;
            behavior = HumanBehavior(getPlayer2Keys(), getPlayer2InputLeft());
            options.setBehavior(NULL, &behavior);
        }
        
        // Match data
        match = select.getArcadePath();
    }
    
    Filesystem::AbsolutePath intro;
    Filesystem::AbsolutePath ending;
    bool displayWinScreen = false;
    bool continueScreenEnabled = false;
    bool defaultEndingEnabled = false;
    Filesystem::AbsolutePath defaultEnding;
    bool gameOverEnabled = false;
    Filesystem::AbsolutePath gameOver;
    bool creditsEnabled = false;
    Filesystem::AbsolutePath credits;

    // get intro and ending for player
    Filesystem::AbsolutePath file;
    if (playerType == Player1){
        file = player1Collection.getFirst().getDef();
    } else {
        file = player2Collection.getFirst().getDef();
    }
        
    AstRef playerParsed(Mugen::Util::parseDef(file));
    
    Filesystem::AbsolutePath baseDir = file.getDirectory();
    try{
        intro = Storage::instance().lookupInsensitive(baseDir, Filesystem::RelativePath(Util::probeDef(playerParsed, "arcade", "intro.storyboard")));
    } catch (const MugenException & fail){
        Global::debug(0) << "Failed to get intro from " << file.path() << " " << fail.getReason() << std::endl;
    } catch (const Filesystem::NotFound & fail){
        Global::debug(0) << "Failed to get intro from " << file.path() << " " << fail.getTrace() << std::endl;
    }

    try{
        ending = Storage::instance().lookupInsensitive(baseDir, Filesystem::RelativePath(Util::probeDef(playerParsed, "arcade", "ending.storyboard")));
    } catch (const MugenException & fail){
        Global::debug(0) << "Failed to get ending from " << file.path() << " " << fail.getReason() << std::endl;
    } catch (const Filesystem::NotFound & fail){
        Global::debug(0) << "Failed to get ending from " << file.path() << " " << fail.getTrace() << std::endl;
    }

    try{
        // Win screen if player has ending it will not show this
        if (Util::probeDef(parsed, "win screen", "enabled") == "1"){
            displayWinScreen = true;
        }
    } catch (const MugenException & fail){
        Global::debug(0) << "Failed to get win screen from " << systemFile.path() << " " << fail.getReason() << std::endl;
    } catch (const Filesystem::NotFound & fail){
        Global::debug(0) << "Failed to get win screen from " << systemFile.path() << " " << fail.getTrace() << std::endl;
    }

    try{
        // Get Default ending
        if (Util::probeDef(parsed, "default ending", "enabled") == "1"){
            defaultEndingEnabled = true;
            defaultEnding = Mugen::Data::getInstance().getFileFromMotif(Filesystem::RelativePath(Util::probeDef(parsed, "default ending", "storyboard")));
        }
    } catch (const MugenException & fail){
        Global::debug(0) << "Failed to get ending screen from " << systemFile.path() << " " << fail.getReason() << std::endl;
    } catch (const Filesystem::NotFound & fail){
        Global::debug(0) << "Failed to get ending screen from " << systemFile.path() << " " << fail.getTrace() << std::endl;
    }

    try{
        // Get Game Over
        if (Util::probeDef(parsed, "game over screen", "enabled") == "1"){
            gameOverEnabled = true;
            gameOver = Mugen::Data::getInstance().getFileFromMotif(Filesystem::RelativePath(Util::probeDef(parsed, "game over screen", "storyboard")));
        }
    } catch (const MugenException & fail){
        Global::debug(0) << "Failed to get game over screen from " << systemFile.path() << fail.getReason() << std::endl;
    } catch (const Filesystem::NotFound & fail){
        Global::debug(0) << "Failed to get game over screen from " << systemFile.path() << fail.getTrace() << std::endl;
    }

    try{
        // Get credits
        if (Util::probeDef(parsed, "end credits", "enabled") == "1"){
            gameOverEnabled = true;
            credits = Mugen::Data::getInstance().getFileFromMotif(Filesystem::RelativePath(Util::probeDef(parsed, "end credits", "storyboard")));
        }
    } catch (const MugenException & fail){
        Global::debug(0) << "Failed to get end credits from " << systemFile.path() << " " << fail.getReason() << std::endl;
    } catch (const Filesystem::NotFound & fail){
        Global::debug(0) << "Failed to get end credits from " << systemFile.path() << " " << fail.getTrace() << std::endl;
    }
    
    // Run intro before we begin game
    if (!intro.isEmpty()){
        try{
            Storyboard story(intro, true);
            story.setInput(playerKeys);
            story.run();
        } catch (...){
            Global::debug(0) << "Failed to load storyboard for some reason" << std::endl;
        }
    }
    
    bool quit = false;
    // Total wins from player
    int wins = 0;
    // Display game over storyboard
    bool displayGameOver = false;
    
    // Rematch?
    bool rematch = false;
    
    while (!quit){
        if (!rematch){
            if (playerType == Mugen::Player1){
                player2Collection = match.next();
            } else {
                player1Collection = match.next();
            }
        }
        
        PaintownUtil::ReferenceCount<CharacterTeam> ourPlayer = PaintownUtil::ReferenceCount<CharacterTeam>(NULL);
        Filesystem::AbsolutePath stagePath;
        Filesystem::AbsolutePath musicOverride;
        if (playerType == Mugen::Player1){
            if (!playerLoaded){
                player1 = PaintownUtil::ReferenceCount<CharacterTeam>(new CharacterTeam(player1Collection, Stage::Player1Side));
                playerLoaded = true;
            }
            if (!rematch){
                player2 = PaintownUtil::ReferenceCount<CharacterTeam>(new CharacterTeam(player2Collection, Stage::Player2Side));
            } else {
                player2->getFirst().resetPlayer();
                rematch = false;
            }
                
            ourPlayer = player1;
            player1->getFirst().setBehavior(&behavior);
            player2->getFirst().setBehavior(&AIBehavior);
            stagePath = player2Collection.getFirst().getStage();
        } else {
            if (!playerLoaded){
                player2 = PaintownUtil::ReferenceCount<CharacterTeam>(new CharacterTeam(player2Collection, Stage::Player2Side));
                playerLoaded = true;
            }
            if (!rematch){
                player1 = PaintownUtil::ReferenceCount<CharacterTeam>(new CharacterTeam(player1Collection, Stage::Player1Side));
            } else {
                player1->getFirst().resetPlayer();
                rematch = false;
            }
            
            ourPlayer = player2;
            player2->getFirst().setBehavior(&behavior);
            player1->getFirst().setBehavior(&AIBehavior);
            stagePath = player1Collection.getFirst().getStage();
        }
        
        PaintownUtil::ReferenceCount<PlayerLoader> loader = preLoadCharacters(*player1, *player2);
        
        {
            VersusMenu versus(systemFile);
            versus.init(player1Collection, player2Collection);
            PaintownUtil::ReferenceCount<PaintownUtil::Logic> logic = versus.getLogic(keys1, keys2);
            PaintownUtil::ReferenceCount<PaintownUtil::Draw> draw = versus.getDraw();
            PaintownUtil::standardLoop(*logic, *draw);
            if (versus.wasCanceled()){
                return;
            }
        }
        
        // FIXME use override music later
        Mugen::Stage stage(stagePath);
        prepareStage(loader, stage);
        stage.reset();
        try {
            runMatch(&stage, "", options);
            if (ourPlayer->getFirst().getMatchWins() > wins){
                wins = ourPlayer->getFirst().getMatchWins();
                // Reset player for next match
                ourPlayer->getFirst().resetPlayer();
                // There is a win they may proceed
                if (!match.hasMore()){
                    // Game is over and player has won display ending storyboard
                    if (displayWinScreen && ending.isEmpty()){
                        // Need to parse that and display it for now just ignore

                        // Show Default ending if enabled
                        if (defaultEndingEnabled){
                            if (!defaultEnding.isEmpty()){
                                Storyboard story(defaultEnding, true);
                                story.setInput(playerKeys);
                                story.run();
                            }
                        }
                    } else if (defaultEndingEnabled && ending.isEmpty()){
                        if (!defaultEnding.isEmpty()){
                            Storyboard story(defaultEnding, true);
                            story.setInput(playerKeys);
                            story.run();
                        }
                    } else if (!ending.isEmpty()){
                        Storyboard story(ending, true);
                        story.setInput(playerKeys);
                        story.run();
                    } 
                    if (creditsEnabled){                    
                        // credits
                        if (!credits.isEmpty()){
                            Storyboard story(defaultEnding, true);
                            story.setInput(playerKeys);
                            story.run();
                        }
                    }
                    quit = displayGameOver = true;
                }
            } else {
                // Player lost do continue screen if enabled for now just quit
                if (stage.doContinue(playerType, playerKeys)){
                    Mugen::CharacterSelect select(systemFile);
                    select.init();
                    if (playerType == Mugen::Player1){
                        select.setMode(Mugen::Arcade, Mugen::CharacterSelect::Player1);
                    } else {
                        select.setMode(Mugen::Arcade, Mugen::CharacterSelect::Player2);
                    }
                    PaintownUtil::ReferenceCount<PaintownUtil::Logic> logic = select.getLogic(keys1, keys2, searcher);
                    PaintownUtil::ReferenceCount<PaintownUtil::Draw> draw = select.getDraw();
                    PaintownUtil::standardLoop(*logic, *draw);
                    
                    if (select.wasCanceled()){
                        return;
                    }
                    
                    if (playerType == Mugen::Player1){
                        player1Collection = select.getPlayer1().getCollection();
                        playerLoaded = false;
                        rematch = true;
                        wins = 0;
                    } else {
                        player2Collection = select.getPlayer2().getCollection();
                        playerLoaded = false;
                        rematch = true;
                        wins = 0;
                    }
                } else {
                    quit = displayGameOver = true;
                }
            }
        } catch (const Exception::Return & ex){
            return;
        } catch (const QuitGameException & ex){
            return;
        }
    }
    
    // Show game over if ended through game otherwise just get out
    if (displayGameOver){
        if (!gameOver.isEmpty()){
            Storyboard story(gameOver, true);
            story.setInput(playerKeys);
            story.run();
        }
    }
}

void Game::doVersus(Searcher & searcher){
    while (true){
        Mugen::CharacterSelect select(systemFile);
        select.init();
        select.setMode(Mugen::Versus, Mugen::CharacterSelect::Both);
        InputMap<Mugen::Keys> keys1 = Mugen::getPlayer1Keys();
        InputMap<Mugen::Keys> keys2 = Mugen::getPlayer2Keys();
        HumanBehavior behavior1 = HumanBehavior(keys1, getPlayer1InputLeft());
        HumanBehavior behavior2 = HumanBehavior(keys2, getPlayer2InputLeft());
        PaintownUtil::ReferenceCount<PaintownUtil::Logic> logic = select.getLogic(keys1, keys2, searcher);
        PaintownUtil::ReferenceCount<PaintownUtil::Draw> draw = select.getDraw();
        PaintownUtil::standardLoop(*logic, *draw);
        
        RunMatchOptions options;
        options.setBehavior(&behavior1, &behavior2);
        
        if (select.wasCanceled()){
            return;
        }
        Mugen::ArcadeData::CharacterCollection player1Collection(Mugen::ArcadeData::CharacterCollection::Single);
        Mugen::ArcadeData::CharacterCollection player2Collection(Mugen::ArcadeData::CharacterCollection::Single);
        player1Collection = select.getPlayer1().getCollection();
        player2Collection = select.getPlayer2().getCollection();
        
        // Prepares futures
        CharacterTeam player1(player1Collection, Stage::Player1Side);
        CharacterTeam player2(player2Collection, Stage::Player2Side);
        PaintownUtil::ReferenceCount<PlayerLoader> loader = preLoadCharacters(player1, player2);
        
        {
            VersusMenu versus(systemFile);
            versus.init(player1Collection, player2Collection);
            PaintownUtil::ReferenceCount<PaintownUtil::Logic> logic = versus.getLogic(keys1, keys2);
            PaintownUtil::ReferenceCount<PaintownUtil::Draw> draw = versus.getDraw();
            PaintownUtil::standardLoop(*logic, *draw);
            if (versus.wasCanceled()){
                continue;
            }
        }
        
        player1.getFirst().setBehavior(&behavior1);
        player2.getFirst().setBehavior(&behavior2);
        Mugen::Stage stage(select.getStage());
        prepareStage(loader, stage);
        stage.reset();
        try {
            runMatch(&stage, "", options);
        } catch (const Exception::Return & ex){
        } catch (const QuitGameException & ex){
        }
    }
}

static int getNumber(const std::string & number){
    return atoi(number.c_str());
}

void Game::doNetworkVersus(bool isServer, Searcher & searcher){
#ifdef HAVE_NETWORKING
    // Keys
    InputMap<Mugen::Keys> keys1 = Mugen::getPlayer1Keys();
    InputMap<Mugen::Keys> keys2 = Mugen::getPlayer2Keys();
    // Local behaviors
    HumanBehavior behavior1 = HumanBehavior(keys1, getPlayer1InputLeft());
    PaintownUtil::ReferenceCount<NetworkLocalBehavior> player1LocalBehavior, player2LocalBehavior;
    PaintownUtil::ReferenceCount<NetworkRemoteBehavior> player1RemoteBehavior, player2RemoteBehavior;
    
    try {
        /*! TODO Network Local Behavior
            create or bind to socket */
        int port = 8473;
        std::string host = "localhost";
        Network::Socket socket;
        if (isServer){
            port = getNumber(OptionMenu::doInputDialog("Enter Port", "8473", true, Graphics::makeColor(0,0,0), 128, true));
            Network::Socket remote = Network::open(port);
            Network::listen(remote);
            Global::debug(0) << "Waiting for a connection on port " << port << std::endl;
            socket = Network::accept(remote);
            Network::close(remote);
            Global::debug(0) << "Got a connection" << std::endl;
            
            player1LocalBehavior = PaintownUtil::ReferenceCount<NetworkLocalBehavior>(new NetworkLocalBehavior(&behavior1, socket));
            player2RemoteBehavior = PaintownUtil::ReferenceCount<NetworkRemoteBehavior>(new NetworkRemoteBehavior(socket));
        } else {
            std::vector< PaintownUtil::ReferenceCount<Gui::ScrollItem> > list;
            OptionMenu options(list);
            port = getNumber(OptionMenu::doInputDialog("Enter Port", "8473", true, Graphics::makeColor(0,0,0), 128, true, &options));
            host = OptionMenu::doInputDialog("Enter Hostname", "000.000.000.000", true, Graphics::makeColor(0,0,0), 128, true, &options);
            Global::debug(0) << "Connecting to " << host << " on port " << port << std::endl;
            socket = Network::connect(host, port); 
            Global::debug(0) << "Connected" << std::endl;
            player2LocalBehavior = PaintownUtil::ReferenceCount<NetworkLocalBehavior>(new NetworkLocalBehavior(&behavior1, socket));
            player1RemoteBehavior = PaintownUtil::ReferenceCount<NetworkRemoteBehavior>(new NetworkRemoteBehavior(socket));
        }
    
        while (true){
            // Discuss before fight
            
            Mugen::CharacterSelect select(systemFile);
            select.init();
            // FIXME Make characterselect network useable
            select.setMode(Mugen::Arcade, Mugen::CharacterSelect::Player1);
            select.overrideTitle((isServer ? "(Server Mode) " : "(Client Mode - Host: " + host + ") ") + " Select Player");
            
            PaintownUtil::ReferenceCount<PaintownUtil::Logic> logic = select.getLogic(keys1, keys2, searcher);
            PaintownUtil::ReferenceCount<PaintownUtil::Draw> draw = select.getDraw();
            PaintownUtil::standardLoop(*logic, *draw);
            
            RunMatchOptions options;
            
            if (select.wasCanceled()){
                return;
            }

#ifdef HAVE_NETWORKING
            if (isServer){
                int sync = Network::read16(socket);
                Network::send16(socket, sync);
            } else {
                Network::send16(socket, 0);
                Network::read16(socket);
            }

            if (!Network::blocking(socket, false)){
                Global::debug(0) << "Could not set socket to be non-blocking" << std::endl;
            }
#endif

            Mugen::ArcadeData::CharacterCollection player1Collection(Mugen::ArcadeData::CharacterCollection::Single);
            Mugen::ArcadeData::CharacterCollection player2Collection(Mugen::ArcadeData::CharacterCollection::Single);
            if (isServer){
                // Get remote character
                const std::string & remotePath = Mugen::NetworkUtil::readMessage(socket);
                Mugen::ArcadeData::CharacterInfo remoteCharacter(Storage::instance().find(Filesystem::RelativePath(remotePath)));
                player2Collection.setFirst(remoteCharacter);
                options.setBehavior(&behavior1, NULL);
                // Send chosen character
                player1Collection = select.getPlayer1().getCollection();
                std::string localPath = Storage::instance().cleanse(player1Collection.getFirst().getDef()).path();
                Mugen::NetworkUtil::sendMessage(localPath, socket);
            } else {
                // Send chosen character
                player2Collection = select.getPlayer1().getCollection();
                std::string localPath = Storage::instance().cleanse(player1Collection.getFirst().getDef()).path();
                Mugen::NetworkUtil::sendMessage(localPath, socket);
                // Get remote character
                const std::string & remotePath = Mugen::NetworkUtil::readMessage(socket);
                Mugen::ArcadeData::CharacterInfo remoteCharacter(Storage::instance().find(Filesystem::RelativePath(remotePath)));
                player1Collection.setFirst(remoteCharacter);
                options.setBehavior(NULL, &behavior1);
            }
            
            // Prepares futures
            CharacterTeam player1(player1Collection, Stage::Player1Side);
            CharacterTeam player2(player2Collection, Stage::Player2Side);
            PaintownUtil::ReferenceCount<PlayerLoader> loader = preLoadCharacters(player1, player2);
            
            {
                VersusMenu versus(systemFile);
                versus.init(player1Collection, player2Collection);
                PaintownUtil::ReferenceCount<PaintownUtil::Logic> logic = versus.getLogic(keys1, keys2);
                PaintownUtil::ReferenceCount<PaintownUtil::Draw> draw = versus.getDraw();
                PaintownUtil::standardLoop(*logic, *draw);
                if (versus.wasCanceled()){
                    continue;
                }
            }
            if (isServer){
                player1.getFirst().setBehavior(player1LocalBehavior.raw());
                player2.getFirst().setBehavior(player2RemoteBehavior.raw());
            } else {
                player1.getFirst().setBehavior(player1RemoteBehavior.raw());
                player2.getFirst().setBehavior(player2LocalBehavior.raw());
            }
            Mugen::Stage stage(select.getStage());
            prepareStage(loader, stage);
            stage.reset();
            
            if (isServer){
                player1LocalBehavior->begin();
                player2RemoteBehavior->begin();
            } else {
                player2LocalBehavior->begin();
                player1RemoteBehavior->begin();
            }
            try {
                runMatch(&stage, "", options);
            } catch (const Exception::Return & ex){
            } catch (const QuitGameException & ex){
            }
        }
    } catch (const Network::NetworkException & ex){
        Global::debug(0) << "Problem with network connection! Reason: " << ex.getMessage();
    }
#endif
}

void Game::startDemo(Searcher & searcher){
    
    // Display select screen?
    bool showSelectScreen = false;
    bool showVersusScreen = false;
    int endTime = 1500;
    bool displayFightBars = false;
    AstRef parsed(Mugen::Util::parseDef(systemFile));
    if (Util::probeDef(parsed, "demo mode", "select.enabled") == "1"){
        showSelectScreen = true;
    }
    if (Util::probeDef(parsed, "demo mode", "vsscreen.enabled") == "1"){
        showVersusScreen = true;
    }
    {
        std::string temp = Util::probeDef(parsed, "demo mode", "fight.endtime");
        if (!temp.empty()){
            endTime = atoi(temp.c_str());
        }
    }
    if (Util::probeDef(parsed, "demo mode", "fight.bars.display") == "1"){
        showSelectScreen = true;
    }
    /*select.enabled = 1        ;Set to 1 to display select screen, 0 to disable
    vsscreen.enabled = 1      ;Set to 1 to display versus screen, 0 to disable
    title.waittime = 600      ;Time to wait at title before starting demo mode
    fight.endtime = 1500      ;Time to display the fight before returning to title
    fight.playbgm = 0         ;Set to 1 to enable in-fight BGM, 0 to disable
    fight.stopbgm = 1         ;Set to 1 to stop title BGM (only if playbgm = 0)
    fight.bars.display = 0    ;Set to 1 to display lifebar, 0 to disable
    intro.waitcycles = 1      ;Cycles to wait before intro cutscene is played again
    debuginfo = 0             ;Set to 0 to hide debugging info (debug mode only)*/

    
    Mugen::ArcadeData::CharacterCollection player1Collection(Mugen::ArcadeData::CharacterCollection::Single);
    Mugen::ArcadeData::CharacterCollection player2Collection(Mugen::ArcadeData::CharacterCollection::Single);
    Filesystem::AbsolutePath stagePath;
    
    InputMap<Mugen::Keys> keys1;
    InputMap<Mugen::Keys> keys2;
    LearningAIBehavior behavior1(Mugen::Data::getInstance().getDifficulty());
    LearningAIBehavior behavior2(Mugen::Data::getInstance().getDifficulty());
    
    // Display Select screen?
    if (showSelectScreen){
        // Clear it before continuing
        InputManager::waitForClear();
        Mugen::CharacterSelect select(systemFile);
        select.init();
        select.setMode(Mugen::Versus, Mugen::CharacterSelect::Demo);
        PaintownUtil::ReferenceCount<PaintownUtil::Logic> logic = select.getLogic(keys1, keys2, searcher);
        PaintownUtil::ReferenceCount<PaintownUtil::Draw> draw = select.getDraw();
        PaintownUtil::standardLoop(*logic, *draw);
        
        if (select.wasCanceled()){
            return;
        }
        player1Collection = select.getPlayer1().getCollection();
        player2Collection = select.getPlayer2().getCollection();
        stagePath = select.getStage();
    } else {
        class Collections{
        public:
            Collections(){
            }
            ~Collections(){
            }
            void addCharacter(const Filesystem::AbsolutePath & path){
                PaintownUtil::Thread::ScopedLock scoped(lock);
                try {
                    // Check for name of character ... if it doesn't find this it probably isn't a character
                    Util::probeDef(path, "info", "name");
                } catch (const MugenException & ex){
                    return;
                }
                allCharacters.push_back(path);
            }
            
            void addStage(const Filesystem::AbsolutePath & path){
                PaintownUtil::Thread::ScopedLock scoped(lock);
                try {
                    // Just like character check for name of stage ... if it doesn't find this it probably isn't a stage
                    Util::probeDef(path, "info", "name");
                } catch (const MugenException & ex){
                    return;
                }
                allStages.push_back(path);
            }
            const Filesystem::AbsolutePath getCharacter(){
                PaintownUtil::Thread::ScopedLock scoped(lock);
                return allCharacters[PaintownUtil::rnd(allCharacters.size())];
            }
            const Filesystem::AbsolutePath getStage(){
                PaintownUtil::Thread::ScopedLock scoped(lock);
                return allStages[PaintownUtil::rnd(allStages.size())];
            }
            
            bool isEmpty(){
                PaintownUtil::Thread::ScopedLock scoped(lock);
                return (allCharacters.empty() || allStages.empty());
            }
            
        private:
            std::vector<Filesystem::AbsolutePath> allCharacters;
            std::vector<Filesystem::AbsolutePath> allStages;
            PaintownUtil::Thread::LockObject lock;
        } collections;
        
        class Subscriber: public Searcher::Subscriber {
        public:
            Subscriber(Collections & collections):
            collections(collections){
            }
            
            Collections & collections;
        
            virtual ~Subscriber(){
            }
        
            virtual void receiveCharacters(const std::vector<Filesystem::AbsolutePath> & paths){
                for (std::vector<Filesystem::AbsolutePath>::const_iterator it = paths.begin(); it != paths.end(); it++){
                    const Filesystem::AbsolutePath & path = *it;
                    collections.addCharacter(path);
                }
            }

            virtual void receiveStages(const std::vector<Filesystem::AbsolutePath> & paths){
                for (std::vector<Filesystem::AbsolutePath>::const_iterator it = paths.begin(); it != paths.end(); it++){
                    const Filesystem::AbsolutePath & path = *it;
                    collections.addStage(path);
                }
            }
        } subscription(collections);
        
        class WithSubscription{
        public:
            WithSubscription(Searcher & search, Searcher::Subscriber & subscription):
            subscribeThread(PaintownUtil::Thread::uninitializedValue),
            search(search),
            subscription(subscription){
                if (!PaintownUtil::Thread::createThread(&subscribeThread, NULL, (PaintownUtil::Thread::ThreadFunction) subscribe, this)){
                    doSubscribe();
                }
            }

            PaintownUtil::Thread::Id subscribeThread;

            /* Start the subscription in a thread so that characters that are already found
            * will be added in a separate thread instead of the main one
            */
            static void * subscribe(void * me){
                WithSubscription * self = (WithSubscription*) me;
                self->doSubscribe();
                return NULL;
            }

            void doSubscribe(){
                // Only subscribe if auto search is enabled
                if (Data::getInstance().autoSearch()){
                    search.subscribe(&subscription);
                }
            }

            Searcher & search;
            Searcher::Subscriber & subscription;

            virtual ~WithSubscription(){
                /* Make sure we wait for the initial join to finish before trying to unsubscribe */
                PaintownUtil::Thread::joinThread(subscribeThread);
                search.unsubscribe(&subscription);
            }
        } withSubscription(searcher, subscription);
        
        Filesystem::AbsolutePath path1;
        Filesystem::AbsolutePath path2;
        
        int currentTime = System::currentSeconds();
        Global::debug(1) << "Waiting for search..." << std::endl;
        int i = 0;
        while (collections.isEmpty()){
            if (System::currentSeconds() >= (currentTime + 4)){
                Global::debug(1) << "Couldn't find characters or stages withnin a reasonable time, Aborting." << std::endl;
                return;
            }
            Global::debug(1) << "Time elapsed: " << i << std::endl;
        }
        
        path1 = collections.getCharacter();
        path2 = collections.getCharacter();
        
        Global::debug(1) << "Got player1: " << path1.path() << std::endl;
        Global::debug(1) << "Got player2: " << path2.path() << std::endl;
        
        player1Collection.setFirst(Mugen::ArcadeData::CharacterInfo(path1));
        player2Collection.setFirst(Mugen::ArcadeData::CharacterInfo(path2));
        
        stagePath = collections.getStage();
    }
    
    // Prepares futures
    CharacterTeam player1(player1Collection, Stage::Player1Side);
    CharacterTeam player2(player2Collection, Stage::Player2Side);
    PaintownUtil::ReferenceCount<PlayerLoader> loader = preLoadCharacters(player1, player2);
    
    if (showVersusScreen){
        InputManager::waitForClear();
        VersusMenu versus(systemFile, true);
        versus.init(player1Collection, player2Collection);
        PaintownUtil::ReferenceCount<PaintownUtil::Logic> logic = versus.getLogic(keys1, keys2);
        PaintownUtil::ReferenceCount<PaintownUtil::Draw> draw = versus.getDraw();
        PaintownUtil::standardLoop(*logic, *draw);
        if (versus.wasCanceled()){
            return;
        }
    }
    
    // Load it up
    player1.getFirst().setBehavior(&behavior1);
    player2.getFirst().setBehavior(&behavior2);
    Mugen::Stage stage(stagePath);
    prepareStage(loader, stage);
    stage.reset();
    try {
        InputManager::waitForClear();
        
        runMatch(&stage, "", RunMatchOptions(endTime));
    } catch (const Exception::Return & ex){
    } catch (const QuitGameException & ex){
    }
}

template<typename T>
static T getRandom(const std::vector<T> & vec){
    return vec[PaintownUtil::rnd(vec.size())];
}

void Game::doSurvival(Searcher & searcher){
    InputMap<Mugen::Keys> keys1 = Mugen::getPlayer1Keys();
    InputMap<Mugen::Keys> keys2 = Mugen::getPlayer2Keys();
    InputMap<Mugen::Keys> playerKeys;
    HumanBehavior behavior(keys1, keys2);
    LearningAIBehavior AIBehavior(Mugen::Data::getInstance().getDifficulty());
    
    Mugen::ArcadeData::CharacterCollection player1Collection(Mugen::ArcadeData::CharacterCollection::Single);
    Mugen::ArcadeData::CharacterCollection player2Collection(Mugen::ArcadeData::CharacterCollection::Single);
    
    PaintownUtil::ReferenceCount<CharacterTeam> player1 = PaintownUtil::ReferenceCount<CharacterTeam>(NULL);
    PaintownUtil::ReferenceCount<CharacterTeam> player2 = PaintownUtil::ReferenceCount<CharacterTeam>(NULL);
    
    std::vector<Mugen::ArcadeData::CharacterInfo> characters;
    
    Filesystem::AbsolutePath stagePath;
    
    RunMatchOptions options;
    
    // Scoped so it doesn't persist
    {
        Mugen::CharacterSelect select(systemFile);
        select.init();
        if (playerType == Mugen::Player1){
            select.setMode(Mugen::Survival, Mugen::CharacterSelect::Player1);
        } else {
            select.setMode(Mugen::Survival, Mugen::CharacterSelect::Player2);
        }
        PaintownUtil::ReferenceCount<PaintownUtil::Logic> logic = select.getLogic(keys1, keys2, searcher);
        PaintownUtil::ReferenceCount<PaintownUtil::Draw> draw = select.getDraw();
        PaintownUtil::standardLoop(*logic, *draw);
        
        if (select.wasCanceled()){
            return;
        }
    
        if (playerType == Mugen::Player1){
            player1Collection = select.getPlayer1().getCollection();
            playerKeys = keys1;
            behavior = HumanBehavior(getPlayer1Keys(), getPlayer1InputLeft());
            player2Collection = select.getPlayer1().getOpponentCollection();
            options.setBehavior(&behavior, NULL);
        } else {
            player2Collection = select.getPlayer2().getCollection();
            playerKeys = keys2;
            behavior = HumanBehavior(getPlayer2Keys(), getPlayer2InputLeft());
            player1Collection = select.getPlayer2().getOpponentCollection();
            options.setBehavior(NULL, &behavior);
        }
        
        characters = select.getCharacters();
        stagePath = select.getStage();
    }
    
    int wins = 0;
    bool playerLoaded = false;
    
    while (true){
        if (playerType == Mugen::Player1){
            switch (player2Collection.getType()){
                case Mugen::ArcadeData::CharacterCollection::Turns4:
                    player2Collection.setFourth(getRandom<Mugen::ArcadeData::CharacterInfo>(characters));
                case Mugen::ArcadeData::CharacterCollection::Turns3:
                    player2Collection.setThird(getRandom<Mugen::ArcadeData::CharacterInfo>(characters));
                case Mugen::ArcadeData::CharacterCollection::Turns2:
                case Mugen::ArcadeData::CharacterCollection::Simultaneous:
                    player2Collection.setSecond(getRandom<Mugen::ArcadeData::CharacterInfo>(characters));
                case Mugen::ArcadeData::CharacterCollection::Single:
                default:
                    player2Collection.setFirst(getRandom<Mugen::ArcadeData::CharacterInfo>(characters));
                    break;
            }
        } else {
            switch (player1Collection.getType()){
                case Mugen::ArcadeData::CharacterCollection::Turns4:
                    player1Collection.setFourth(getRandom<Mugen::ArcadeData::CharacterInfo>(characters));
                case Mugen::ArcadeData::CharacterCollection::Turns3:
                    player1Collection.setThird(getRandom<Mugen::ArcadeData::CharacterInfo>(characters));
                case Mugen::ArcadeData::CharacterCollection::Turns2:
                case Mugen::ArcadeData::CharacterCollection::Simultaneous:
                    player1Collection.setSecond(getRandom<Mugen::ArcadeData::CharacterInfo>(characters));
                case Mugen::ArcadeData::CharacterCollection::Single:
                default:
                    player1Collection.setFirst(getRandom<Mugen::ArcadeData::CharacterInfo>(characters));
                    break;
            }
        }
        
        PaintownUtil::ReferenceCount<CharacterTeam> ourPlayer = PaintownUtil::ReferenceCount<CharacterTeam>(NULL);
        if (playerType == Mugen::Player1){
            if (!playerLoaded){
                player1 = PaintownUtil::ReferenceCount<CharacterTeam>(new CharacterTeam(player1Collection, Stage::Player1Side));
                ourPlayer = player1;
                player1->getFirst().setBehavior(&behavior);
                playerLoaded = true;
                options.setBehavior(&behavior, NULL);
            }
            
            player2 = PaintownUtil::ReferenceCount<CharacterTeam>(new CharacterTeam(player2Collection, Stage::Player2Side));
            player2->getFirst().setBehavior(&AIBehavior);
        } else {
            if (!playerLoaded){
                player2 = PaintownUtil::ReferenceCount<CharacterTeam>(new CharacterTeam(player2Collection, Stage::Player2Side));
                ourPlayer = player2;
                player2->getFirst().setBehavior(&behavior);
                playerLoaded = true;
                options.setBehavior(NULL, &behavior);
            }
            
            player1 = PaintownUtil::ReferenceCount<CharacterTeam>(new CharacterTeam(player1Collection, Stage::Player1Side));
            player1->getFirst().setBehavior(&AIBehavior);
        }
        
        PaintownUtil::ReferenceCount<PlayerLoader> loader = preLoadCharacters(*player1, *player2);
        
        {
            VersusMenu versus(systemFile);
            versus.init(player1Collection, player2Collection);
            PaintownUtil::ReferenceCount<PaintownUtil::Logic> logic = versus.getLogic(keys1, keys2);
            PaintownUtil::ReferenceCount<PaintownUtil::Draw> draw = versus.getDraw();
            PaintownUtil::standardLoop(*logic, *draw);
            if (versus.wasCanceled()){
                return;
            }
        }
        
        // FIXME use override music later
        Mugen::Stage stage(stagePath);
        prepareStage(loader, stage);
        // FIXME Add mode in stage to keep from reverting the wins and allowing matches to last 1 round
        stage.getGameInfo()->setWins(1, 0);
        stage.reset();
        try {
            runMatch(&stage, "", options);
            if (ourPlayer->getFirst().getMatchWins()> wins){
                wins++;
                // Reset player for next match
                ourPlayer->getFirst().resetPlayer();
            } else {
                break;
            }
        } catch (const Exception::Return & ex){
            return;
        } catch (const QuitGameException & ex){
            return;
        }
    }
    // FIXME show total matches won (very similar to Stage::doContinue)
    Global::debug(0) << "Matches Survived: " << wins << std::endl;
}

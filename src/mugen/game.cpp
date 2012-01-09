#include "util/bitmap.h"
#include "util/stretch-bitmap.h"
#include "game.h"

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
#include "factory/font_render.h"
#include "util/exceptions/shutdown_exception.h"
#include "util/exceptions/exception.h"
#include "search.h"
#include "util/loading.h"

#include "character.h"
#include "util.h"
#include "font.h"
#include "menu.h"
#include "stage.h"
#include "character-select.h"
#include "storyboard.h"
#include "behavior.h"
#include "parse-cache.h"
#include "util/music.h"
#include "config.h"

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
            case TeamArcade: {
                //select.run("Team Arcade" , 1, true, &screen);
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
                //gameInfo = select.run("Survival" , 1, true, &screen);
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
        }
    } catch (const MugenException e){
        std::ostringstream out;
        out << "Press ENTER to continue\n";
        out << "\n";
        out << "We are very sorry but an error has occured while trying to load MUGEN.";
        Graphics::Bitmap screen(GFX_X, GFX_Y);
        PaintownUtil::showError(screen, e, out.str());
        InputManager::waitForKeys(Keyboard::Key_ENTER, Keyboard::Key_ESC, InputSource());
    }
}

class QuitGameException: public MugenException {
public:
    QuitGameException():
        MugenException("Quit game"){
        }

    virtual ~QuitGameException() throw (){
    }
};

enum MugenInput{
    SlowDown,
    SpeedUp,
    NormalSpeed,
    ToggleDebug,
    QuitGame,
    SetHealth,
    ShowFps,
    ToggleConsole
};

static void runMatch(Mugen::Stage * stage, const std::string & musicOverride = ""){
    //Music::changeSong();
    // *NOTE according to bgs.txt they belong in sound directory
    Filesystem::AbsolutePath file;
    try{
        if (musicOverride.empty()){
            Music::loadSong(Storage::instance().find(Filesystem::RelativePath(Mugen::Data::getInstance().getDirectory().path() + "/sound/" + stage->getMusic())).path());
        } else {
            Music::loadSong(Storage::instance().find(Filesystem::RelativePath(Mugen::Data::getInstance().getDirectory().path() + "/sound/" + musicOverride)).path());
        }
    } catch (const Filesystem::NotFound & fail){
        Global::debug(0) << "Could not load music because " << fail.getTrace() << std::endl;
    }
    /* Ignore volume for now */
    //Music::setVolume(stage->getMusicVolume());
    Music::pause();
    Music::play();

    Console::Console console(150);
    {
        class CommandQuit: public Console::Command {
        public:
            CommandQuit(){
            }

            string act(const string & line){
                throw ShutdownException();
            }
        };

        class CommandMemory: public Console::Command {
        public:
            CommandMemory(){
            }

            string act(const string & line){
                ostringstream out;
                out << "Memory usage: " << PaintownUtil::niceSize(System::memoryUsage()) << "\n";
                return out.str();
            }
        };

        class CommandHelp: public Console::Command {
        public:
            CommandHelp(){
            }

            string act(const string & line){
                ostringstream out;
                out << "quit - quit the game entirely" << "\n";
                out << "memory - current memory usage" << "\n";
                out << "record - record character commands" << "\n";
                out << "help - this help menu";
                return out.str();
            }
        };

        class CommandDebug: public Console::Command {
        public:
            CommandDebug(Mugen::Stage * stage):
            stage(stage){
            }

            Mugen::Stage * stage;

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

            string act(const string & line){
                std::istringstream input(line);
                string command;
                int character = 0;
                int state = 0;
                input >> command >> character >> state;
                std::vector<Object*> players = stage->getPlayers();
                int count = 0;
                if (character < players.size()){
                    std::vector<string> inputs;
                    Character * player = (Character*) players[character];
                    player->changeState(*stage, state, inputs);
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

            string act(const string & line){
                std::vector<Object*> players = stage->getPlayers();
                int count = 0;
                for (std::vector<Object*>::iterator it = players.begin(); it != players.end(); it++){
                    Character * player = (Character*) *it;
                    player->startRecording(count + 1);
                }
                return "Recording";
            }
        };

        console.addCommand("quit", PaintownUtil::ReferenceCount<Console::Command>(new CommandQuit()));
        console.addAlias("exit", "quit");
        console.addCommand("help", PaintownUtil::ReferenceCount<Console::Command>(new CommandHelp()));
        console.addCommand("memory", PaintownUtil::ReferenceCount<Console::Command>(new CommandMemory()));
        console.addCommand("record", PaintownUtil::ReferenceCount<Console::Command>(new CommandRecord(stage)));
        console.addCommand("debug", PaintownUtil::ReferenceCount<Console::Command>(new CommandDebug(stage)));
        console.addCommand("change-state", PaintownUtil::ReferenceCount<Console::Command>(new CommandChangeState(stage)));
    }

    bool show_fps = false;

    struct GameState{
        GameState():
            endMatch(false){
            }

        bool endMatch;
    };

    class Logic: public PaintownUtil::Logic {
    public:
        Logic(GameState & state, Mugen::Stage * stage, bool & show_fps, Console::Console & console):
        state(state),
        gameSpeed(1.0),
        stage(stage),
        show_fps(show_fps),
        console(console){
            gameInput.set(Keyboard::Key_F1, 10, false, SlowDown);
            gameInput.set(Keyboard::Key_F2, 10, false, SpeedUp);
            gameInput.set(Keyboard::Key_F3, 10, false, NormalSpeed);
            gameInput.set(Keyboard::Key_F4, 10, true, ToggleDebug);
            gameInput.set(Keyboard::Key_ESC, 0, true, QuitGame);
            gameInput.set(Configuration::config(0).getJoystickQuit(), 0, true, QuitGame);
            gameInput.set(Keyboard::Key_F5, 10, true, SetHealth);
            gameInput.set(Keyboard::Key_F9, 10, true, ShowFps);
            gameInput.set(Keyboard::Key_TILDE, 10, true, ToggleConsole);

            Global::registerInfo(&messages);
        }

        ~Logic(){
            Global::unregisterInfo(&messages);
        }

        InputMap<MugenInput> gameInput;
        GameState & state;
        double gameSpeed;
        Mugen::Stage * stage;
        bool & show_fps;
        Console::Console & console;
        /* global info messages will appear in the console */
        MessageQueue messages;

        void doInput(){
            class Handler: public InputHandler<MugenInput> {
            public:
                Handler(Logic & logic):
                    logic(logic){
                    }

                Logic & logic;

                void release(const MugenInput & out, Keyboard::unicode_t code){
                }

                void press(const MugenInput & out, Keyboard::unicode_t code){
                    switch (out){
                        case SlowDown: {
                            logic.gameSpeed -= 0.1;
                            break;
                        }
                        case SpeedUp: {
                            logic.gameSpeed += 0.1;
                            break;
                        }
                        case NormalSpeed: {
                            logic.gameSpeed = 1;
                            break;
                        }
                        case ToggleDebug: {
                            logic.stage->toggleDebug(0);
                            break;
                        }
                        case QuitGame: {
                            throw QuitGameException();
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

            if (gameSpeed < 0.1){
                gameSpeed = 0.1;
            }
        }

        virtual void run(){
            // Do stage logic catch match exception to handle the next match
            stage->logic();
            while (messages.hasAny()){
                console.addLine(messages.get());
            }
            console.act();
            state.endMatch = stage->isMatchOver();

            if (console.isActive()){
                try{
                    console.doInput();
                } catch (Exception::Return & r){
                    throw QuitGameException();
                }
            } else {
                doInput();
            }
        }

        virtual bool done(){
            return state.endMatch;
        }

        virtual double ticks(double system){
            return Mugen::Util::gameTicks(system, gameSpeed);
        }
    };

    class Draw: public PaintownUtil::Draw {
    public:
        Draw(Mugen::Stage * stage, bool & show_fps, Console::Console & console):
            stage(stage),
            show(true),
            show_fps(show_fps),
            console(console){
            }

        Mugen::Stage * stage;
        bool show;
        bool & show_fps;
        Console::Console & console;
    
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

            Graphics::StretchedBitmap work(DEFAULT_WIDTH, DEFAULT_HEIGHT, screen, Graphics::qualityFilterName(Configuration::getQualityFilter()));
            work.start();
            stage->render(&work);
            work.finish();
            FontRender * render = FontRender::getInstance();
            render->render(&screen);
            console.draw(screen);
            screen.BlitToScreen();
        }
    };

    GameState state;
    Logic logic(state, stage, show_fps, console);
    Draw draw(stage, show_fps, console);

    PaintownUtil::standardLoop(logic, draw);
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

    Character * guy = new Character(path, Stage::Player1Side);
    Global::stream_type & out = Global::debug(0);
    out << "Loading player " << path.path();
    out.flush();
    timer.startTime();
    guy->load();
    timer.endTime();
    out << timer.printTime(" took") << std::endl;

    return guy;
}

static Character * makeCharacter(const std::string & name, bool random, std::vector<Filesystem::AbsolutePath> & all){
    if (random){
        while (all.size() > 0){
            int choice = PaintownUtil::rnd(all.size());
            try{
                return doLoad(all[choice]);
            } catch (const MugenException & failed){
                Global::debug(0) << "Failed to load because " << failed.getReason() << std::endl;
                all.erase(all.begin() + choice);
            } catch (const Filesystem::NotFound & failed){
                Global::debug(0) << "Failed to load because " << failed.getTrace() << std::endl;
                all.erase(all.begin() + choice);
            }
        }
        throw MugenException("No characters left to choose from!");
    } else {
        Filesystem::AbsolutePath path = Storage::instance().findInsensitive(Filesystem::RelativePath("mugen/chars/" + name + "/" + name + ".def"));
        return doLoad(path);
    }
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

void Game::startWatch(const std::string & player1Name, const std::string & player2Name, const std::string & stageName){
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
    try{
        while (true){
            Mugen::CharacterSelect select(systemFile, playerType, gameType);
            select.setPlayer1Keys(Mugen::getPlayer1Keys(20));
            select.setPlayer2Keys(Mugen::getPlayer2Keys(20));
            select.load();
            try{
                {
                    select.run("Training Mode", searcher);
                    select.renderVersusScreen();
                }
                HumanBehavior player1Behavior(getPlayer1Keys(), getPlayer1InputLeft());
                HumanBehavior player2Behavior(getPlayer2Keys(), getPlayer2InputLeft());
                DummyBehavior dummyBehavior;
                // Set regenerative health
                select.getPlayer1()->setRegeneration(true);
                select.getPlayer2()->setRegeneration(true);
                if (playerType == Player1){
                    select.getPlayer1()->setBehavior(&player1Behavior);
                    select.getPlayer2()->setBehavior(&dummyBehavior);
                } else {
                    select.getPlayer1()->setBehavior(&dummyBehavior);
                    select.getPlayer2()->setBehavior(&player2Behavior);
                }
                Mugen::Stage *stage = select.getStage();
                stage->reset();
                runMatch(stage);
            } catch (const QuitGameException & e){
            }
        }
    } catch (const Exception::Return & e){
    }
    Mugen::Data::getInstance().setTime(time);
}

void Game::doWatch(Searcher & searcher){
    /* Do watch screen */
    bool quit = false;
    while (!quit){
	Mugen::CharacterSelect select(systemFile, playerType, gameType);
	select.setPlayer1Keys(Mugen::getPlayer1Keys(20));
	select.setPlayer2Keys(Mugen::getPlayer2Keys(20));
	select.load();
        try{
            {
                select.run("Watch Mode", searcher);
                select.renderVersusScreen();
            }
            LearningAIBehavior player1AIBehavior(Mugen::Data::getInstance().getDifficulty());
            LearningAIBehavior player2AIBehavior(Mugen::Data::getInstance().getDifficulty());
            select.getPlayer1()->setBehavior(&player1AIBehavior);
            select.getPlayer2()->setBehavior(&player2AIBehavior);
            Mugen::Stage *stage = select.getStage();
            stage->reset();
            runMatch(stage);
        } catch (const Exception::Return & e){
	    quit = true;
	} catch (const QuitGameException & e){
        }
    }
}

void Game::doArcade(Searcher & searcher){
    Mugen::CharacterSelect select(systemFile, playerType, gameType);
    select.setPlayer1Keys(Mugen::getPlayer1Keys(20));
    select.setPlayer2Keys(Mugen::getPlayer2Keys(20));
    select.load();
    select.run("Arcade", searcher);
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
        file = select.getPlayer1Def();
    } else {
        file = select.getPlayer2Def();
    }
    Filesystem::AbsolutePath baseDir = file.getDirectory();
    try{
        intro = Storage::instance().lookupInsensitive(baseDir, Filesystem::RelativePath(Util::probeDef(file, "arcade", "intro.storyboard")));
    } catch (const MugenException & fail){
        Global::debug(0) << "Failed to get intro from " << file.path() << " " << fail.getReason() << std::endl;
    } catch (const Filesystem::NotFound & fail){
        Global::debug(0) << "Failed to get intro from " << file.path() << " " << fail.getTrace() << std::endl;
    }

    try{
        ending = Storage::instance().lookupInsensitive(baseDir, Filesystem::RelativePath(Util::probeDef(file, "arcade", "ending.storyboard")));
    } catch (const MugenException & fail){
        Global::debug(0) << "Failed to get ending from " << file.path() << " " << fail.getReason() << std::endl;
    } catch (const Filesystem::NotFound & fail){
        Global::debug(0) << "Failed to get ending from " << file.path() << " " << fail.getTrace() << std::endl;
    }

    try{
        // Win screen if player has ending it will not show this
        if (Util::probeDef(systemFile, "win screen", "enabled") == "1"){
            displayWinScreen = true;
        }
    } catch (const MugenException & fail){
        Global::debug(0) << "Failed to get win screen from " << systemFile.path() << " " << fail.getReason() << std::endl;
    } catch (const Filesystem::NotFound & fail){
        Global::debug(0) << "Failed to get win screen from " << systemFile.path() << " " << fail.getTrace() << std::endl;
    }

    try{
        // Get Default ending
        if (Util::probeDef(systemFile, "default ending", "enabled") == "1"){
            defaultEndingEnabled = true;
            defaultEnding = Mugen::Data::getInstance().getFileFromMotif(Filesystem::RelativePath(Util::probeDef(systemFile, "default ending", "storyboard")));
        }
    } catch (const MugenException & fail){
        Global::debug(0) << "Failed to get ending screen from " << systemFile.path() << " " << fail.getReason() << std::endl;
    } catch (const Filesystem::NotFound & fail){
        Global::debug(0) << "Failed to get ending screen from " << systemFile.path() << " " << fail.getTrace() << std::endl;
    }

    try{
        // Get Game Over
        if (Util::probeDef(systemFile, "game over screen", "enabled") == "1"){
            gameOverEnabled = true;
            gameOver = Mugen::Data::getInstance().getFileFromMotif(Filesystem::RelativePath(Util::probeDef(systemFile, "game over screen", "storyboard")));
        }
    } catch (const MugenException & fail){
        Global::debug(0) << "Failed to get game over screen from " << systemFile.path() << fail.getReason() << std::endl;
    } catch (const Filesystem::NotFound & fail){
        Global::debug(0) << "Failed to get game over screen from " << systemFile.path() << fail.getTrace() << std::endl;
    }

    try{
        // Get credits
        if (Util::probeDef(systemFile, "end credits", "enabled") == "1"){
            gameOverEnabled = true;
            credits = Mugen::Data::getInstance().getFileFromMotif(Filesystem::RelativePath(Util::probeDef(systemFile, "end credits", "storyboard")));
        }
    } catch (const MugenException & fail){
        Global::debug(0) << "Failed to get end credits from " << systemFile.path() << " " << fail.getReason() << std::endl;
    } catch (const Filesystem::NotFound & fail){
        Global::debug(0) << "Failed to get end credits from " << systemFile.path() << " " << fail.getTrace() << std::endl;
    }

    InputMap<Mugen::Keys> input;
    // Get player keys
    if (playerType == Mugen::Player1){
        input = Mugen::getPlayer1Keys(20);
    } else if (playerType == Mugen::Player2){
        input = Mugen::getPlayer2Keys(20);
    }
	
    // Run intro before we begin game
    if (!intro.isEmpty()){
        try{
            Storyboard story(intro, true);
            story.setInput(input);
            story.run();
        } catch (...){
            Global::debug(0) << "Failed to load storyboard for some reason" << std::endl;
        }
    }

    HumanBehavior player1HumanBehavior(getPlayer1Keys(), getPlayer1InputLeft());
    HumanBehavior player2HumanBehavior(getPlayer2Keys(), getPlayer2InputLeft());

    // RandomAIBehavior player1AIBehavior;
    // RandomAIBehavior player2AIBehavior;
    LearningAIBehavior player1AIBehavior(Mugen::Data::getInstance().getDifficulty());
    LearningAIBehavior player2AIBehavior(Mugen::Data::getInstance().getDifficulty());

    bool quit = false;
    
    // Total wins from player
    int wins = 0;

    // Display game over storyboard
    bool displayGameOver = false;

    // Set initial oponent
    if (!select.setNextArcadeMatch()){
        throw MugenException("Could not set up arcade match");
    }
    
    try{
        while (!quit){
            select.renderVersusScreen();
	    
	    /* Reset characters. TODO: why? */
	    select.getPlayer1()->resetPlayer();
	    select.getPlayer2()->resetPlayer();

            /* this is the guy thats in control */
            Mugen::Character * player = 0;
	    std::string musicOverride;
            if (playerType == Mugen::Player1){
                player = select.getPlayer1();
                player->setBehavior(&player1HumanBehavior);
                select.getPlayer2()->setBehavior(&player2AIBehavior);
                // player->setInput(getPlayer1Keys(), getPlayer1InputLeft());
		// Get music from character
		musicOverride = select.getPlayer2Music();
            } else if (playerType == Mugen::Player2){
                player = select.getPlayer2();
                // player->setInput(getPlayer2Keys(), getPlayer2InputLeft());
                player->setBehavior(&player2HumanBehavior);
                select.getPlayer1()->setBehavior(&player1AIBehavior);
		// Get music from character
		musicOverride = select.getPlayer1Music();
            }


            Mugen::Stage *stage = select.getStage();

            // Lets reset the stage for good measure
            stage->reset();

            runMatch(stage, musicOverride);

            /*! *FIXME *TODO
             * Set next match and check if we have 
             * completed the game to handle appropriately and other misc arcade stuff
             */
            // Check players wins
            if (player->getMatchWins() > wins){
                wins = player->getMatchWins();
                // Reset player for next match
                player->resetPlayer();
                // There is a win they may proceed
                if (!select.setNextArcadeMatch()){
                    // Game is over and player has won display ending storyboard
                    if (displayWinScreen && ending.isEmpty()){
                        // Need to parse that and display it for now just ignore

                        // Show Default ending if enabled
                        if (defaultEndingEnabled){
                            if (!defaultEnding.isEmpty()){
                                Storyboard story(defaultEnding, true);
                                story.setInput(input);
                                story.run();
                            }
                        }
                    } else if (defaultEndingEnabled && ending.isEmpty()){
                        if (!defaultEnding.isEmpty()){
                            Storyboard story(defaultEnding, true);
                            story.setInput(input);
                            story.run();
                        }
                    } else if (!ending.isEmpty()){
                        Storyboard story(ending, true);
                        story.setInput(input);
                        story.run();
                    } 
                    if (creditsEnabled){                    
                        // credits
                        if (!credits.isEmpty()){
                            Storyboard story(defaultEnding, true);
                            story.setInput(input);
                            story.run();
                        }
                    }
                    quit = displayGameOver = true;
                }
            } else {
                // Player lost do continue screen if enabled for now just quit
                if (stage->doContinue(playerType, input)){
                    select.reset();
                    select.getPlayer1()->resetPlayer();
                    select.getPlayer2()->resetPlayer();
                    select.run("Arcade", searcher);
                } else {
                    quit = displayGameOver = true;
                }
            }
        }
    } catch (const QuitGameException & e){
    }

    // Show game over if ended through game otherwise just get out
    if (displayGameOver){
        if (!gameOver.isEmpty()){
            Storyboard story(gameOver, true);
            story.setInput(input);
            story.run();
        }
    }
}

void Game::doVersus(Searcher & searcher){
    bool quit = false;
    while (!quit){
        Mugen::CharacterSelect select(systemFile, playerType, gameType);
        select.setPlayer1Keys(Mugen::getPlayer1Keys(20));
        select.setPlayer2Keys(Mugen::getPlayer2Keys(20));
        select.load();
        select.run("Versus Mode", searcher);
        select.renderVersusScreen();

        HumanBehavior player1Input(getPlayer1Keys(), getPlayer1InputLeft());
        HumanBehavior player2Input(getPlayer2Keys(), getPlayer2InputLeft());

        /*
	select.getPlayer1()->setInput(Mugen::getPlayer1Keys(), getPlayer1InputLeft());
	select.getPlayer2()->setInput(Mugen::getPlayer2Keys(), getPlayer2InputLeft());
        */
	select.getPlayer1()->setBehavior(&player1Input);
	select.getPlayer2()->setBehavior(&player2Input);

        select.getStage()->reset();
        try{
            runMatch(select.getStage());
        } catch (const QuitGameException & e){
        }
    }
}

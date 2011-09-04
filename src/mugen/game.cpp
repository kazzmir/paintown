#include "util/bitmap.h"
#include "util/stretch-bitmap.h"
#include "game.h"

#include <ostream>
#include "util/font.h"
#include "util/token.h"
#include "configuration.h"
#include "menu/menu.h"
#include "globals.h"
#include "util/init.h"
#include "util/input/keyboard.h"
#include "util/input/input-manager.h"
#include "util/input/input-source.h"
#include "util/input/input-map.h"
#include "util/funcs.h"
#include "util/timedifference.h"
#include "util/events.h"
#include "factory/font_render.h"
#include "exceptions/shutdown_exception.h"
#include "search.h"
#include "exceptions/exception.h"
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

const int DEFAULT_WIDTH = 320;
const int DEFAULT_HEIGHT = 240;

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
        InputManager::waitForKeys(Keyboard::Key_ENTER, Keyboard::Key_ESC);
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

    bool show_fps = false;

    struct GameState{
        GameState():
            endMatch(false){
            }

        bool endMatch;
    };

    class Logic: public PaintownUtil::Logic {
    public:
        Logic(GameState & state, Mugen::Stage * stage, bool & show_fps):
        state(state),
        gameSpeed(1.0),
        stage(stage),
        show_fps(show_fps){
            /* FIXME: use an enum here */
            gameInput.set(Keyboard::Key_F1, 10, false, 0);
            gameInput.set(Keyboard::Key_F2, 10, false, 1);
            gameInput.set(Keyboard::Key_F3, 10, false, 2);
            gameInput.set(Keyboard::Key_F4, 10, true, 3);
            gameInput.set(Keyboard::Key_ESC, 0, true, 4);
            gameInput.set(Configuration::config(0).getJoystickQuit(), 0, true, 4);
            gameInput.set(Keyboard::Key_F5, 10, true, 5);
            gameInput.set(Keyboard::Key_F9, 10, true, 6);
        }

        InputMap<int> gameInput;
        GameState & state;
        double gameSpeed;
        Mugen::Stage * stage;
        bool & show_fps;

        void doInput(){
            std::vector<InputMap<int>::InputEvent> out = InputManager::getEvents(gameInput, InputSource());
            for (std::vector<InputMap<int>::InputEvent>::iterator it = out.begin(); it != out.end(); it++){
                const InputMap<int>::InputEvent & event = *it;
                if (!event.enabled){
                    continue;
                }

                if (event[0]){
                    gameSpeed -= 0.1;
                }
                if (event[1]){
                    gameSpeed += 0.1;
                }
                if (event[2]){
                    gameSpeed = 1;
                }
                if (event[3]){
                    stage->toggleDebug();
                }
                if (event[4]){
                    throw QuitGameException();
                    /*
                       quit = true;
                       endMatch = true;
                       */
                }
                if (event[5]){
                    stage->setPlayerHealth(1);
                }
                if (event[6]){
                    show_fps = ! show_fps;
                }
            }

            if (gameSpeed < 0.1){
                gameSpeed = 0.1;
            }
        }

        virtual void run(){
            // Do stage logic catch match exception to handle the next match
            stage->logic();
            state.endMatch = stage->isMatchOver();

            doInput();
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
        Draw(Mugen::Stage * stage, bool & show_fps):
            stage(stage),
            show(true),
            show_fps(show_fps){
            }

        Mugen::Stage * stage;
        bool show;
        bool & show_fps;
    
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

            Graphics::StretchedBitmap work(DEFAULT_WIDTH, DEFAULT_HEIGHT, screen);
            work.start();
            stage->render(&work);
            work.finish();
            screen.BlitToScreen();
            // work.BlitToScreen();
        }
    };

    GameState state;
    Logic logic(state, stage, show_fps);
    Draw draw(stage, show_fps);

    PaintownUtil::standardLoop(logic, draw);

#if 0
    Global::speed_counter = 0;

    while (!endMatch){
        bool draw = false;

        if (Global::speed_counter > 0){
            runCounter += Mugen::Util::gameTicks(gameSpeed);
            Global::speed_counter = 0;

            while (runCounter > 1){
                InputManager::poll();
                // Do stage logic catch match exception to handle the next match
                stage->logic();
                endMatch = stage->isMatchOver();

                runCounter -= 1;
                draw = true;

                if (Global::shutdown()){
                    throw ShutdownException();
                }

                std::vector<InputMap<int>::InputEvent> out = InputManager::getEvents(gameInput);
                for (std::vector<InputMap<int>::InputEvent>::iterator it = out.begin(); it != out.end(); it++){
                    const InputMap<int>::InputEvent & event = *it;
                    if (!event.enabled){
                        continue;
                    }

                    if (event[0]){
                        gameSpeed -= 0.1;
                    }
                    if (event[1]){
                        gameSpeed += 0.1;
                    }
                    if (event[2]){
                        gameSpeed = 1;
                    }
                    if (event[3]){
                        stage->toggleDebug();
                    }
                    if (event[4]){
                        throw QuitGameException();
                        /*
                           quit = true;
                           endMatch = true;
                           */
                    }
                    if (event[5]){
                        stage->setPlayerHealth(1);
                    }
                }

                if (gameSpeed < 0.1){
                    gameSpeed = 0.1;
                }
            }
            // Global::speed_counter = 0;
        }

        if (second_counter != Global::second_counter){
            int difference = Global::second_counter - second_counter;
            double alpha = 0.2;
            /* unlikely, but just in case */
            if (difference == 0){
                difference = 1;
            }
            fps = (alpha * fps) + ((1-alpha) * (double) frames / difference);
            // fps[fps_index] = (double) frames / (double) difference;
            // fps_index = (fps_index+1) % max_fps_index;
            second_counter = Global::second_counter;
            frames = 0;
        }

        if (draw){
            frames += 1;
            stage->render(&work);
            /*
            work.Stretch(buffer);

            FontRender * render = FontRender::getInstance();
            render->render(&buffer);

            

            buffer.BlitToScreen();
            */

            if (show_fps){
                const Font & font = Font::getFont(Global::DEFAULT_FONT, 10, 10);
                font.printf(work.getWidth() - 60, work.getHeight() - font.getHeight() - 1, Graphics::makeColor(255,255,255), work, "FPS: %0.2f", 0, fps);
            }

            work.BlitToScreen();
        }

        while (Global::speed_counter == 0){
            PaintownUtil::rest(1);
        }
    }
#endif
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
    Character * player1 = NULL;
    Character * player2 = NULL;
    class Cleanup{
    public:
        Cleanup(Character *& player1, Character *& player2):
        player1(player1),
        player2(player2){
        }

        Character *& player1;
        Character *& player2;

        ~Cleanup(){
            delete player1;
            delete player2;
        }
    };

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
    stage.addPlayer1(player1);
    stage.addPlayer2(player2);
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
    Character * player1 = NULL;
    Character * player2 = NULL;
    class Cleanup{
    public:
        Cleanup(Character *& player1, Character *& player2):
        player1(player1),
        player2(player2){
        }

        Character *& player1;
        Character *& player2;

        ~Cleanup(){
            delete player1;
            delete player2;
        }
    };

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
    stage.addPlayer1(player1);
    stage.addPlayer2(player2);
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

/* is there a reason why doArcade and doVersus don't share the main loop?
 * answer: jon moved the main loop to runMatch(). versus should use this eventually
 */
void Game::doArcade(Searcher & searcher){
    /* FIXME: there isn't really a need to have this bitmap exist forever.
     * a temporary bitmap can be created when its needed.
     */
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

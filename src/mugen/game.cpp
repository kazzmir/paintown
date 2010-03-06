#include "util/bitmap.h"
#include "game.h"

#include <ostream>
#include "util/font.h"
#include "util/token.h"
#include "configuration.h"
#include "menu/menu.h"
#include "menu/menu_global.h"
#include "globals.h"
#include "init.h"
#include "input/keyboard.h"
#include "input/input-manager.h"
#include "input/input-map.h"
#include "util/funcs.h"
#include "factory/font_render.h"
#include "shutdown_exception.h"
#include "loading.h"

#include "character.h"
#include "util.h"
#include "font.h"
#include "menu.h"
#include "stage.h"
#include "character-select.h"
#include "storyboard.h"
#include "behavior.h"
#include "music.h"
#include "config.h"

namespace PaintownUtil = ::Util;

using namespace Mugen;

const int DEFAULT_WIDTH = 320;
const int DEFAULT_HEIGHT = 240;

static void showError(const Bitmap & screen, const MugenException & e){
    screen.BlitFromScreen(0, 0);
    Bitmap error(screen.getWidth() - 100, screen.getHeight() - 100);
    error.fill(Bitmap::darken(Bitmap::makeColor(255, 0, 0), 3));
    static const char * DEFAULT_FONT = "fonts/arial.ttf";
    const Font & font = Font::getFont(Filesystem::find(Filesystem::RelativePath(DEFAULT_FONT)).path(), 18, 18);
    int y = 10;
    std::ostringstream out;
    out << "Press ENTER to continue\n";
    out << "\n";
    out << "We are very sorry but an error has occured while trying to load MUGEN.";
    out << " " << e.getReason();
    font.printfWrap(10, 10, Bitmap::makeColor(255, 255, 255), error, error.getWidth() - 20, out.str(), 0);
    Global::debug(0) << out.str() << std::endl;

    Bitmap::transBlender(0, 0, 0, 200);
    error.drawTrans(50, 50, screen);
    screen.BlitToScreen();
}

Game::Game(const PlayerType & playerType, const GameType & gameType, const Filesystem::AbsolutePath & systemFile):
playerType(playerType),
gameType(gameType),
systemFile(systemFile),
motifDirectory(systemFile.getDirectory()){
}

Game::~Game(){
}

void Game::run(){
    Bitmap screen(GFX_X, GFX_Y);

    try{
        switch (gameType){
            default:
            case Arcade:
                doArcade(screen);
                break;
            case Versus:
                doVersus(screen);
                break;
            case TeamArcade:
                //select.run("Team Arcade" , 1, true, &screen);
                break;
            case TeamVersus:
                //gameInfo = select.run("Team Versus" , 2, true, &screen);
                break;
            case TeamCoop:
                //gameInfo = select.run("Team Cooperative" , 1, true, &screen);
                break;
            case Survival:
                //gameInfo = select.run("Survival" , 1, true, &screen);
                break;
            case SurvivalCoop:
                //gameInfo = select.run("Survival Cooperative" , 1, true, &screen);
                break;
            case Training:
                doTraining(screen);
                break;
            case Watch:
                doWatch(screen);
                break;
        }
    } catch (const MugenException e){
        showError(screen, e);

        InputMap<int> wait;
        wait.set(Keyboard::Key_ENTER, 0, false, 1);
        wait.set(Keyboard::Key_ESC, 0, false, 1);
        InputManager::waitForPress(wait, 1);
        InputManager::waitForRelease(wait, 1);
    }
}

/* kind of dumb to just copy/paste the above. find a better solution */
static InputMap<Mugen::Keys> getPlayer1InputLeft(){
    InputMap<Mugen::Keys> input;
    input.set(Keyboard::Key_UP, 0, false, Mugen::Up);
    input.set(Keyboard::Key_DOWN, 0, false, Mugen::Down);
    input.set(Keyboard::Key_LEFT, 0, false, Mugen::Right);
    input.set(Keyboard::Key_RIGHT, 0, false, Mugen::Left);

    input.set(Joystick::Up, 0, false, Mugen::Up);
    input.set(Joystick::Left, 0, false, Mugen::Right);
    input.set(Joystick::Right, 0, false, Mugen::Left);
    input.set(Joystick::Down, 0, false, Mugen::Down);

    input.set(Keyboard::Key_A, 0, true, Mugen::A);
    input.set(Keyboard::Key_S, 0, true, Mugen::B);
    input.set(Keyboard::Key_D, 0, true, Mugen::C);
    input.set(Keyboard::Key_Z, 0, true, Mugen::X);
    input.set(Keyboard::Key_X, 0, true, Mugen::Y);
    input.set(Keyboard::Key_C, 0, true, Mugen::Z);
    input.set(Keyboard::Key_ENTER, 0, true, Mugen::Start);
    input.set(Keyboard::Key_ESC, 0, true, Mugen::Esc);

    input.set(Joystick::Button1, 0, false, Mugen::A);
    input.set(Joystick::Button2, 0, false, Mugen::B);
    input.set(Joystick::Button3, 0, false, Mugen::X);
    input.set(Joystick::Button4, 0, false, Mugen::Y);

    return input;
}

static InputMap<Mugen::Keys> getPlayer2InputLeft(){
    InputMap<Mugen::Keys> input;
    input.set(Keyboard::Key_Y, 0, false, Mugen::Up);
    input.set(Keyboard::Key_H, 0, false, Mugen::Down);
    input.set(Keyboard::Key_G, 0, false, Mugen::Right);
    input.set(Keyboard::Key_J, 0, false, Mugen::Left);

    input.set(Keyboard::Key_I, 0, false, Mugen::A);
    input.set(Keyboard::Key_O, 0, false, Mugen::B);
    input.set(Keyboard::Key_P, 0, false, Mugen::C);
    input.set(Keyboard::Key_8, 0, false, Mugen::X);
    input.set(Keyboard::Key_9, 0, false, Mugen::Y);
    input.set(Keyboard::Key_0, 0, false, Mugen::Z);
    input.set(Keyboard::Key_K, 0, false, Mugen::Start);
    input.set(Keyboard::Key_ESC, 0, true, Mugen::Esc);
    return input;
}

class QuitGameException: public MugenException {
public:
    QuitGameException():
        MugenException("Quit game"){
        }

    virtual ~QuitGameException() throw (){
    }
};

static void runMatch(MugenStage * stage, const Bitmap & buffer){
    Bitmap work(DEFAULT_WIDTH,DEFAULT_HEIGHT);
    InputMap<int> gameInput;
    gameInput.set(Keyboard::Key_F1, 10, false, 0);
    gameInput.set(Keyboard::Key_F2, 10, false, 1);
    gameInput.set(Keyboard::Key_F3, 10, false, 2);
    gameInput.set(Keyboard::Key_F4, 10, true, 3);
    gameInput.set(Keyboard::Key_ESC, 0, true, 4);
    gameInput.set(Keyboard::Key_F5, 10, true, 5);

    Music::changeSong();

    double gameSpeed = 1.0;
    double runCounter = 0;
    double mugenSpeed = 60;

    unsigned int second_counter = Global::second_counter;
    int frames = 0;
    double fps = Global::TICS_PER_SECOND;
    bool show_fps = true;

    bool endMatch = false;

    while (!endMatch){
        bool draw = false;

        if (Global::speed_counter > 0){
            runCounter += Global::speed_counter * gameSpeed * mugenSpeed / Global::TICS_PER_SECOND;
            if (runCounter > 10){
                runCounter = 10;
            }

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

                InputMap<int>::Output out = InputManager::getMap(gameInput);
                if (out[0]){
                    gameSpeed -= 0.1;
                }
                if (out[1]){
                    gameSpeed += 0.1;
                }
                if (out[2]){
                    gameSpeed = 1;
                }
                if (out[3]){
                    stage->toggleDebug();
                }
                if (out[4]){
                    throw QuitGameException();
                    /*
                    quit = true;
                    endMatch = true;
                    */
                }
                if (out[5]){
                    stage->setPlayerHealth(1);
                }
                if (gameSpeed < 0.1){
                    gameSpeed = 0.1;
                }
            }
            Global::speed_counter = 0;
        }

        if (second_counter != Global::second_counter){
            int difference = Global::second_counter - second_counter;
            /* unlikely, but just in case */
            if (difference == 0){
                difference = 1;
            }
            fps = (0.75 * fps) + (0.25 * (double) frames / difference);
            // fps[fps_index] = (double) frames / (double) difference;
            // fps_index = (fps_index+1) % max_fps_index;
            second_counter = Global::second_counter;
            frames = 0;
        }

        if (draw){
            frames += 1;
            stage->render(&work);
            work.Stretch(buffer);

            FontRender * render = FontRender::getInstance();
            render->render(&buffer);

            if (show_fps){
                static const char * DEFAULT_FONT = "fonts/arial.ttf";
                const Font & font = Font::getFont(Filesystem::find(Filesystem::RelativePath(DEFAULT_FONT)).path(), 20, 20 );
                font.printf(buffer.getWidth() - 120, buffer.getHeight() - font.getHeight() - 1, Bitmap::makeColor(255,255,255), buffer, "FPS: %0.2f", 0, fps );
            }

            buffer.BlitToScreen();
        }

        while (Global::speed_counter == 0){
            PaintownUtil::rest(1);
        }
    }
}

void Game::doTraining(const Bitmap & bmp){
    int time = Mugen::Data::getInstance().getTime();
    Mugen::Data::getInstance().setTime(-1);
    try{
        while (true){
            Mugen::CharacterSelect select(systemFile, playerType, gameType);
            select.setPlayer1Keys(Mugen::getPlayer1Keys(20));
            select.setPlayer2Keys(Mugen::getPlayer2Keys(20));
            select.load();
            try{
                select.run("Training Mode", bmp);
                select.renderVersusScreen(bmp);
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
                MugenStage *stage = select.getStage();
                stage->reset();
                runMatch(stage, bmp);
            } catch (const QuitGameException & e){
            }
        }
    } catch (const ReturnException & e){
    }
    Mugen::Data::getInstance().setTime(time);
}

void Game::doWatch(const Bitmap & bmp){
    /* Do watch screen */
    bool quit = false;
    while (!quit){
	Mugen::CharacterSelect select(systemFile, playerType, gameType);
	select.setPlayer1Keys(Mugen::getPlayer1Keys(20));
	select.setPlayer2Keys(Mugen::getPlayer2Keys(20));
	select.load();
        try{
            select.run("Watch Mode", bmp);
            select.renderVersusScreen(bmp);
            LearningAIBehavior player1AIBehavior(Mugen::Data::getInstance().getDifficulty());
            LearningAIBehavior player2AIBehavior(Mugen::Data::getInstance().getDifficulty());
            select.getPlayer1()->setBehavior(&player1AIBehavior);
            select.getPlayer2()->setBehavior(&player2AIBehavior);
            MugenStage *stage = select.getStage();
            stage->reset();
            runMatch(stage, bmp);
        } catch (const ReturnException & e){
	    quit = true;
	} catch (const QuitGameException & e){
        }
    }
}

/* is there a reason why doArcade and doVersus don't share the main loop?
 * answer: jon moved the main loop to runMatch(). versus should use this eventually
 */
void Game::doArcade(const Bitmap & bmp){
    Mugen::CharacterSelect select(systemFile, playerType, gameType);
    select.setPlayer1Keys(Mugen::getPlayer1Keys(20));
    select.setPlayer2Keys(Mugen::getPlayer2Keys(20));
    select.load();
    select.run("Arcade", bmp);
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

    try{
        // get intro and ending for player
        Filesystem::AbsolutePath file;
	if (playerType == Player1){
	    file = select.getPlayer1Def();
	} else {
	    file = select.getPlayer2Def();
	}
        Filesystem::AbsolutePath baseDir = file.getDirectory();
	intro = Util::getCorrectFileLocation(baseDir,Util::probeDef(file, "arcade","intro.storyboard"));
	ending = Util::getCorrectFileLocation(baseDir,Util::probeDef(file, "arcade","ending.storyboard"));
        
        // Win screen if player has ending it will not show this
        if (Util::probeDef(systemFile, "win screen", "enabled") == "1"){
            displayWinScreen = true;
        }
        
        // Get Default ending
        if (Util::probeDef(systemFile, "default ending", "enabled") == "1"){
            defaultEndingEnabled = true;
            defaultEnding = Mugen::Data::getInstance().getFileFromMotif(Filesystem::RelativePath(Util::probeDef(systemFile, "default ending", "storyboard")));
        }
        // Get Game Over
        if (Util::probeDef(systemFile, "game over screen", "enabled") == "1"){
            gameOverEnabled = true;
            gameOver = Mugen::Data::getInstance().getFileFromMotif(Filesystem::RelativePath(Util::probeDef(systemFile, "game over screen", "storyboard")));
        }
        // Get credits
        if (Util::probeDef(systemFile, "end credits", "enabled") == "1"){
            gameOverEnabled = true;
            credits = Mugen::Data::getInstance().getFileFromMotif(Filesystem::RelativePath(Util::probeDef(systemFile, "end credits", "storyboard")));
        }

    } catch (const MugenException & e){
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
	Storyboard story(intro);
	story.setInput(input);
	story.run(bmp);
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
    select.setNextArcadeMatch();
    
    try{
        while (!quit){
            select.renderVersusScreen(bmp);

            /* this is the guy thats in control */
            Mugen::Character * player = 0;
            if (playerType == Mugen::Player1){
                player = select.getPlayer1();
                player->setBehavior(&player1HumanBehavior);
                select.getPlayer2()->setBehavior(&player2AIBehavior);
                // player->setInput(getPlayer1Keys(), getPlayer1InputLeft());
            } else if (playerType == Mugen::Player2){
                player = select.getPlayer2();
                // player->setInput(getPlayer2Keys(), getPlayer2InputLeft());
                player->setBehavior(&player2HumanBehavior);
                select.getPlayer1()->setBehavior(&player1AIBehavior);
            }


            MugenStage *stage = select.getStage();

            // Lets reset the stage for good measure
            stage->reset();

            runMatch(stage, bmp);

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
                                Storyboard story(defaultEnding);
                                story.setInput(input);
                                story.run(bmp);
                            }
                        }
                    } else if (defaultEndingEnabled && ending.isEmpty()){
                        if (!defaultEnding.isEmpty()){
                            Storyboard story(defaultEnding);
                            story.setInput(input);
                            story.run(bmp);
                        }
                    } else if (!ending.isEmpty()){
                        Storyboard story(ending);
                        story.setInput(input);
                        story.run(bmp);
                    } 
                    if (creditsEnabled){                    
                        // credits
                        if (!credits.isEmpty()){
                            Storyboard story(defaultEnding);
                            story.setInput(input);
                            story.run(bmp);
                        }
                    }
                    quit = displayGameOver = true;
                }
            } else {
                // Player lost do continue screen if enabled for now just quit
                if (stage->doContinue(playerType, input, bmp)){
                    select.reset();
                    select.getPlayer1()->resetPlayer();
                    select.getPlayer2()->resetPlayer();
                    select.run("Arcade", bmp);
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
            Storyboard story(gameOver);
            story.setInput(input);
            story.run(bmp);
        }
    }
}

/* FIXME: can this code use runMatch() ? */
void Game::doVersus(const Bitmap & bmp){
    bool quit = false;
    while (!quit){
        Mugen::CharacterSelect select(systemFile, playerType, gameType);
        select.setPlayer1Keys(Mugen::getPlayer1Keys(20));
        select.setPlayer2Keys(Mugen::getPlayer2Keys(20));
        select.load();
	select.run("Versus Mode", bmp);
	select.renderVersusScreen(bmp);

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
            runMatch(select.getStage(), bmp);
        } catch (const QuitGameException & e){
        }
#if 0

	MugenStage *stage = select.getStage();
	InputMap<int> gameInput;
	gameInput.set(Keyboard::Key_F1, 10, false, 0);
	gameInput.set(Keyboard::Key_F2, 10, false, 1);
	gameInput.set(Keyboard::Key_F3, 10, false, 2);
	gameInput.set(Keyboard::Key_F4, 10, true, 3);
	gameInput.set(Keyboard::Key_ESC, 0, true, 4);
	
	Bitmap work(DEFAULT_WIDTH,DEFAULT_HEIGHT);
	double gameSpeed = 1.0;
	double runCounter = 0;
	double mugenSpeed = 60;

	// Lets reset the stage for good measure
	stage->reset();

        bool endMatch = false;

	while (!endMatch){
	    bool draw = false;

	    if ( Global::speed_counter > 0 ){
		runCounter += Global::speed_counter * gameSpeed * mugenSpeed / Global::TICS_PER_SECOND;
		while (runCounter > 1){
		    InputManager::poll();
		    stage->logic();
                    if (stage->isMatchOver()){
                        endMatch = true;
                    }
		    runCounter -= 1;
		    draw = true;

                    if (Global::shutdown()){
                        throw ShutdownException();
                    }

		    InputMap<int>::Output out = InputManager::getMap(gameInput);
		    if (out[0]){
			gameSpeed -= 0.1;
		    }
		    if (out[1]){
			gameSpeed += 0.1;
		    }
		    if (out[2]){
			gameSpeed = 1;
		    }
		    if (out[3]){
			stage->toggleDebug();
		    }
		    if (out[4]){
			endMatch = true;
		    }
		    if (gameSpeed < 0.1){
			gameSpeed = 0.1;
		    }
		}
		Global::speed_counter = 0;
	    }

	    if (draw){
		stage->render(&work);
		work.Stretch(bmp);

		FontRender * render = FontRender::getInstance();
		render->render(&bmp);
    
		bmp.BlitToScreen();
	    }

	    while (Global::speed_counter == 0){
		PaintownUtil::rest(1);
	    }
	}
#endif
    }
}

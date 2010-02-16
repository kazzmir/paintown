#include "mugen/game.h"

#include "util/bitmap.h"
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

#include "config.h"

namespace PaintownUtil = ::Util;

using namespace Mugen;

const int DEFAULT_WIDTH = 320;
const int DEFAULT_HEIGHT = 240;

Game::Game(const PlayerType & playerType, const GameType & gameType, const std::string & systemFile):
playerType(playerType),
gameType(gameType),
systemFile(systemFile),
motifDirectory(Util::getFileDir(systemFile)){
}

Game::~Game(){
}

void Game::run(){
    Bitmap screen(GFX_X, GFX_Y);
    
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
	    //gameInfo = select.run("Training Mode" , 1, true, &screen);
	    break;
	case Watch:
	    //gameInfo = select.run("Watch Mode" , 1, true, &screen);
	    doWatch(screen);
	    break;
    }
}

/* kind of dumb to just copy/paste the above. find a better solution */
static InputMap<Mugen::Keys> getPlayer1InputLeft(){
    InputMap<Mugen::Keys> input;
    input.set(Keyboard::Key_UP, 0, false, Mugen::Up);
    input.set(Keyboard::Key_DOWN, 0, false, Mugen::Down);
    input.set(Keyboard::Key_LEFT, 0, false, Mugen::Right);
    input.set(Keyboard::Key_RIGHT, 0, false, Mugen::Left);

    input.set(Keyboard::Key_A, 0, true, Mugen::A);
    input.set(Keyboard::Key_S, 0, true, Mugen::B);
    input.set(Keyboard::Key_D, 0, true, Mugen::C);
    input.set(Keyboard::Key_Z, 0, true, Mugen::X);
    input.set(Keyboard::Key_X, 0, true, Mugen::Y);
    input.set(Keyboard::Key_C, 0, true, Mugen::Z);
    input.set(Keyboard::Key_ENTER, 0, true, Mugen::Start);
    input.set(Keyboard::Key_ESC, 0, true, Mugen::Esc);
    input.set(Keyboard::Key_ENTER, 0, true, Mugen::Enter);
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
    input.set(Keyboard::Key_L, 0, false, Mugen::Start);
    input.set(Keyboard::Key_ESC, 0, true, Mugen::Esc);
    input.set(Keyboard::Key_K, 0, true, Mugen::Enter);
    return input;
}

class QuitGameException: public MugenException {
public:
    QuitGameException():
        MugenException(){
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
                static const char * DEFAULT_FONT = "/fonts/arial.ttf";
                const Font & font = Font::getFont(Filesystem::find(DEFAULT_FONT), 20, 20 );
                font.printf(buffer.getWidth() - 120, buffer.getHeight() - font.getHeight() - 1, Bitmap::makeColor(255,255,255), buffer, "FPS: %0.2f", 0, fps );
            }

            buffer.BlitToScreen();
        }

        while (Global::speed_counter == 0){
            PaintownUtil::rest(1);
        }
    }
}

void Game::doWatch(const Bitmap & bmp){
    /* **WRONG! you don't select randomly!
    Current player makes selection for his slot then chooses opponent then stage
    This is very wrong! -> make select choose random characters and screens */
    bool quit = false;
    while (!quit){
	Mugen::CharacterSelect select(systemFile, playerType, gameType);
	select.setPlayer1Keys(Mugen::getPlayer1Keys(20));
	select.setPlayer2Keys(Mugen::getPlayer2Keys(20));
	select.load();
        try{
            select.run("Arcade", bmp);
            select.renderVersusScreen(bmp);
            LearningAIBehavior player1AIBehavior(Mugen::Data::getInstance().getDifficulty());
            LearningAIBehavior player2AIBehavior(Mugen::Data::getInstance().getDifficulty());
            select.getPlayer1()->setBehavior(&player1AIBehavior);
            select.getPlayer2()->setBehavior(&player2AIBehavior);
            MugenStage *stage = select.getStage();
            stage->reset();
            runMatch(stage, bmp);
        } catch (const ReturnException & e){
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
    std::string intro;
    std::string ending;
    bool displayWinScreen = false;
    bool continueScreenEnabled = false;
    bool defaultEndingEnabled = false;
    std::string defaultEnding;
    bool gameOverEnabled = false;
    std::string gameOver;
    bool creditsEnabled = false;
    std::string credits;

    try{
        // get intro and ending for player
	std::string file = Filesystem::find(select.getPlayer1Def());
	std::string baseDir = Util::getFileDir(file);
	intro = Util::getCorrectFileLocation(baseDir,Util::probeDef(file,"arcade","intro.storyboard"));
	ending = Util::getCorrectFileLocation(baseDir,Util::probeDef(file,"arcade","ending.storyboard"));
        
        // Win screen if player has ending it will not show this
        if (Util::probeDef(systemFile, "win screen", "enabled") == "1"){
            displayWinScreen = true;
        }

        // Continue screen
        if (Util::probeDef(systemFile, "continue screen", "enabled") == "1"){
            continueScreenEnabled = true;
        }
        // Get Default ending
        if (Util::probeDef(systemFile, "default ending", "enabled") == "1"){
            defaultEndingEnabled = true;
            defaultEnding = Mugen::Data::getInstance().getFileFromMotif(Util::probeDef(systemFile, "default ending", "storyboard"));
        }
        // Get Game Over
        if (Util::probeDef(systemFile, "game over screen", "enabled") == "1"){
            gameOverEnabled = true;
            gameOver = Mugen::Data::getInstance().getFileFromMotif(Util::probeDef(systemFile, "game over screen", "storyboard"));
        }
        // Get credits
        if (Util::probeDef(systemFile, "end credits", "enabled") == "1"){
            gameOverEnabled = true;
            credits = Mugen::Data::getInstance().getFileFromMotif(Util::probeDef(systemFile, "end credits", "storyboard"));
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
    if (!intro.empty()){
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
                    if (displayWinScreen && ending.empty()){
                        // Need to parse that and display it for now just ignore

                        // Show Default ending if enabled
                        if (defaultEndingEnabled){
                            if (!defaultEnding.empty()){
                                Storyboard story(defaultEnding);
                                story.setInput(input);
                                story.run(bmp);
                            }
                        }
                    } else if (defaultEndingEnabled && ending.empty()){
                        if (!defaultEnding.empty()){
                            Storyboard story(defaultEnding);
                            story.setInput(input);
                            story.run(bmp);
                        }
                    } else if (!ending.empty()){
                        Storyboard story(ending);
                        story.setInput(input);
                        story.run(bmp);
                    } 
                    if (creditsEnabled){                    
                        // credits
                        if (!credits.empty()){
                            Storyboard story(defaultEnding);
                            story.setInput(input);
                            story.run(bmp);
                        }
                    }
                    quit = displayGameOver = true;
                }
            } else {
                // Player lost do continue screen if enabled for now just quit
                if (continueScreenEnabled){
                } else {
                }
                quit = displayGameOver = true;
            }
        }
    } catch (const QuitGameException & e){
    }

    // Show game over if ended through game otherwise just get out
    if (displayGameOver){
        if (!gameOver.empty()){
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
    }
}


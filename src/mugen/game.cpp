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
#include "mugen/util.h"
#include "mugen/font.h"
#include "mugen/menu.h"
#include "mugen/stage.h"
#include "character-select.h"
#include "storyboard.h"

#include "mugen/match-exception.h"
#include "mugen/config.h"

namespace PaintownUtil = ::Util;

using namespace Mugen;

const int DEFAULT_WIDTH = 320;
const int DEFAULT_HEIGHT = 240;

Game::Game(GameType type, const std::string & systemFile):
type(type),
systemFile(systemFile),
motifDirectory(Util::getFileDir(systemFile)){
}

Game::~Game(){
}

void Game::run(){
    Bitmap screen(GFX_X, GFX_Y);
    Mugen::CharacterSelect select(systemFile,type);
    select.setPlayer1Keys(Mugen::getPlayer1MenuKeys());
    select.setPlayer2Keys(Mugen::getPlayer2MenuKeys());
    select.load();
    
    switch (type){
	default:
	case Arcade:
	    doArcade(screen, select);
	    break;
	case Versus:
	    doVersus(screen,select);
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
	    break;
    }
}


static InputMap<Mugen::Command::Keys> getPlayer1InputRight(){
    InputMap<Mugen::Command::Keys> input;
    input.set(Keyboard::Key_UP, 0, false, Mugen::Command::Up);
    input.set(Keyboard::Key_DOWN, 0, false, Mugen::Command::Down);
    input.set(Keyboard::Key_RIGHT, 0, false, Mugen::Command::Forward);
    input.set(Keyboard::Key_LEFT, 0, false, Mugen::Command::Back);

    input.set(Keyboard::Key_A, 0, false, Mugen::Command::A);
    input.set(Keyboard::Key_S, 0, false, Mugen::Command::B);
    input.set(Keyboard::Key_D, 0, false, Mugen::Command::C);
    input.set(Keyboard::Key_Z, 0, false, Mugen::Command::X);
    input.set(Keyboard::Key_X, 0, false, Mugen::Command::Y);
    input.set(Keyboard::Key_C, 0, false, Mugen::Command::Z);
    return input;
}

/* kind of dumb to just copy/paste the above. find a better solution */
static InputMap<Mugen::Command::Keys> getPlayer1InputLeft(){
    InputMap<Mugen::Command::Keys> input;
    input.set(Keyboard::Key_UP, 0, false, Mugen::Command::Up);
    input.set(Keyboard::Key_DOWN, 0, false, Mugen::Command::Down);

    /* these keys are flipped from ...InputRight */
    input.set(Keyboard::Key_LEFT, 0, false, Mugen::Command::Forward);
    input.set(Keyboard::Key_RIGHT, 0, false, Mugen::Command::Back);

    input.set(Keyboard::Key_A, 0, false, Mugen::Command::A);
    input.set(Keyboard::Key_S, 0, false, Mugen::Command::B);
    input.set(Keyboard::Key_D, 0, false, Mugen::Command::C);
    input.set(Keyboard::Key_Z, 0, false, Mugen::Command::X);
    input.set(Keyboard::Key_X, 0, false, Mugen::Command::Y);
    input.set(Keyboard::Key_C, 0, false, Mugen::Command::Z);
    return input;
}

/* is there a reason why doArcade and doVersus don't share the main loop? */

void Game::doArcade(const Bitmap & bmp, CharacterSelect & select){
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
    // Run intro before we begin game
    if (!intro.empty()){
	Storyboard story(intro);
	story.setInput(Mugen::getPlayer1MenuKeys());
	story.run(bmp);
    }
    bool quit = false;
    unsigned int second_counter = Global::second_counter;
    int frames = 0;
    double fps = Global::TICS_PER_SECOND;
    bool show_fps = true;

    // Total wins from player
    int wins = 0;

    // Display game over storyboard
    bool displayGameOver = false;

    while (!quit){
	select.renderVersusScreen(bmp);
	select.getPlayer1()->setInput(getPlayer1InputRight(), getPlayer1InputLeft());
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

	    if (Global::speed_counter > 0){
		runCounter += Global::speed_counter * gameSpeed * mugenSpeed / Global::TICS_PER_SECOND;
                if (runCounter > 10){
                    runCounter = 10;
                }

		while (runCounter > 1){
		    InputManager::poll();
                    // Do stage logic catch match exception to handle the next match
                    try{
		        stage->logic();
                    } catch (const MatchException & e){
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
			quit = true;
                        endMatch = true;
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
		work.Stretch(bmp);

		FontRender * render = FontRender::getInstance();
		render->render(&bmp);

                if (show_fps){
                    static const char * DEFAULT_FONT = "/fonts/arial.ttf";
                    const Font & font = Font::getFont(Filesystem::find(DEFAULT_FONT), 20, 20 );
                    font.printf(bmp.getWidth() - 120, bmp.getHeight() - font.getHeight() - 1, Bitmap::makeColor(255,255,255), bmp, "FPS: %0.2f", 0, fps );
                }
    
		bmp.BlitToScreen();
	    }

	    while (Global::speed_counter == 0){
		PaintownUtil::rest(1);
	    }
	}
	/*! *FIXME *TODO
	* Set next match and check if we have 
	* completed the game to handle appropriately and other misc arcade stuff
	*/
        // Check players wins
        if (select.getPlayer1()->getMatchWins() > wins){
            wins = select.getPlayer1()->getMatchWins();
            // Reset player for next match
            select.getPlayer1()->resetPlayer();
            // There is a win they may proceed
            if (!select.setNextArcadeMatch()){
                // Game is over and player has won display ending storyboard
                if (displayWinScreen && ending.empty()){
                    // Need to parse that and display it for now just ignore
                    
                    // Show Default ending if enabled
                    if (defaultEndingEnabled){
                        if (!defaultEnding.empty()){
                            Storyboard story(defaultEnding);
                            story.setInput(Mugen::getPlayer1MenuKeys());
                            story.run(bmp);
                        }
                    }
                } else if (defaultEndingEnabled && ending.empty()){
                    if (!defaultEnding.empty()){
                        Storyboard story(defaultEnding);
                        story.setInput(Mugen::getPlayer1MenuKeys());
                        story.run(bmp);
                    }
                } else if (!ending.empty()){
	            Storyboard story(ending);
	            story.setInput(Mugen::getPlayer1MenuKeys());
	            story.run(bmp);
                } 
                if (creditsEnabled){                    
                    // credits
                    if (!credits.empty()){
                        Storyboard story(defaultEnding);
                        story.setInput(Mugen::getPlayer1MenuKeys());
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
    // Show game over if ended through game otherwise just get out
    if (displayGameOver){
        if (!gameOver.empty()){
            Storyboard story(gameOver);
            story.setInput(Mugen::getPlayer1MenuKeys());
            story.run(bmp);
        }
    }
}

void Game::doVersus(const Bitmap & bmp, CharacterSelect & select){
    bool quit = false;
    while(!quit){
	select.run("Versus Mode", bmp);
	select.renderVersusScreen(bmp);
	select.getPlayer1()->setInput(getPlayer1InputRight(), getPlayer1InputLeft());
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

	while( !quit ){
	    bool draw = false;

	    if ( Global::speed_counter > 0 ){
		runCounter += Global::speed_counter * gameSpeed * mugenSpeed / Global::TICS_PER_SECOND;
		while (runCounter > 1){
		    InputManager::poll();
		    stage->logic();
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
			quit = true;
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

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
#include "loading.h"

#include "character.h"
#include "mugen_util.h"
#include "mugen_font.h"
#include "mugen_menu.h"
#include "mugen_stage.h"
#include "character_select.h"
#include "storyboard.h"

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

void Game::doArcade(const Bitmap & bmp, CharacterSelect & select){
    select.run("Arcade", bmp);
    std::string intro;
    std::string ending;  
    try{
	std::string file = Filesystem::find(select.getPlayer1Def());
	std::string baseDir = Util::getFileDir(file);
	intro = Util::getCorrectFileLocation(baseDir,Util::probeDef(file,"arcade","intro.storyboard"));
	ending = Util::getCorrectFileLocation(baseDir,Util::probeDef(file,"arcade","ending.storyboard"));
    } catch (const MugenException & e){
    }
    // Run intro before we begin game
    if (!intro.empty()){
	Storyboard story(intro);
	story.setInput(Mugen::getPlayer1MenuKeys());
	story.run(bmp);
    }
    bool quit = false;
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

	while( !quit ){
	    bool draw = false;

	    if ( Global::speed_counter > 0 ){
		runCounter += Global::speed_counter * gameSpeed * mugenSpeed / Global::TICS_PER_SECOND;
		while (runCounter > 1){
		    InputManager::poll();
		    stage->logic();
		    runCounter -= 1;
		    draw = true;

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
	/*! *FIXME *TODO
	* Set next match and check if we have 
	* completed the game to handle appropriately and other misc arcade stuff
	*/
	select.setNextArcadeMatch();
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



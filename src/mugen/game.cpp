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

namespace PaintownUtil = ::Util;

using namespace Mugen;

Game::Game(GameType type, const std::string & systemFile):
type(type),
systemFile(systemFile),
motifDirectory(Util::getFileDir(systemFile)){
}

Game::~Game(){
}

void Game::run(){
    Bitmap screen(GFX_X, GFX_Y);

    std::string selectFile = Util::probeDef(systemFile, "files", "select");
    Mugen::CharacterSelect select(selectFile,type);
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

void Game::doArcade(const Bitmap & bmp, CharacterSelect & select){
    select.run("Arcade", bmp);
}

void Game::doVersus(const Bitmap & bmp, CharacterSelect & select){
    select.run("Versus Mode", bmp);
}

#if 0
void Game::runGame(MugenStage * stage, const Bitmap & screen){
    InputMap<int> gameInput;
    gameInput.set(Keyboard::Key_F1, 10, false, 0);
    gameInput.set(Keyboard::Key_F2, 10, false, 1);
    gameInput.set(Keyboard::Key_F3, 10, false, 2);
    gameInput.set(Keyboard::Key_F4, 10, true, 3);
    gameInput.set(Keyboard::Key_ESC, 0, true, 4);
    
    // Load the stage
    try{
        /* FIXME: replace hard coded numbers */
        Bitmap work(320,240);
        bool quit = false;
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
                work.Stretch(screen);

                FontRender * render = FontRender::getInstance();
                render->render(&screen);
    
                screen.BlitToScreen();
            }

            while (Global::speed_counter == 0){
                PaintownUtil::rest(1);
            }
        }
    } catch (const MugenException &ex){
	Global::debug(0) << "Problem with stage: " << stage->getName() << " Problem was: " << ex.getReason() << std::endl;
    }
}

MugenStage * Game::setupStage(Mugen::SelectedChars * gameInfo){
    try{
        /* Need to display VS Screen here */
	
	MugenStage * stage = gameInfo->selectedStage;
        // Load player 1
        gameInfo->team1[0]->load();
        gameInfo->team1[0]->setInput(getPlayer1Input(), getPlayer1Input());
        stage->addp1(gameInfo->team1[0]);

        /* for testing, load kfm as player 2 */
       // stage->addp2(loadKfm());
        stage->load();
        return stage;
    } catch (const MugenException & e){
        throw e;
    }
}
#endif


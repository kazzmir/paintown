#include "util/bitmap.h"
#include "mugen/option_versus.h"
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
#include "loading.h"

#include "character.h"
#include "mugen_animation.h"
#include "mugen_background.h"
#include "mugen_item.h"
#include "mugen_item_content.h"
#include "mugen_section.h"
#include "mugen_sound.h"
#include "mugen_reader.h"
#include "mugen_sprite.h"
#include "mugen_util.h"
#include "mugen_font.h"
#include "mugen_storyboard.h"
#include "mugen_menu.h"
#include "mugen_stage.h"
#include "character_select.h"

/* get rid of this at some point */
#include "gui/keyinput_manager.h"

using namespace std;

MugenOptionVersus::MugenOptionVersus(Token *token) throw (LoadException): 
MenuOption(token, Event){
    // notin
}
MugenOptionVersus::MugenOptionVersus( const std::string &name ) throw( LoadException ):
MenuOption(0, Event){
    if (name.empty()){
	throw LoadException("No name given to versus");
    }
    this->setText(name);
}

MugenOptionVersus::~MugenOptionVersus(){
	// Nothing
}

void MugenOptionVersus::logic(){
}

static Mugen::Character * loadKfm(){
    Mugen::Character * kfm = new Mugen::Character("kfm");
    kfm->load();
    return kfm;
}

static InputMap<Mugen::Command::Keys> getPlayer1Input(){
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

void MugenOptionVersus::runGame(MugenStage * stage, const Bitmap & screen){
    InputMap<int> gameInput;
    gameInput.set(Keyboard::Key_F1, 10, false, 0);
    gameInput.set(Keyboard::Key_F2, 10, false, 1);
    gameInput.set(Keyboard::Key_F3, 10, false, 2);
    gameInput.set(Keyboard::Key_F4, 10, true, 3);
    
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
                    keyInputManager::update();
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
                    if (gameSpeed < 0.1){
                        gameSpeed = 0.1;
                    }

                    quit |= keyInputManager::keyState(keys::ESC, true );
                }
                Global::speed_counter = 0;
            }

            if (draw){
                stage->render(&work);
                work.Stretch(screen);
                screen.BlitToScreen();
            }

            while (Global::speed_counter == 0){
                Util::rest(1);
                keyInputManager::update();
            }
        }
    } catch (const MugenException &ex){
	Global::debug(0) << "Problem with stage: " << stage->getName() << " Problem was: " << ex.getReason() << endl;
    }
}

MugenStage * MugenOptionVersus::setupStage(Mugen::SelectedChars * gameInfo){
    Bitmap background;
    ((MugenMenu*) getParent())->copyBackground(background);
    background.resize(GFX_X, GFX_Y);

    pthread_t loader;
    try{
        Level::LevelInfo info;
        info.setBackground(&background);
        info.setLoadingMessage("Loading M.U.G.E.N");
        Loader::startLoading(&loader, (void*) &info);

        MugenStage * stage = gameInfo->selectedStage;
        // Load player 1
        gameInfo->team1[0]->load();
        gameInfo->team1[0]->setInput(getPlayer1Input());
        stage->addp1(gameInfo->team1[0]);

        /* for testing, load kfm as player 2 */
        stage->addp2(loadKfm());
        stage->load();
        Loader::stopLoading(loader);
        return stage;
    } catch (const MugenException & e){
        Loader::stopLoading(loader);
        throw e;
    }
}

void MugenOptionVersus::run(bool &endGame){
    Bitmap screen(GFX_X, GFX_Y);
    // Do select screen change back to 2 once finished testing
    int ticker = 0;

    /* an ugly way to get the select info file */
    Mugen::CharacterSelect select(ticker, ((MugenMenu*) getParent())->getSelectInfoFile());
    select.load();
    Mugen::SelectedChars * gameInfo = select.run(getText(), 1, true, &screen);

    if (gameInfo == 0){
        return;
    }

    runGame(setupStage(gameInfo), screen);
}

#include "util/bitmap.h"
#include "mugen/option_versus.h"
#include "util/font.h"
#include "util/token.h"
#include "configuration.h"
#include "menu/menu.h"
#include "menu/menu_global.h"
#include "globals.h"
#include "init.h"
#include "util/keyboard.h"
#include "util/funcs.h"

#include "mugen_animation.h"
#include "mugen_background.h"
#include "mugen_character.h"
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

void MugenOptionVersus::run(bool &endGame){
    Bitmap screen(GFX_X, GFX_Y);
    // Do select screen change back to 2 once finished testing
    MugenSelectedChars *gameInfo = ((MugenMenu *)getParent())->getSelect()->run(getText(), 1, true, &screen);
    
    if (gameInfo == 0)return;
    
    // Load the stage
    try{
	gameInfo->selectedStage->load();
    }catch (MugenException &ex){
	Global::debug(0) << "Problem with stage: " << gameInfo->selectedStage->getName() << " Problem was: " << ex.getReason() << endl;
	return;
    }
    
    Bitmap work(320,240);
    bool quit = false;
    double gameSpeed = 1.0;
    double runCounter = 0;
   
    // Lets reset the stage for good measure
    gameInfo->selectedStage->reset();
    
    while( !quit ){
        bool draw = false;
        
        if ( Global::speed_counter > 0 ){
            runCounter += Global::speed_counter * gameSpeed * Global::LOGIC_MULTIPLIER;
            while (runCounter > 1){
		keyInputManager::update();
		gameInfo->selectedStage->logic();
                runCounter -= 1;
                draw = true;
		
		quit |= keyInputManager::keyState(keys::ESC, true );
            }
            Global::speed_counter = 0;
        }

        if (draw){
            gameInfo->selectedStage->render(&work);
	    work.Stretch(screen);
	    screen.BlitToScreen();
        }

        while (Global::speed_counter == 0){
            Util::rest(1);
            keyInputManager::update();
        }
    }
    
}


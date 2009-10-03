#include "util/bitmap.h"
#include "option_change_mod.h"
#include "menu/menu_global.h"
#include "util/keyboard.h"
#include "util/load_exception.h"
#include "util/token.h"
#include "util/funcs.h"
#include "game.h"
#include "globals.h"

#include "menu/menu.h"

#include <iostream>

using namespace std;

OptionChangeMod::OptionChangeMod(Token *token) throw (LoadException):
MenuOption(token, Event){
	if ( *token != "change-mod" ){
		throw LoadException("Not a change mod");
	}

    setText("Change mod");
}

OptionChangeMod::~OptionChangeMod(){
	// Nothing
}

void OptionChangeMod::logic(){
}

void OptionChangeMod::run(bool &endGame){
}

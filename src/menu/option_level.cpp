#include "menu/option_level.h"
#include "util/token.h"
#include "menu/menu_global.h"
#include "util/funcs.h"
#include <string>

OptionLevel::OptionLevel(Token *token, int * set, int value) throw( LoadException ):
MenuOption(token, Event),
set(set),
value(value){
  // Nothing
}

OptionLevel::~OptionLevel(){
}

void OptionLevel::logic(){
}

/* redo this to not use global state */
void OptionLevel::run(bool &endGame) throw (ReturnException) {
	endGame = true;
    *set = value;
}

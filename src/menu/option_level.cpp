#include "menu/option_level.h"
#include "util/token.h"
#include "menu/menu_global.h"
#include "util/funcs.h"

OptionLevel::OptionLevel(Token *token) throw( LoadException ):
MenuOption(token, Event){
  // Nothing
}

OptionLevel::~OptionLevel(){
}

void OptionLevel::logic(){
}

/* redo this to not use global state */
void OptionLevel::run(bool &endGame){
	endGame = true;
	MenuGlobals::setLevel(Util::getDataPath() + getText());
}

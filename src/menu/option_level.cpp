#include "menu/option_level.h"
#include "util/token.h"
#include "menu/menu_global.h"
#include "util/funcs.h"
#include <string>

OptionLevel::OptionLevel(Token *token, std::string * set) throw( LoadException ):
MenuOption(token, Event),
update(set){
  // Nothing
}

OptionLevel::~OptionLevel(){
}

void OptionLevel::logic(){
}

/* redo this to not use global state */
void OptionLevel::run(bool &endGame){
	endGame = true;
        *update = getText();
	// MenuGlobals::setLevel(Util::getDataPath() + getText());
}

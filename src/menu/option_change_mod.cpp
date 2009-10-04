#include "util/bitmap.h"
#include "option_change_mod.h"
#include "option_level.h"
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

void OptionChangeMod::run(bool &endGame) throw (ReturnException) {
    try{
        int select = 0;
        // Menu menu(Filesystem::find("menu/change-mod.txt"));
        // menu.addOption(new OptionLevel(0, &select, 0));
        // menu.setupOptions();
        Menu menu;
        // menu.addOption(new OptionLevel(0, &select, 0));
        OptionLevel *opt = new OptionLevel(0, &select, 0);
        opt->setText("Kung foo willy");
        opt->setInfoText("Choose this thing");
        menu.addOption(opt);

        menu.load(Filesystem::find("menu/change-mod.txt"));
        menu.run();
    } catch (const LoadException & le){
        Global::debug(0) << "Could not load menu/change-mod.txt: " << le.getReason() << endl;
    } catch (const Filesystem::NotFound & e){
        Global::debug(0) << "Could not load menu/change-mod.txt: " << e.getReason() << endl;
    }
}

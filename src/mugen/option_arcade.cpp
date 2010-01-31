#include "util/bitmap.h"
#include "mugen/option_arcade.h"
#include "mugen/game.h"
#include "mugen/mugen_menu.h"

using namespace std;
using namespace Mugen;

OptionArcade::OptionArcade(Token *token) throw (LoadException): 
MenuOption(token, Event){
    // notin
}
OptionArcade::OptionArcade( const std::string &name ) throw( LoadException ):
MenuOption(0, Event){
    if (name.empty()){
	throw LoadException("No name given to versus");
    }
    this->setText(name);
}

OptionArcade::~OptionArcade(){
	// Nothing
}

void OptionArcade::logic(){
}

void OptionArcade::run(bool &endGame){
    /* an ugly way to get the select info file */
    Mugen::Game versus(Mugen::Arcade, ((MugenMenu*) getParent())->getSelectInfoFile());
    versus.run();
}

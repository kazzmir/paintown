#include "util/bitmap.h"
#include "mugen/option-versus.h"
#include "mugen/game.h"
#include "mugen/menu.h"

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
    /* an ugly way to get the select info file */
    Mugen::Game versus(Mugen::Versus, ((MugenMenu*) getParent())->getSelectInfoFile());
    versus.run();
}

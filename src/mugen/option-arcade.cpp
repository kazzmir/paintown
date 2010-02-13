#include "util/bitmap.h"
#include "mugen/option-arcade.h"
#include "mugen/game.h"
#include "mugen/menu.h"

using namespace std;
using namespace Mugen;

OptionArcade::OptionArcade( const std::string &name ) throw( LoadException ){
    if (name.empty()){
	throw LoadException("No name given to versus");
    }
    this->setText(name);
}

OptionArcade::~OptionArcade(){
	// Nothing
}

void OptionArcade::executeOption(const Mugen::PlayerType & player, bool &endGame){
    /* an ugly way to get the select info file */
    Mugen::Game versus(Mugen::Arcade, ((MugenMenu*) getParent())->getSelectInfoFile());
    versus.run();
}

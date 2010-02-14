#include "util/bitmap.h"
#include "mugen/option-arcade.h"
#include "mugen/game.h"
#include "mugen/menu.h"

#include "mugen/config.h"

using namespace std;
using namespace Mugen;

OptionArcade::OptionArcade(const string & name){
    if (name.empty()){
	throw LoadException("No name given to versus");
    }

    this->setText(name);
}

OptionArcade::~OptionArcade(){
	// Nothing
}

void OptionArcade::executeOption(const Mugen::PlayerType & player, bool &endGame){
    /* Get default motif system.def */
    Mugen::Game versus(player, Mugen::Arcade, Mugen::Data::getInstance().getFileFromMotif(Mugen::Data::getInstance().getMotif()));
    versus.run();
}

#include "util/bitmap.h"
#include "mugen/option-versus.h"
#include "mugen/game.h"
#include "mugen/menu.h"

using namespace std;
using namespace Mugen;

OptionVersus::OptionVersus( const std::string &name ) throw( LoadException ){
    if (name.empty()){
	throw LoadException("No name given to versus");
    }
    this->setText(name);
}

OptionVersus::~OptionVersus(){
	// Nothing
}

void OptionVersus::executeOption(const Mugen::PlayerType & player, bool &endGame){
    /* an ugly way to get the select info file */
    Mugen::Game versus(Mugen::Versus, ((MugenMenu*) getParent())->getSelectInfoFile());
    versus.run();
}

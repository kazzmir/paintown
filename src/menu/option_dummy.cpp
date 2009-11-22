#include "util/bitmap.h"
#include "menu/option_dummy.h"
#include "util/token.h"
#include "menu/menu.h"
#include "globals.h"
#include "init.h"

using namespace std;

OptionDummy::OptionDummy(Token *token) throw( LoadException ):
MenuOption(token, Event){
    if ( *token != "dummy" ){
        throw LoadException("Not dummy option");
    }

    readName(token);

    if (getText().empty()){
        this->setText("Dummy");
    }
}

OptionDummy::OptionDummy( const std::string &name ) throw( LoadException ):
MenuOption(0, Event){
    if (name.empty()){
	throw LoadException("No name given to dummy");
    }
    this->setText(name);
}

OptionDummy::~OptionDummy(){
}

void OptionDummy::logic(){
}

void OptionDummy::run(bool &endGame){
}

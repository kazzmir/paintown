#include "util/bitmap.h"
#include "menu/option_quit.h"
#include "util/token.h"
#include "menu/menu.h"
#include "globals.h"
#include "init.h"
#include "util/bitmap.h"

using namespace std;

OptionQuit::OptionQuit(Token *token) throw( LoadException ):
MenuOption(token, Event){
    if ( *token != "quit" ){
        throw LoadException("Not quit option");
    }

    readName(token);
}

OptionQuit::OptionQuit( const std::string &name ) throw( LoadException ):
MenuOption(0, Event){
    if (name.empty()){
	throw LoadException("No name given to quit");
    }
    this->setText(name);
}

OptionQuit::~OptionQuit(){
}

void OptionQuit::logic(){
}

void OptionQuit::run(bool &endGame){
	endGame = true;
}

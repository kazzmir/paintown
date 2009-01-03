#include "util/bitmap.h"
#include "menu/option_quit.h"
#include "util/token.h"
#include "menu/menu.h"
#include "globals.h"
#include "init.h"
#include "util/keyboard.h"
#include "util/bitmap.h"

OptionQuit::OptionQuit(Token *token) throw( LoadException ):
MenuOption(token, Event){
	if ( *token != "quit" ){
		throw LoadException("Not quit option");
	}
}

OptionQuit::~OptionQuit(){
}

void OptionQuit::logic(){
	setText( "Quit" );
}

void OptionQuit::draw( Bitmap * work ){
}

void OptionQuit::run(bool &endGame){
	endGame = true;
}

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
	while ( token->hasTokens() ){
		try{
			Token * tok;
			*token >> tok;
			if ( *tok == "name" ){
				// Create an image and push it back on to vector
				std::string temp;
				*tok >> temp;
				this->setText(temp);
			} else {
				Global::debug( 3 ) <<"Unhandled menu attribute: "<<endl;
                                if (Global::getDebug() >= 3){
                                    tok->print(" ");
                                }
			}
		} catch ( const TokenException & ex ) {
			// delete current;
			string m( "Menu parse error: " );
			m += ex.getReason();
			throw LoadException( m );
		} 
	}
	
	if (getText().empty()){
	    this->setText("Quit");
	}
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

#include "menu/option_network_host.h"
#include "util/keyboard.h"
#include "util/load_exception.h"
#include "util/token.h"
#include "util/funcs.h"
#include "util/keyboard.h"
#include "return_exception.h"
#include "network/server.h"
#include "globals.h"

#include <iostream>

using namespace std;

OptionNetworkHost::OptionNetworkHost(Token *token) throw( LoadException ):
MenuOption(token, Event){
	if ( *token != "network-host" ){
		throw LoadException("Not a network-host");
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
	
	if ( getText().empty() ){
		throw LoadException("No name set, this option should have a name!");
	}
}

OptionNetworkHost::~OptionNetworkHost(){
	// Nothing
}

void OptionNetworkHost::logic(){
}

void OptionNetworkHost::draw(Bitmap *work){
}

void OptionNetworkHost::run(bool &endGame){
	Keyboard key;
	Network::networkServer();
	key.clear();
	key.poll();
	key.wait();
}

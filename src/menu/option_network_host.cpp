#include "menu/option_network_host.h"
#include "input/keyboard.h"
#include "util/load_exception.h"
#include "util/token.h"
#include "util/funcs.h"
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

    readName(token);
}

OptionNetworkHost::~OptionNetworkHost(){
	// Nothing
}

void OptionNetworkHost::logic(){
}

void OptionNetworkHost::run(bool &endGame){
	Keyboard key;
        try{
            Network::networkServer(parent);
        } catch (const ReturnException &e){
        }
	key.clear();
	key.poll();
	key.wait();
}

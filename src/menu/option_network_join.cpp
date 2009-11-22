#include "menu/option_network_join.h"
#include "input/keyboard.h"
#include "util/load_exception.h"
#include "util/token.h"
#include "util/funcs.h"
#include "return_exception.h"
#include "network/client.h"
#include "globals.h"

#include <iostream>

using namespace std;

OptionNetworkJoin::OptionNetworkJoin(Token *token) throw( LoadException ):
MenuOption(token, Event){
    if ( *token != "network-join" ){
        throw LoadException("Not a network-join");
    }

    readName(token);
}

OptionNetworkJoin::~OptionNetworkJoin(){
	// Nothing
}

void OptionNetworkJoin::logic(){
}

void OptionNetworkJoin::run(bool &endGame){
	Keyboard key;
	key.poll();
	key.wait();
        try{
            Network::networkClient();
        } catch (const ReturnException &r){
        }

	key.clear();
	key.poll();
	key.wait();
}

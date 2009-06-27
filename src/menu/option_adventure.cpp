#include "util/bitmap.h"
#include "menu/option_adventure.h"
#include "menu/menu_global.h"
#include "util/keyboard.h"
#include "util/load_exception.h"
#include "util/token.h"
#include "util/funcs.h"
#include "level/utils.h"
#include "object/player.h"
#include "object/object.h"
#include "game.h"
#include "globals.h"

#include "menu/menu.h"

#include <iostream>

using namespace std;

OptionAdventure::OptionAdventure(Token *token) throw( LoadException ):
MenuOption(token, Event){
	if ( *token != "adventure" ){
		throw LoadException("Not an adventure");
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

OptionAdventure::~OptionAdventure(){
	// Nothing
}

void OptionAdventure::logic(){
}

void OptionAdventure::run(bool &endGame){
	Keyboard key;
	Object * player = NULL;
	try{
		//string level = Game::selectLevelSet( Util::getDataPath() + "/levels" );
		string level = MenuGlobals::doLevelMenu("/levels");

		if (level.empty()){
                    Global::debug(0) << "*bug* Level name is empty?" << endl;
                    /* throw an error or something */
                    return;
                }
		key.wait();

                Level::LevelInfo info = Level::readLevels(level);
		
		player = Game::selectPlayer(MenuGlobals::getInvincible(), "Pick a player", info.getPlayerPath());
                player->setObjectId(-1);
		((Player *)player)->setLives( MenuGlobals::getLives() );
		vector< Object * > players;
		players.push_back( player );
		Game::realGame(players, info);
	} catch ( const LoadException & le ){
		Global::debug( 0 ) << "Error while loading: " << le.getReason() << endl;
	} catch ( const ReturnException & r ){
		key.wait();
	}

	if ( player != NULL ){
		delete player;
	}
}

#include "menu/option_adventure_cpu.h"
#include "util/token.h"
#include "menu/menu.h"
#include "menu/menu_global.h"
#include "game.h"
#include "globals.h"
#include "object/object.h"
#include "object/player.h"
#include "object/buddy_player.h"
#include "util/funcs.h"
#include "util/keyboard.h"

using namespace std;

OptionAdventureCpu::OptionAdventureCpu(Token *token) throw( LoadException ):
MenuOption(event){
	if ( *token != "adventure-cpu" ){
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
				tok->print(" ");
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

OptionAdventureCpu::~OptionAdventureCpu(){
}

void OptionAdventureCpu::logic(){
}

void OptionAdventureCpu::draw(Bitmap *work){
}

void OptionAdventureCpu::run(bool &endGame){
	bool isInvincible = false;
	int startingLives = 4;
	int max_buddies = 1;

	Keyboard key;
	Object * player = NULL;
	vector< Object * > buddies;
	try{
		string level = Game::selectLevelSet( Util::getDataPath() + "/levels" );
		key.wait();

		player = Game::selectPlayer( isInvincible, "Pick a player" );
		((Player *)player)->setLives( startingLives );
		vector< Object * > players;
		players.push_back( player );

		for ( int i = 0; i < max_buddies; i++ ){
			Object * b = Game::selectPlayer( isInvincible, "Pick a buddy" );
			buddies.push_back( b );
			Object * buddy = new BuddyPlayer( (Character *) player, *(Character *) b );
			buddies.push_back( buddy );
			players.push_back( buddy );
		}
		Game::realGame( players, level );
	} catch ( const LoadException & le ){
		Global::debug( 0 ) << "Could not load player: " << le.getReason() << endl;
	} catch ( const ReturnException & r ){
		key.wait();
	}
	if ( player != NULL ){
		delete player;
	}
	for ( vector< Object * >::iterator it = buddies.begin(); it != buddies.end(); it++ ){
		delete *it;
	}
}

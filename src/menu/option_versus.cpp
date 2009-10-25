#include "menu/option_versus.h"
#include "game.h"
#include "globals.h"
#include "util/token.h"
#include "input/keyboard.h"
#include "util/load_exception.h"
#include "level/utils.h"
#include "object/versus_player.h"
#include "object/versus_enemy.h"

using namespace std;

OptionVersus::OptionVersus( Token *token ) throw( LoadException ):
MenuOption(token, Event), human(false){
	if ( *token != "versus" ){
		throw LoadException("Not versus");
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
			} else if( *tok == "cpu" ) {
				human = false;
			} else if( *tok == "human" ) {
				human = true;
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
		} catch ( const LoadException & ex ) {
			// delete current;
			throw ex;
		}
	}
	
	if ( getText().empty() ){
		throw LoadException("No name set, this option should have a name!");
	}
}

OptionVersus::~OptionVersus(){
}

void OptionVersus::logic(){
}

void OptionVersus::run(bool &endGame){
	/*
	Keyboard key;
	Object * player = NULL;
	Object * enemy = NULL;
	try{
		if(human) 
		{
			vector<Object *>temp = versusSelect( false );
			player = temp[0];
			enemy = temp[1];
			if(!player || !enemy)throw LoadException("Null player");
			//player = selectPlayer( false );
			//enemy = selectPlayer( false );
			enemy->setAlliance( ALLIANCE_ENEMY );
			
			for ( int i = 0; i < 3; i += 1 ){
				VersusPlayer en( 1, *(Player *) enemy );
				VersusPlayer pl( 0, *(Player *) player );
				playVersusMode( &pl, &en, i + 1 );
			}
		}
		else
		{
			player = selectPlayer( false );
			enemy = selectPlayer( false );
			enemy->setAlliance( ALLIANCE_ENEMY );
			
			for ( int i = 0; i < 3; i += 1 ){
				VersusEnemy en( *(Player *) enemy );
				VersusPlayer pl( 0, *(Player *) player );
				playVersusMode( &pl, &en, i + 1 );
			}
		}
		key.wait();
	} catch ( const LoadException & le ){
		Global::debug( 0 ) << "Could not load player: " << le.getReason() << endl;
	} catch ( const ReturnException & r ){
		key.wait();
	}
	*/

	Keyboard key;
	Object * player = NULL;
	Object * enemy = NULL;
	try{
		if ( human ){
			vector<Object *>temp = Game::versusSelect( false );
			player = temp[0];
			enemy = temp[1];
			if ( ! player || !enemy ){
				throw LoadException("Null player");
			}

			//player = selectPlayer( false );
			//enemy = selectPlayer( false );
			enemy->setAlliance( ALLIANCE_ENEMY );

			for ( int i = 0; i < 3; i += 1 ){
				VersusPlayer en(*(Player *) enemy );
				VersusPlayer pl(*(Player *) player );
                                en.setConfig(1);
                                pl.setConfig(0);
				Game::playVersusMode( &pl, &en, i + 1 );
			}
		} else {
                    Level::LevelInfo info;
			player = Game::selectPlayer( false, "Pick your player", info);
			enemy = Game::selectPlayer( false, "Pick enemy", info);
			enemy->setAlliance( ALLIANCE_ENEMY );

			for ( int i = 0; i < 3; i += 1 ){
				VersusEnemy en( *(Player *) enemy );
				VersusPlayer pl( *(Player *) player );
                                pl.setConfig(0);
				Game::playVersusMode( &pl, &en, i + 1 );
			}
		}
		key.wait();
	} catch ( const LoadException & le ){
		Global::debug( 0 ) << "Could not load player: " << le.getReason() << endl;
	} catch ( const ReturnException & r ){
		key.wait();
	}

	if ( player != NULL ){
		delete player;
	}
	if ( enemy != NULL ){
		delete enemy;
	}
}

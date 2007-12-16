#include "server.h"
#include "util/keyboard.h"
#include "util/bitmap.h"
#include "util/funcs.h"
#include "globals.h"
#include "object/object.h"
#include "object/character.h"
#include "object/player.h"
#include "init.h"
#include "chat_server.h"
#include "music.h"
#include "network_world.h"
#include "object/network_character.h"
#include "level/utils.h"
#include "util/font.h"
#include "select_player.h"
#include "factory/heart_factory.h"
#include "factory/object_factory.h"
#include "return_exception.h"
#include <sstream>

namespace Network{

static const char * DEFAULT_FONT = "/fonts/arial.ttf";

/*
static void showTitleScreen(){
	Bitmap::Screen->Blit( Util::getDataPath() + "/paintown-title.png" );
}
*/

static void fadeOut( const string & str ){
	/* fill in */
}

static void startLoading( pthread_t * p ){
	/* fill in */
}

static void stopLoading( pthread_t p ){
}

static int playLevel( const World & w, vector< Object * > players, int time ){
	return 0;
}

static void networkSendLevel( const vector< NLsocket > & sockets, string level ){
	level.erase( 0, Util::getDataPath().length() );
	for ( vector< NLsocket >::const_iterator it = sockets.begin(); it != sockets.end(); it++ ){
		NLsocket socket = *it;
		Network::send16( socket, level.length() + 1 );
		Network::sendStr( socket, level );
		/*
		nlWrite( socket, buf, sizeof(uint16_t) );
		nlWrite( socket, level.c_str(), level.length() + 1 );
		*/
	}
}

static const string selectLevelSet( const string & base ) throw( ReturnException ){
	return "";
}

static int getServerPort(){
	const int drawY = 160;
	{
		Bitmap background( Util::getDataPath() + "/paintown-title.png" );
		background.BlitToScreen();
		const Font & font = Font::getFont( Util::getDataPath() + DEFAULT_FONT, 20, 20 );
		font.printf( 40, drawY, Bitmap::makeColor( 255, 255, 255 ), *Bitmap::Screen, "Port:", 0 );
	}

	Keyboard key;
	key.setAllDelay( 90 );
	key.setDelay( Keyboard::Key_BACKSPACE, 30 );
	key.setDelay( Keyboard::Key_ESC, 0 );

	Bitmap work( 200, 25 );

	char buffer[ 10 ];
	unsigned int index = 0;
	Global::speed_counter = 0;
	bool done = false;
	vector< int > pressed;
	pressed.push_back( Keyboard::Key_7 );
	pressed.push_back( Keyboard::Key_8 );
	pressed.push_back( Keyboard::Key_8 );
	pressed.push_back( Keyboard::Key_7 );
	while ( ! done ){

		while ( Global::speed_counter > 0 ){
			Global::speed_counter -= 1;
			key.readKeys( pressed );
			if ( key[ Keyboard::Key_ESC ] ){
				throw ReturnException();
			}
		}

		if ( pressed.size() > 0 ){
			for ( vector< int >::iterator it = pressed.begin(); it != pressed.end(); it++ ){
				int xkey = *it;
				// Global::debug( 0 ) << "Pressed " << Keyboard::keyToName( key ) << endl;
				if ( xkey == Keyboard::Key_BACKSPACE ){
					if ( index > 0 ){
						index -= 1;
					}
					buffer[ index ] = 0;
			   } else if ( xkey == Keyboard::Key_ENTER ){
					done = true;
					key.wait();
				} else if ( Keyboard::isNumber( xkey ) ){
					const char * name = Keyboard::keyToName( xkey );
					buffer[ index ] = name[ 0 ];
					index += 1;
					if ( index > sizeof(buffer) / sizeof(char) - 1 ){
						index = sizeof(buffer) / sizeof(char) - 1;
					}
					buffer[ index ] = 0;
				}
			}
			pressed.clear();
			work.clear();
			const Font & font = Font::getFont( Util::getDataPath() + DEFAULT_FONT, 20, 20 );
			font.printf( 0, 0, Bitmap::makeColor( 255, 255, 255 ), work, buffer, 0 );
			work.Blit( 100, drawY, *Bitmap::Screen );
		}

		while ( Global::speed_counter == 0 ){
			Util::rest( 1 );
			key.poll();
		}
	}

	istringstream str( buffer );
	int i;
	str >> i;
	return i;
}

static void networkGame( const vector< Object * > & players, const string & levelFile, const vector< NLsocket > & sockets ){

	vector< string > levels = Level::readLevels( levelFile );

	// global_debug = true;

	int showHelp = 300;
	for ( vector< string >::iterator it = levels.begin(); it != levels.end(); it++ ){
		Global::done_loading = false;
		pthread_t loading_screen_thread;

		startLoading( &loading_screen_thread );

		networkSendLevel( sockets, *it );

		bool gameState = false;
		try {
			// vector< Object * > players;
			// players.push_back( player );
			NetworkWorld world( sockets, players, *it );

			Music::pause();
			Music::fadeOut( 0.3 );
			Util::rest( 100 );
			Music::fadeIn( 0.3 );
			Music::loadSong( Util::getFiles( Util::getDataPath() + "/music/", "*" ) );
			Music::play();

			for ( vector< Object * >::const_iterator it = players.begin(); it != players.end(); it++ ){
				Player * playerX = (Player *) *it;
				playerX->setY( 200 );
				/* setMoving(false) sets all velocities to 0 */
				playerX->setMoving( false );
				/* but the player is falling so set it back to true */
				playerX->setMoving( true );

				playerX->setStatus( Status_Falling );
				world.addMessage( playerX->movedMessage() );
			}

			stopLoading( loading_screen_thread );

			// nlDisable( NL_BLOCKING_IO );
			gameState = playLevel( world, players, showHelp );
			// nlEnable( NL_BLOCKING_IO );
			showHelp = 0;

			world.addMessage( world.finishMessage() );
			world.flushOutgoing();

		} catch ( const LoadException & le ){
			Global::debug( 0 ) << "Could not load " << *it << " because " << le.getReason() << endl;
			/* if the level couldn't be loaded turn off
			 * the loading screen
			 */
			stopLoading( loading_screen_thread );
		}

		ObjectFactory::destroy();
		HeartFactory::destroy();

		if ( ! gameState ){
			return;
		}

		fadeOut( "Next level" );
	}

	for ( vector< NLsocket >::const_iterator it = sockets.begin(); it != sockets.end(); it++ ){
		NLsocket s = *it;
		Network::send16( s, -1 );
	}

	fadeOut( "You win!" );
}

void networkServer(){

	const int startingLives = 4;
	int port = getServerPort();

	Keyboard key;

	Global::debug( 0 ) << "Port " << port << endl;

	Network::Socket server = Network::open( port );
	// NLsocket server = nlOpen( port, NL_RELIABLE_PACKETS );
	if ( server == NL_INVALID ){
		Global::debug( 0 ) << "hawknl error: " << nlGetSystemErrorStr( nlGetSystemError() ) << endl;
		throw ReturnException();
	}

	ChatServer chat( server );
	chat.run();
	return;

	Object * player = NULL;
	try{
		Global::showTitleScreen();

		const Font & font = Font::getFont( Util::getDataPath() + DEFAULT_FONT, 20, 20 );
		font.printf( 100, 200, Bitmap::makeColor( 255, 255, 255 ), *Bitmap::Screen, "Waiting for a connection", 0 );
		nlListen( server );
		NLsocket client = nlAcceptConnection( server );
		while ( client == NL_INVALID ){
			Util::rest( 1 );
			client = nlAcceptConnection( server );
		}
		NLint group;
		group = nlGroupCreate();
		nlGroupAddSocket( group, client );

		NLaddress client_addr;
		nlGetRemoteAddr( client, &client_addr );

		Global::debug( 0 ) << "client is " << client << " port " << nlGetPortFromAddr( &client_addr ) << endl;

		// NLsocket polled;
		// nlPollGroup( group, NL_READ_STATUS, &polled, 1, -1 );
		int length = Network::read16( client );
		string clientPath = Network::readStr( client, length );

		// NetworkWorld world( port );
		string level = selectLevelSet( Util::getDataPath() + "/levels" );
		key.wait();

		vector< NLsocket > sockets;
		sockets.push_back( client );

		player = selectPlayer( false, "Pick a player" );
		Global::debug( 0 ) << "Player path '" << ((Character *)player)->getPath() << "'" << endl;
		player->setId( 1 );
		((Character *)player)->setLives( startingLives );
		vector< Object * > players;
		players.push_back( player );
		Character * client_character = new NetworkCharacter( Util::getDataPath() + clientPath, ALLIANCE_PLAYER );
		client_character->setLives( startingLives );
		client_character->setId( 2 );
		Network::send16( client, 2 );
		players.push_back( client_character );
		networkGame( players, level, sockets );

	} catch ( const LoadException & le ){
		Global::debug( 0 ) << "Could not load player: " << le.getReason() << endl;
	} catch ( const ReturnException & r ){
		// key.wait();
	} catch ( const Network::NetworkException & e ){
		Global::debug( 0 ) << "Network exception: " << e.getMessage() << endl;
	}

	if ( player != NULL ){
		delete player;
	}
	nlClose( server );
}

}

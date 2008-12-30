#include "util/bitmap.h"

#include "chat_server.h"
#include "factory/font_render.h"
#include "factory/heart_factory.h"
#include "factory/object_factory.h"
#include "game.h"
#include "globals.h"
#include "init.h"
#include "level/utils.h"
#include "loading.h"
#include "music.h"
#include "network_world.h"
#include "object/character.h"
#include "object/network_character.h"
#include "object/network_player.h"
#include "object/object.h"
#include "object/player.h"
#include "return_exception.h"
#include "server.h"
#include <sstream>
#include "util/font.h"
#include "util/funcs.h"
#include "util/keyboard.h"

namespace Network{

static std::ostream & debug( int level ){
	Global::debug( level ) << "[server] ";
	return Global::debug( level );
}

// static const char * DEFAULT_FONT = "/fonts/arial.ttf";

/*
static void showTitleScreen(){
	Bitmap::Screen->Blit( Util::getDataPath() + "/paintown-title.png" );
}
*/

static void stopLoading( pthread_t thread ){
	pthread_mutex_lock( &Global::loading_screen_mutex );
	Global::done_loading = true;
	pthread_mutex_unlock( &Global::loading_screen_mutex );

	pthread_join( thread, NULL );
}

static void startLoading( pthread_t * thread ){
	pthread_create( thread, NULL, loadingScreen, NULL );
}

#if 0
static void fadeOut( const string & str ){
	/* fill in */
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
#endif

#if 0
static const string selectLevelSet( const string & base ) throw( ReturnException ){
	Bitmap::Screen->Blit( Util::getDataPath() + "/paintown-title.png" );
	int fontY = 20;
	const Font & font = Font::getFont( Util::getDataPath() + Global::DEFAULT_FONT, 20, fontY );
	vector< string > possible = Util::getFiles( base + "/", "*.txt" );
	if ( possible.size() == 0 ){
		return "no-files!!!";
	}

	/*
	for ( vector< string >::iterator it = possible.begin(); it != possible.end(); it++ ){
		string & s = *it;
		s.insert( 0, base + "/" );
	}
	*/
	int choose = 0;

	font.printf( 180, (int)(200 - fontY * 1.2), Bitmap::makeColor( 255, 255, 255 ), *Bitmap::Screen, "Select a set of levels to play", 0 );
	for ( unsigned int i = 0; i < possible.size(); i++ ){
		int yellow = Bitmap::makeColor( 255, 255, 0 );
		int white = Bitmap::makeColor( 255, 255, 255 );
		unsigned int color = i == (unsigned) choose ? yellow : white;
		font.printf( 200, (int)(200 + i * fontY * 1.2), color, *Bitmap::Screen, possible[ i ], 0 );
	}

	Keyboard key;
	bool done = false;

	int LAZY_KEY_DELAY = 200;
	key.setDelay( Keyboard::Key_UP, LAZY_KEY_DELAY );
	key.setDelay( Keyboard::Key_DOWN, LAZY_KEY_DELAY );
	Global::speed_counter = 0;

	while ( ! done ){
		
		key.poll();
		bool draw = false;
		if ( Global::speed_counter > 0 ){
			double think = Global::speed_counter;

			while ( think > 0 ){
				think--;

				if ( key[ Keyboard::Key_UP ] ){
					draw = true;
					choose = (choose - 1 + possible.size()) % possible.size();
				}

				if ( key[ Keyboard::Key_DOWN ] ){
					draw = true;
					choose = (choose + 1 + possible.size()) % possible.size();
				}

				if ( key[ Keyboard::Key_ENTER ] ){
					return possible[ choose ];
				}

				if ( key[ Keyboard::Key_ESC ] ){
					throw ReturnException();
				}
			}

			Global::speed_counter = 0;
		}

		if ( draw ){
			for ( unsigned int i = 0; i < possible.size(); i++ ){
				int yellow = Bitmap::makeColor( 255, 255, 0 );
				int white = Bitmap::makeColor( 255, 255, 255 );
				unsigned int color = i == (unsigned) choose ? yellow : white;
				font.printf( 200, (int)(200 + i * fontY * 1.2), color, *Bitmap::Screen, possible[ i ], 0 );
			}		
		}
		
		while ( Global::speed_counter == 0 ){
			Util::rest( 1 );
			key.poll();
		}
	}

	return "nothing-selected";
}
#endif

static int getServerPort(){
    Bitmap background( Global::titleScreen() );
	const int drawY = 250;
	{
		// background.BlitToScreen();
		const Font & font = Font::getFont( Util::getDataPath() + Global::DEFAULT_FONT, 20, 20 );
		Bitmap black( 300, font.getHeight() * 4 );
		black.clear();
		black.border( 0, 1, Bitmap::makeColor( 255, 255, 255 ) );
		Bitmap::transBlender( 0, 0, 0, 92 );
		black.drawTrans( 20, drawY - font.getHeight() - 20, background );
		font.printf( 40, drawY, Bitmap::makeColor( 255, 255, 255 ), background, "Port:", 0 );
		font.printf( 40, drawY - font.getHeight() - 5, Bitmap::makeColor( 255, 255, 255 ), background, "Enter to start. ESC to quit", 0 );
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
			const Font & font = Font::getFont( Util::getDataPath() + Global::DEFAULT_FONT, 20, 20 );
			font.printf( 0, 0, Bitmap::makeColor( 255, 255, 255 ), work, buffer, 0 );
			work.Blit( 100, drawY, background );
                        background.BlitToScreen();
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

#if 0
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
#endif

static void sendToAll( const vector< Socket > & sockets, const Message & message ){
	for ( vector< Socket >::const_iterator it = sockets.begin(); it != sockets.end(); it++ ){
		const Socket & socket = *it;
		message.send( socket );
	}
}

static Network::Message removeMessage( int id ){
	Network::Message message;

	message.id = 0;
	message << World::REMOVE;
	message << id;

	return message;
}

static void playLevel( World & world, const vector< Object * > & players ){
	Keyboard key;
	
	key.setDelay( Keyboard::Key_F2, 100 );
	key.setDelay( Keyboard::Key_F12, 50 );

	key.setDelay( Keyboard::Key_MINUS_PAD, 2 );
	key.setDelay( Keyboard::Key_PLUS_PAD, 2 );
	key.setDelay( Keyboard::Key_P, 100 );

	key.setDelay( Keyboard::Key_F4, 200 );

	key.setDelay( Keyboard::Key_F8, 300 );
	key.setDelay( Keyboard::Key_TAB, 300 );
	
	/* the game graphics are meant for 320x240 and will be stretched
	 * to fit the screen
	 */
	Bitmap work( 320, 240 );
	// Bitmap work( GFX_X, GFX_Y );
	Bitmap screen_buffer( GFX_X, GFX_Y );

	Global::speed_counter = 0;
	Global::second_counter = 0;
	int game_time = 100;
	bool done = false;

	double gameSpeed = 1.0;
	
	double runCounter = 0;
	while ( ! done ){

		bool draw = false;
		key.poll();

		if ( Global::speed_counter > 0 ){
			runCounter += Global::speed_counter * gameSpeed * Global::LOGIC_MULTIPLIER;

			while ( runCounter >= 1.0 ){
				draw = true;
				world.act();
				runCounter -= 1.0;

				if ( key[ Keyboard::Key_ESC ] ){
					throw ReturnException();
				}

				for ( vector< Object * >::const_iterator it = players.begin(); it != players.end(); it++ ){
					Character * player = (Character *) *it;
					if ( player->getHealth() <= 0 ){
						if ( player->spawnTime() == 0 ){
							player->deathReset();
							world.addMessage( removeMessage( player->getId() ) );
							world.addObject( player );
							world.addMessage( player->getCreateMessage() );
							world.addMessage( player->movedMessage() );
							world.addMessage( player->animationMessage() );
						}
					}
				}
			}

			if ( key[ Keyboard::Key_TAB ] ){
				world.drawMiniMaps( ! world.shouldDrawMiniMaps() );
			}

			Global::speed_counter = 0;
		}
		
		while ( Global::second_counter > 0 ){
			game_time--;
			Global::second_counter--;
			if ( game_time < 0 )
				game_time = 0;
		}
	
		if ( draw ){
			world.draw( &work );

			work.Stretch( screen_buffer );
			FontRender * render = FontRender::getInstance();
			render->render( &screen_buffer );
	
			// const Font & font = Font::getFont( Util::getDataPath() + DEFAULT_FONT, 20, 20 );

			/* getX/Y move when the world is quaking */
			screen_buffer.BlitToScreen( world.getX(), world.getY() );

			/*
			if ( key[ Keyboard::Key_F12 ] ){
				string file = findNextFile( "scr.bmp" );
				Global::debug( 2 ) << "Saved screenshot to " << file << endl;
				work.save( file );
			}
			*/

			work.clear();
		}

		while ( Global::speed_counter < 1 ){
			Util::rest( 1 );
			key.poll();
		}

		done |= world.finished();
	}

	if ( key[ Keyboard::Key_ESC ] ){
		while ( key[ Keyboard::Key_ESC ] ){
			key.poll();
			Util::rest( 1 );
		}
	}
}

static void playGame( const vector< Socket > & sockets ){
	vector< Object * > players;
	pthread_t loading_screen_thread;
	try{
		Object * player = Game::selectPlayer( false, "Pick a player" );
		players.push_back( player );
		string levelSet = Game::selectLevelSet( Util::getDataPath() + "/levels" );
		

		startLoading( &loading_screen_thread );

		int id = 1;
		player->setId( id );
		id += 1;
		for ( vector< Socket >::const_iterator it = sockets.begin(); it != sockets.end(); it++ ){
			const Socket & socket = *it;
			debug( 1 ) << "Read character path from " << id << endl;
			Message message( socket );
			int type;
			message >> type;
			if ( type == World::CREATE_CHARACTER ){
				Character * client_character = new NetworkPlayer( Util::getDataPath() + message.path, ALLIANCE_PLAYER );
				((NetworkCharacter *)client_character)->alwaysShowName();
				players.push_back( client_character );
				client_character->setLives( 1 );
				client_character->setId( id );
				Message clientId;
				clientId << World::SET_ID;
				clientId << id;
				clientId.send( socket );
				id += 1;
			} else {
				debug( 0 ) << "Got a bogus message: " << type << endl;
			}
		}

		for ( vector< Object * >::iterator it = players.begin(); it != players.end(); it++ ){
			Character * c = (Character *) *it;
			string path = c->getPath();
			path.erase( 0, Util::getDataPath().length() );
			Message add;
			add << World::CREATE_CHARACTER;
			add << c->getId();
			add.path = path;
			sendToAll( sockets, add );
		}

		Message addServer;
		addServer << World::CREATE_CHARACTER;
		addServer << player->getId();
		addServer << ((Character *)player)->getPath().substr( Util::getDataPath().length() );
		sendToAll( sockets, addServer );

		vector< string > levels = Level::readLevels( levelSet );
		for ( vector< string >::iterator it = levels.begin(); it != levels.end(); it++ ){
			string level = *it;
			debug( 1 ) << "Sending level '" << level << "'" << endl;
			Message loadLevel;
			loadLevel << World::LOAD_LEVEL;
			loadLevel.path = level;
			sendToAll( sockets, loadLevel );

			for ( vector< Object * >::const_iterator it = players.begin(); it != players.end(); it++ ){
				Player * playerX = (Player *) *it;
				playerX->setY( 200 );
				/* setMoving(false) sets all velocities to 0 */
				playerX->setMoving( false );
				/* but the player is falling so set it back to true */
				playerX->setMoving( true );

				playerX->setStatus( Status_Falling );
			}

			debug( 1 ) << "Create network world" << endl;
			NetworkWorld world( sockets, players, level );

			debug( 1 ) << "Load music" << endl;

			Music::pause();
			Music::fadeIn( 0.3 );
			Music::loadSong( Util::getFiles( Util::getDataPath() + "/music/", "*" ) );
			Music::play();

			stopLoading( loading_screen_thread );
			playLevel( world, players );
			world.stopRunning();
			Message finish;
			finish << World::FINISH;
			finish.id = 0;
			sendToAll( sockets, finish );

			for ( vector< Socket >::const_iterator it = sockets.begin(); it != sockets.end(); it++ ){
				Socket s = *it;
				bool done = false;
				while ( ! done ){
					Message ok( s );
					int type;
					ok >> type;
					if ( type == World::OK ){
						done = true;
					}
				}

				Message ok;
				ok << World::OK;
				ok.send( s );
			}
			startLoading( &loading_screen_thread );
		}
		Network::Message gameOver;
		gameOver.id = 0;
		gameOver << World::GAME_OVER;
		sendToAll( sockets, gameOver );

	} catch ( const LoadException & le ){
		debug( 0 ) << "Load exception: " + le.getReason() << endl;
	} catch ( const ReturnException & re ){
	} catch ( const NetworkException & ne ){
		debug( 0 ) << "Network excetion: " + ne.getMessage() << endl;
	}

	for ( vector< Object * >::iterator it = players.begin(); it != players.end(); it++ ){
		delete *it;
	}

	for ( vector< Socket >::const_iterator it = sockets.begin(); it != sockets.end(); it++ ){
		Network::close( *it );
	}

	stopLoading( loading_screen_thread );
}

static void popup( const Font & font, const string & message ){
	int length = font.textLength( message.c_str() ) + 20; 
        Bitmap background( Global::titleScreen() );
	Bitmap area( background, background.getWidth() / 2 - length / 2, 220, length, font.getHeight() * 3 );
	Bitmap::transBlender( 0, 0, 0, 128 );
	area.drawingMode( Bitmap::MODE_TRANS );
	area.rectangleFill( 0, 0, area.getWidth(), area.getHeight(), Bitmap::makeColor( 64, 0, 0 ) );
	area.drawingMode( Bitmap::MODE_SOLID );
	int color = Bitmap::makeColor( 255, 255, 255 );
	area.rectangle( 0, 0, area.getWidth() - 1, area.getHeight() - 1, color );
	font.printf( 10, area.getHeight() / 2, Bitmap::makeColor( 255, 255, 255 ), area, message, 0 );
        background.BlitToScreen();
}

void networkServer(){

	// const int startingLives = 4;
	int port = getServerPort();

	Keyboard key;

	debug( 1 ) << "Port is " << port << endl;

	const Font & font = Font::getFont( Util::getDataPath() + Global::DEFAULT_FONT, 20, 20 );
	try{
		/*
#ifdef _WIN32
		Network::blocking( false );
#endif
*/
		debug( 1 ) << "Get socket" << endl;
		Network::Socket server = Network::open( port );
		
		/*
		// NLsocket server = nlOpen( port, NL_RELIABLE_PACKETS );
		if ( server == NL_INVALID ){
			Global::debug( 0 ) << "hawknl error: " << nlGetSystemErrorStr( nlGetSystemError() ) << endl;
			throw ReturnException();
		}
		*/
		
		debug( 1 ) << "Run chat server" << endl;

		ChatServer chat( "server", server );
		chat.run();
		/*
#ifdef _WIN32
		Network::blocking( true );
#endif
*/
		vector< Network::Socket > sockets = chat.getConnectedClients();
		if ( ! sockets.empty() ){
			debug( 1 ) << "Start game with " << sockets.size() << " clients" << endl;
			playGame( sockets );
		} else {
			key.poll();
			popup( font, "No clients connected" );
			key.wait();
			key.readKey();
		}
		Network::close( server );
	} catch ( const NetworkException & ne ){
		debug( 0 ) << "Network error: " << ne.getMessage() << endl;
		key.poll();
		popup( font, "Network error: " + ne.getMessage() );
		key.wait();
		key.readKey();
	}
	return;

#if 0
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
#endif
}

}

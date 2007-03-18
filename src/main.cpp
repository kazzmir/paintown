#ifdef WINDOWS
#define BITMAP dummyBITMAP
#include <windows.h>
#undef BITMAP
#endif

#include "main.h"
#include "init.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>

#include "factory/collector.h"
#include "factory/object_factory.h"
#include "factory/heart_factory.h"
#include "factory/font_render.h"
#include "fonts.h"
#include "globals.h"
#include "level/scene.h"
#include "loading.h"
#include "music.h"
#include "object/character.h"
#include "object/effect.h"
#include "object/enemy.h"
#include "object/object.h"
#include "object/object_attack.h"
#include "object/player.h"
#include "util/bitmap.h"
#include "util/funcs.h"
#include "util/load_exception.h"
#include "util/token.h"
#include "util/token_exception.h"
#include "util/tokenreader.h"
#include "util/timedifference.h"
#include "world.h"

#include <pthread.h>

/* Global effect for copying */
// static Object * bang = NULL;

#ifndef debug
#define debug cout<<"File: "<<__FILE__<<" Line: "<<__LINE__<<endl;
// #define debug
#endif

/*
void resize_callback( int w, int h, void * data ){
	*(bool *)data = true;
}
*/

/* fade the screen and tell the player they lost */
void fadeOut( const string & message ){
	Bitmap dark( GFX_X, GFX_Y );
	dark.fill( Bitmap::makeColor( 0, 0, 0 ) );
	Global::speed_counter = 0;
	Bitmap::transBlender( 0, 0, 0, 10 );
	int fade = 0;
	const Font & f = Font::getFont( "data/fonts/arial.ttf", 50, 50 );
	f.printf( 200, 200, Bitmap::makeColor( 255, 0, 0 ), dark, message );
	while ( fade < 100 ){
		fade++;
	
		Bitmap::transBlender( 0, 0, 0, fade / 2 );

		bool draw = false;
		if ( Global::speed_counter > 0 ){
			draw = true;
			Global::speed_counter = 0;
		}

		if ( draw ){
			dark.drawTrans( 0, 0, *Bitmap::Screen );
		}
		
		while ( Global::speed_counter == 0 ){
			Util::rest( 1 );
		}
	}

	Util::rest( 1000 );
}

static bool playLevel( World & world, Player * player ){
	Keyboard key;
	
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
	
	while ( ! done ){

		bool draw = false;
		key.poll();

		if ( Global::speed_counter >= gameSpeed ){
			double think = Global::speed_counter;
			double lose = think;
			while ( think >= gameSpeed ){
				draw = true;
				world.act();
				think -= gameSpeed;

				if ( player->getHealth() <= 0 ){
					player->deathReset();
					if ( player->getLives() == 0 ){
						fadeOut( "You lose" );
						return false;
					}
					world.addObject( player );
				}
				/*
				if ( key[ Keyboard::Key_SPACE ] || playerX->getLives() <= 0 ){
					fadeOut();
					return;
				}
				*/

				if ( key[ Keyboard::Key_MINUS_PAD ] ){
					gameSpeed -= 0.01;
					if ( gameSpeed < 0.01 ){
						gameSpeed = 0.01;
					}
				}

				if ( key[ Keyboard::Key_PLUS_PAD ] ){
					gameSpeed += 0.01;
				}
			}

			Global::speed_counter -= lose;
		}
		
		while ( Global::second_counter > 0 ){
			game_time--;
			Global::second_counter--;
			if ( game_time < 0 )
				game_time = 0;
		}
	
		if ( draw ){
			world.draw( &work );

			// work.printf( 180, 1, Bitmap::makeColor(255,255,255), (FONT *)all_fonts[ JOHNHANDY_PCX ].dat, "%d", game_time );

			work.Stretch( screen_buffer );
			FontRender * render = FontRender::getInstance();
			render->render( &screen_buffer );

			/* getX/Y move when the world is quaking */
			screen_buffer.Blit( world.getX(), world.getY(), *Bitmap::Screen );

			if ( key[ Keyboard::Key_F12 ] ){
				work.save( "scr.bmp" );
			}

			work.clear();
		}

		while ( Global::speed_counter < gameSpeed ){
			Util::rest( 1 );
			key.poll();
		}

		done |= key[ Keyboard::Key_ESC ] || world.finished();
	}

	if ( key[ Keyboard::Key_ESC ] ){
		while ( key[ Keyboard::Key_ESC ] ){
			key.poll();
			Util::rest( 1 );
		}
		return false;
	}

	return true;
}

static vector< string > readLevels( const string & filename ){
	try{
		TokenReader tr( filename );
		Token * head = tr.readToken();

		vector< string > levels;
		if ( *head == "levels" ){
			while ( head->hasTokens() ){
				string s;
				*head >> s;
				levels.push_back( s );
			}
		}

		return levels;
	} catch ( const LoadException & lex ){
		cout << "Could not load " << filename << ". Reason: " << lex.getReason() << endl;
		return vector< string >();
	}
}

void realGame( Object * player ){

	vector< string > levels = readLevels( "data/levels/w1.txt" );

	// global_debug = true;
	bool show_loading_screen = true;

	if ( show_loading_screen ){
		pthread_mutex_init( &Global::loading_screen_mutex, NULL );
	}

	for ( vector< string >::iterator it = levels.begin(); it != levels.end(); it++ ){
		Global::done_loading = false;
		pthread_t loading_screen_thread;

		if ( show_loading_screen ){
			pthread_create( &loading_screen_thread, NULL, loadingScreen, NULL );
		}

		bool b = false;
		{ /* force scope */
			World world = World( player, *it );

			Music::pause();
			Music::fadeIn( 0.3 );
			Music::loadSong( Util::getFiles( "data/music/", "*" ) );
			Music::play();

			Player * playerX = (Player *) player;
			playerX->setY( 200 );
			playerX->setMoving( true );
			playerX->setStatus( Status_Falling );

			if ( show_loading_screen ){
				pthread_mutex_lock( &Global::loading_screen_mutex );
				Global::done_loading = true;
				pthread_mutex_unlock( &Global::loading_screen_mutex );

				pthread_join( loading_screen_thread, NULL );
			}

			b = playLevel( world, playerX );

		}

		ObjectFactory::destroy();
		HeartFactory::destroy();

		if ( ! b ){
			return;
		}

		fadeOut( "Next level" );
	}

	fadeOut( "You win!" );
}

static bool isArg( const char * s1, const char * s2 ){
	return strcasecmp( s1, s2 ) == 0;
}

static Object * selectPlayer() throw( LoadException ){
	Bitmap background( "data/sprites/select.png" );
	// background.resize( GFX_X, GFX_Y );

	Object * maxima = new Player( "data/chars/maxima/maxima.txt" );
	cout << "Loaded maxima" << endl;
	Object * mandy = new Player( "data/chars/mandy/mandy.txt" );
	cout << "Loaded mandy" << endl;
	Object * kula = new Player( "data/chars/kula/kula.txt" );
	cout << "Loaded kula" << endl;

	Object * all[] = { maxima, mandy, kula };
	Object ** end = &all[ 2 ];

	Keyboard key;

	Bitmap work( GFX_X / 2, GFX_Y / 2 );

	Global::speed_counter = 0;

	vector< Object * > temp;
	World world;

	Object ** current1 = all;

	key.setDelay( Keyboard::Key_RIGHT, 100 );
	key.setDelay( Keyboard::Key_LEFT, 100 );

	bool draw = true;
	while ( ! key[ Keyboard::Key_ENTER ] && ! key[ Keyboard::Key_SPACE ] ){
		key.poll();

		Character * ch = (Character *) *current1;

		if ( Global::speed_counter > 0 ){
			double think = Global::speed_counter;
			while ( think > 0 ){

				if ( key[ Keyboard::Key_LEFT ] ){
					current1 -= 1;
					if ( current1 < all ){
						current1 = end;
					}
				}

				if ( key[ Keyboard::Key_RIGHT ] ){
					current1 += 1;
					if ( current1 > end ){
						current1 = all;
					}
				}

				if ( ch->testAnimation() ){
					ch->testReset();
				}

				think--;
			}

			Global::speed_counter = 0;
			draw = true;
		}

		if ( draw ){

			Character copy( *ch );
			copy.setX( 83 );
			copy.setY( 0 );
			copy.setZ( 240 );

			background.Blit( work );
			copy.draw( &work, 0 );
			work.Stretch( *Bitmap::Screen );
			draw = false;
		}

		while ( Global::speed_counter == 0 ){
			key.poll();
			Util::rest( 1 );
		}
	}

	// Object * v = new Player( *(Character *)*current1 );
	for ( unsigned int i = 0; i < sizeof( all ) / sizeof( Object * ); i++ ){
		if ( current1 != &all[ i ] ){
			delete all[ i ];
		}
	}

	return *current1;
}

static bool titleScreen(){
	Bitmap::Screen->Blit( string( "data/paintown-title.png" ) );
	Music::loadSong( "data/music/aqua.s3m" );

	const int fontY = 20;
	const Font & font = Font::getFont( "data/fonts/arial.ttf", 20, fontY );

	const int PLAY = 0;
	const int QUIT = 2;
	const char * options[] = { "New game",
	                           "Change controls",
				   "Quit" };
	// font.printf( 1, 1, Bitmap::makeColor( 255, 255, 255 ), *Bitmap::Screen, "foo" );
	unsigned int choose = 0;
	const unsigned int maxOptions = sizeof( options ) / sizeof( char* );
	for ( unsigned int i = 0; i < maxOptions; i++ ){
		int yellow = Bitmap::makeColor( 255, 255, 0 );
		int white = Bitmap::makeColor( 255, 255, 255 );
		unsigned int color = i == choose ? yellow : white;
		font.printf( 200, (int)(200 + i * fontY * 1.2), color, *Bitmap::Screen, options[ i ] );

	}

	Keyboard key;

	key.setDelay( Keyboard::Key_UP, 100 );
	key.setDelay( Keyboard::Key_DOWN, 100 );

	bool done = false;
	while ( ! done ){
		
		key.poll();
		bool draw = false;
		if ( Global::speed_counter > 0 ){
			double think = Global::speed_counter;

			while ( think > 0 ){
				think--;

				if ( key[ Keyboard::Key_UP ] ){
					draw = true;
					choose = (choose - 1 + maxOptions) % maxOptions;
				}

				if ( key[ Keyboard::Key_DOWN ] ){
					draw = true;
					choose = (choose + 1 + maxOptions) % maxOptions;
				}

				done = key[ Keyboard::Key_ENTER ] || key[ Keyboard::Key_SPACE ] || key[ Keyboard::Key_ESC ];
				if ( key[ Keyboard::Key_ESC ] ){
					choose = QUIT;
				}
				if ( key[ Keyboard::Key_ENTER ] || key[ Keyboard::Key_SPACE ] ){
									}
			}

			Global::speed_counter = 0;
		}

		if ( draw ){
			for ( unsigned int i = 0; i < maxOptions; i++ ){
				int yellow = Bitmap::makeColor( 255, 255, 0 );
				int white = Bitmap::makeColor( 255, 255, 255 );
				unsigned int color = i == choose ? yellow : white;
				font.printf( 200, (int)(200 + i * fontY * 1.2), color, *Bitmap::Screen, options[ i ] );
			}
		}
		
		while ( Global::speed_counter == 0 ){
			Util::rest( 1 );
			key.poll();
		}
	}

	while ( key[ Keyboard::Key_ENTER ] || key[ Keyboard::Key_SPACE ] ){
		key.poll();
		Util::rest( 1 );
	}

	switch ( choose ){
		case QUIT : {
			return false;
			break;
		}
		case PLAY : {
			try{
				realGame( selectPlayer() );
			} catch ( const LoadException & le ){
				cout << "Could not load player: " << le.getReason() << endl;
			}
			return true;
			break;
		}
		default : return true;
	}

	while ( key.keypressed() ){
		key.poll();
		Util::rest( 1 );
	}
}

int paintown_main( int argc, char ** argv ){
	
	/* janitor cleans up some global stuff */
	Collector janitor;
	int gfx = Global::WINDOWED;

	// bool tester = false;
	// const char * name = NULL;
	// int xmap = 0;
	const char * WINDOWED_ARG = "-w";
	
	for ( int q = 1; q < argc; q++ ){
		if ( isArg( argv[ q ], WINDOWED_ARG ) ){
			gfx = Global::FULLSCREEN;
		}
	}
	
	TimeDifference diff;
	diff.startTime();
	if ( ! init( gfx ) ){
		cout << "Could not initialize system" << endl;
	}
	diff.endTime();
	diff.printTime("Init:");
	
	Music m;

	while ( titleScreen() != false );

	/*
	pthread_mutex_init( &Global::loading_screen_mutex, NULL );
	
	pthread_t loading_screen_thread;
	pthread_create( &loading_screen_thread, NULL, loadingScreen, NULL );

	const char * player_path = "data/chars/maxima/maxima.txt";
	// const char * player_path = "data/chars/gangmember/gangmember.txt";

	Object * ch;
	try{
		ch = new Player( player_path );
	} catch ( const LoadException & ex ){
		cout<<"Could not load player: "<< ex.getReason() << endl;
		return 1;
	}

	pthread_mutex_lock( &Global::loading_screen_mutex );
	Global::done_loading = true;
	pthread_mutex_unlock( &Global::loading_screen_mutex );
	pthread_join( loading_screen_thread, NULL );

	realGame( ch );

	delete ch;
	*/

	// unload_datafile( Global::all_fonts );
	cout<<"Exiting normally"<<endl;

	return 0;
}

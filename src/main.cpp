#include "main.h"
#include "init.h"
#include <iostream>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#include "factory/collector.h"
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
#include "util/timedifference.h"
#include "world.h"

/*
#define GFX_X 640
#define GFX_Y 480
*/

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

void realGame( Object * player ){
	// global_debug = true;
	bool show_loading_screen = true;

	Global::done_loading = false;
	pthread_t loading_screen_thread;
	
	if ( show_loading_screen ){
		pthread_create( &loading_screen_thread, NULL, loadingScreen, NULL );
	}

	World world = World( player, "data/levels/easy/1aeasy.txt" );
	
	// Bitmap Screen( screen );
	Bitmap work( GFX_X / 2, GFX_Y / 2 );
	Bitmap screen_buffer( GFX_X, GFX_Y );
	// Bitmap screen_buffer( Screen.getWidth(), Screen.getHeight() );
	Global::speed_counter = 0;
	Global::second_counter = 0;
	int game_time = 100;

	/* end of init stuff */

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

	Keyboard key;

	while ( !key[ Keyboard::Key_ESC ] ){

		bool draw = false;
		key.poll();

		if ( Global::speed_counter > 0 ){
			int think = Global::speed_counter;
			while ( think > 0 ){
				draw = true;
				world.act();
				think--;

				if ( playerX->getHealth() <= 0 ){
					playerX->deathReset();
					world.addObject( player );
				}
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

			// work.printf( 180, 1, Bitmap::makeColor(255,255,255), (FONT *)all_fonts[ JOHNHANDY_PCX ].dat, "%d", game_time );

			work.Stretch( screen_buffer );
			FontRender * render = FontRender::getInstance();
			render->render( &screen_buffer );

			// acquire_screen();
			screen_buffer.Blit( world.getX(), world.getY(), *Bitmap::Screen );
			// release_screen();

			if ( key[ Keyboard::Key_F12 ] ){
				work.save( "scr.bmp" );
			}

			work.clear();
		}

		while ( Global::speed_counter == 0 ){
			Util::rest( 1 );
			key.poll();
		}
	}

}

static bool isArg( const char * s1, const char * s2 ){
	return strcasecmp( s1, s2 ) == 0;
}

static Object * selectPlayer() throw( LoadException ){
	Bitmap background( "data/sprites/select.png" );
	// background.resize( GFX_X, GFX_Y );

	Object * maxima = new Player( "data/chars/maxima/maxima.txt" );
	Object * mandy = new Player( "data/chars/mandy/mandy.txt" );

	Object * all[] = { maxima, mandy };

	Keyboard key;

	Bitmap work( GFX_X / 2, GFX_Y / 2 );

	Global::speed_counter = 0;

	vector< Object * > temp;
	World world;

	Character * current1 = (Character *)mandy;

	bool draw = true;
	while ( ! key[ Keyboard::Key_ESC ] ){
		key.poll();

		if ( Global::speed_counter > 0 ){
			int think = Global::speed_counter;
			while ( think > 0 ){
				if ( current1->testAnimation() ){
					current1->testReset();
				}
				think--;
			}

			Global::speed_counter = 0;
			draw = true;
		}

		if ( draw ){

			current1->setX( 83 );
			current1->setY( 0 );
			current1->setZ( 240 );

			background.Blit( work );
			current1->draw( &work, 0 );
			work.Stretch( *Bitmap::Screen );
			draw = false;
		}

		while ( Global::speed_counter == 0 ){
			key.poll();
			Util::rest( 1 );
		}
	}

	for ( unsigned int i = 0; i < sizeof( all ) / sizeof( Object * ); i++ ){
		if ( current1 != all[ i ] ){
			delete all[ i ];
		}
	}

	return current1;
}

static void titleScreen(){
	Bitmap::Screen->Blit( string( "data/paintown-title.png" ) );

	const int fontY = 20;
	const Font & font = Font::getFont( "data/fonts/arial.ttf", 20, fontY );

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

	while ( ! key[ Keyboard::Key_ESC ] ){
		
		key.poll();
		bool draw = false;
		if ( Global::speed_counter > 0 ){
			int think = Global::speed_counter;

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

				if ( key[ Keyboard::Key_ENTER ] || key[ Keyboard::Key_SPACE ] ){
					try{
						realGame( selectPlayer() );
					} catch ( const LoadException & le ){
						cout << "Could not load player: " << le.getReason() << endl;
					}
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
}

int paintown_main( int argc, char ** argv ){
	
	/* janitor cleans up some global stuff */
	Collector janitor;
	Music m;
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
	diff.printTime("Init: ");

	titleScreen();

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

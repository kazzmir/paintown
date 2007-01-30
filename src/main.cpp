#include <allegro.h>

#ifdef WINDOWS
#include <winalleg.h>
#endif

#include <iostream>
#include <loadpng.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#include "factory/collector.h"
#include "factory/font_render.h"
#include "fonts.h"
#include "globals.h"
#include "level/scene.h"
#include "loading.h"
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
#include "init.h"

#define GFX_X 640
#define GFX_Y 480

/* Global effect for copying */
static Object * bang = NULL;

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

	done_loading = false;
	pthread_t loading_screen_thread;
	
	if ( show_loading_screen ){
		pthread_create( &loading_screen_thread, NULL, loadingScreen, NULL );
	}

	World world = World( player, "data/levels/easy/1aeasy.txt" );
	
	if ( show_loading_screen ){
		pthread_mutex_lock( &loading_screen_mutex );
		done_loading = true;
		pthread_mutex_unlock( &loading_screen_mutex );

		pthread_join( loading_screen_thread, NULL );
	}

	// Bitmap Screen( screen );
	Bitmap work( 320, 240 );
	Bitmap screen_buffer( GFX_X, GFX_Y );
	// Bitmap screen_buffer( Screen.getWidth(), Screen.getHeight() );
	speed_counter = 0;
	second_counter = 0;
	int game_time = 100;

	/* end of init stuff */
	
	while ( !key[ KEY_ESC ] ){

		bool draw = false;

		if ( speed_counter > 0 ){
			int think = speed_counter;
			while ( think > 0 ){
				draw = true;
				world.act();
				think--;
			}
			speed_counter = 0;
		}
		
		while ( second_counter > 0 ){
			game_time--;
			second_counter--;
			if ( game_time < 0 )
				game_time = 0;
		}
	
		if ( draw ){
			world.draw( &work );

			// work.printf( 180, 1, Bitmap::makeColor(255,255,255), (FONT *)all_fonts[ JOHNHANDY_PCX ].dat, "%d", game_time );

			work.Stretch( screen_buffer );
			FontRender * render = FontRender::getInstance();
			render->render( &screen_buffer );

			acquire_screen();
			screen_buffer.Blit( world.getX(), world.getY(), *Bitmap::Screen );
			release_screen();

			if ( key[ KEY_F12 ] ){
				work.save( "scr.bmp" );
			}

			work.clear();
		}

		while ( speed_counter == 0 ){
			rest( 1 );
		}
	}

}



int main( int argc, char ** argv ){
	
	Collector janitor;
	int gfx = GFX_AUTODETECT_WINDOWED;

	bool tester = false;
	const char * name = NULL;
	int xmap = 0;
	
	for ( int q = 1; q < argc; q++ ){
		if ( strcasecmp( argv[q], "-w" ) == 0 ){
			gfx = GFX_AUTODETECT_FULLSCREEN;
		} else if ( strcasecmp( argv[q], "-t" ) == 0 ){
			q++;
			tester = true;
			if ( q < argc )
				name = argv[q];
			q++;
			if ( q < argc )
				xmap = atoi( argv[q] );
		}
	}
	
	TimeDifference diff;
	diff.startTime();
	init( gfx );
	diff.endTime();
	diff.printTime("Init: ");

	/*
	if ( tester ){
		// char * name = argv[ 1 ];
		showAnimations( name, xmap );
		return 0;
	}
	*/

	/*
	while ( !key[ KEY_ESC ] ){
		Collector Blah;
		testAnimation();
	}
	*/

	// loadingScreen( NULL );

	pthread_mutex_init( &loading_screen_mutex, NULL );
	
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

	pthread_mutex_lock( &loading_screen_mutex );
	done_loading = true;
	pthread_mutex_unlock( &loading_screen_mutex );
	pthread_join( loading_screen_thread, NULL );

	realGame( ch );

	delete ch;

	// testAnimation( );

	unload_datafile( all_fonts );
	cout<<"Exiting normally"<<endl;

	return 0;

}
END_OF_MAIN()

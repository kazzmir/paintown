#include <allegro.h>

#ifdef WINDOWS
#include <winalleg.h>
#endif

#include <pthread.h>
#include <math.h>
#include <iostream>

#include "loading.h"
#include "util/bitmap.h"

#include "fonts.h"
#include "util/funcs.h"
#include <vector>

using namespace std;

extern pthread_mutex_t loading_screen_mutex;
extern bool done_loading;
extern int speed_counter;
extern DATAFILE * all_fonts;

void * loadingScreen( void * arg ){
	
	cout<<"Loading"<<endl;
	Bitmap Screen( screen );
	// Bitmap work( Screen.getWidth(), Screen.getHeight() );

	int load_x = 80;
	int load_y = 120;
	FONT * my_font = (FONT *)all_fonts[ JOKERMAN_PCX ].dat;
	const char * the_string = "Loading Paintown";
	// const char * the_string = (const char *)arg;
	int load_width = text_length( my_font, the_string );
	int load_height = text_height( my_font );

	Bitmap work( load_width, load_height );

	// make some scope
	/*
	if ( true ){
		
		Bitmap tmp( "data/paintown-title.png" );
		tmp.Blit( 0, 0, Screen );
		tmp.Blit( load_x, load_y, load_width, load_height, 0, 0, work );
	}
	*/
	Screen.Blit( string( "data/paintown-title.png" ) );

	Screen.Blit( load_x, load_y, load_width, load_height, 0, 0, work );

	// DATAFILE * all_fonts = load_datafile( "data/fonts.dat" );

	// caps.reserve( strlen( the_string ) );
	vector< int > caps( strlen( the_string ) );
	// cout<<"Caps size = "<<caps.size()<<endl;
	int xp = 31;
	for ( vector< int >::iterator it = caps.begin(); it != caps.end(); it++ ){
		*it = xp;
		xp -= 3;
		if ( xp < -31 )
			xp = xp + 31 + 31;
	}

	int colors[ 32 ];
	blend_palette( colors, 32, Bitmap::makeColor( 16, 16, 16 ), Bitmap::makeColor( 192, 8, 8 ) );

	unsigned int which_letter = 0;

	bool quit = false;
	speed_counter = 0;
	Screen.printfNormal( 400, 470, Bitmap::makeColor( 192, 0, 0 ), "Made by Jon Rafkind" );
	while ( !quit ){

		bool draw = false;
		if ( speed_counter > 0 ){

			int think = speed_counter;
			while ( think-- ){
		
				caps[ which_letter ] += 3;
				if ( caps[ which_letter ] > 31 ){
					caps[ which_letter ] = -31 + (caps[which_letter] - 31);
				}
				which_letter++;
				if ( which_letter >= caps.size() )
					which_letter = 0;

				for ( int q = caps.size() - 1; q >= 0; q-- ){
					int col = colors[ (int)fabs( caps[ q ] ) ];
					string h;
					for ( int i = 0; i <= q; i++ )
						h += the_string[i];
					work.printf( 1, 1, col, my_font, h.c_str() );
				}

			}

			// work.rectangle( 1, 1, work.getWidth() - 1, work.getHeight() - 1, Bitmap::makeColor(255,255,255) );

			/*
			int think = speed_counter;

			while ( think-- ){

				// work.rectangleFill( rnd( work.getWidth() ), rnd( work.getHeight() ), rnd( work.getWidth() ), rnd( work.getHeight() ), Bitmap::makeColor( rnd( 64 ) + 128, rnd ( 64 ) + 128, rnd( 64 ) + 128 ) );
				work.printf( 1, 1, Bitmap::makeColor( 255, 255, 255 ), my_font, "Hello world" );

			}
			*/

			speed_counter = 0;
			draw = true;
		} else {
			// sched_yield();
			// pthread_yield();
			rest( 1 );
		}

		if ( draw ){
			Screen.acquire();
			work.Blit( load_x, load_y, Screen );
			Screen.release();
			// work.clear();
		}

		pthread_mutex_lock( &loading_screen_mutex );
		quit = done_loading;
		pthread_mutex_unlock( &loading_screen_mutex );

	}
	cout<<"Done loading"<<endl;

	return NULL;
			
}

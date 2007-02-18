#include <allegro.h>

#ifdef WINDOWS
#include <winalleg.h>
#endif

#include <pthread.h>
#include <math.h>
#include <iostream>

#include "loading.h"
#include "util/bitmap.h"
#include "init.h"
#include "fonts.h"
#include "util/font.h"
#include "util/funcs.h"
#include <vector>

using namespace std;

void drawLetters( Bitmap & work, vector< int > caps, const char * str, int * colors, Font & myFont ){
	for ( int q = caps.size() - 1; q >= 0; q-- ){
		int col = colors[ (int)fabs( caps[ q ] ) ];
		string h;
		for ( int i = 0; i <= q; i++ )
			h += str[ i ];
		work.printf( 1, 1, col, myFont, h.c_str() );
	}
}

void * loadingScreen( void * arg ){
	
	cout<<"Loading"<<endl;
	// Bitmap Screen( screen );
	// Bitmap work( Screen.getWidth(), Screen.getHeight() );

	int load_x = 80;
	int load_y = 120;
	Font myFont = getFont( JOKERMAN_PCX );
	// FONT * my_font = (FONT *)Global::all_fonts[ JOKERMAN_PCX ].dat;
	const char * the_string = "Loading Paintown";
	// const char * the_string = (const char *)arg;
	int load_width = myFont.textLength( the_string );
	int load_height = myFont.getHeight();

	Bitmap work( load_width, load_height );

	static vector< int > caps( strlen( the_string ) );
	// cout<<"Caps size = "<<caps.size()<<endl;
	int xp = 31;

	int colors[ 32 ];
	/* blend from dark grey to light red */
	Util::blend_palette( colors, 32, Bitmap::makeColor( 16, 16, 16 ), Bitmap::makeColor( 192, 8, 8 ) );

	static unsigned int which_letter = 0xfff;
	if ( which_letter == 0xfff ){
		for ( vector< int >::iterator it = caps.begin(); it != caps.end(); it++ ){
			*it = xp;
			xp -= 3;
			if ( xp < -31 )
				xp = xp + 31 + 31;
		}
	}

	bool quit = false;
	Global::speed_counter = 0;

	Bitmap::Screen->Blit( string( "data/paintown-title.png" ) );
	Bitmap::Screen->Blit( load_x, load_y, load_width, load_height, 0, 0, work );
	/* I made this :p */
	Bitmap::Screen->printfNormal( 400, 470, Bitmap::makeColor( 192, 0, 0 ), "Made by Jon Rafkind" );
	drawLetters( work, caps, the_string, colors, myFont );

	while ( !quit ){

		bool draw = false;
		if ( Global::speed_counter > 0 ){

			int think = Global::speed_counter;
			while ( think-- ){
		
				draw = true;
				caps[ which_letter ] += 3;
				if ( caps[ which_letter ] > 31 ){
					caps[ which_letter ] = -31 + (caps[which_letter] - 31);
				}
				which_letter++;
				if ( which_letter >= caps.size() )
					which_letter = 0;
			}

			Global::speed_counter = 0;
			draw = true;
		} else {
			rest( 1 );
		}

		if ( draw ){
			drawLetters( work, caps, the_string, colors, myFont );

			// Bitmap::Screen.acquire();
			work.Blit( load_x, load_y, *Bitmap::Screen );
			// Bitmap::Screen.release();
		}

		pthread_mutex_lock( &Global::loading_screen_mutex );
		quit = Global::done_loading;
		pthread_mutex_unlock( &Global::loading_screen_mutex );

	}

	// cout<<"Done loading"<<endl;

	return NULL;
}

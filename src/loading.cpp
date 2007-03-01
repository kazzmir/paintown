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

void drawLetters( Bitmap & work, vector< int > caps, const char * str, int * colors, const Font & myFont ){
	for ( int q = caps.size() - 1; q >= 0; q-- ){
		int col = colors[ (int)fabs( caps[ q ] ) ];
		string h;
		for ( int i = 0; i <= q; i++ )
			h += str[ i ];
		// work.printf( 1, 1, col, myFont, h.c_str() );
		myFont.printf( 0, 0, col, work, h.c_str() );
	}
}

typedef struct pair{
	int x, y;
} ppair;

void * loadingScreen( void * arg ){
	int load_x = 80;
	int load_y = 120;
	string name = "data/fonts/arial.ttf";
	const Font & myFont = Font::getFont( name, 24, 24 );
	const char * the_string = "Loading Paintown";
	int load_width = myFont.textLength( the_string );
	int load_height = myFont.getHeight( the_string );

	Bitmap work( load_width, load_height );
	Bitmap letters( load_width, load_height );

	letters.fill( Bitmap::MaskColor );
	myFont.printf( 0, 0, Bitmap::makeColor( 255, 255, 255 ), letters, the_string ); 

	vector< ppair > pairs;
	/* store every pixel we need to draw */
	for ( int x = 0; x < letters.getWidth(); x++ ){
		for ( int y = 0; y < letters.getHeight(); y++ ){
			int pixel = letters.getPixel( x, y );
			if ( pixel != Bitmap::MaskColor ){
				ppair p;
				p.x = x;
				p.y = y;
				pairs.push_back( p );
			}
		}
	}

	const int MAX_COLOR = 200;
	
	int colors[ MAX_COLOR ];
	int c1 = Bitmap::makeColor( 16, 16, 16 );
	int c2 = Bitmap::makeColor( 192, 8, 8 );
	/* blend from dark grey to light red */
	Util::blend_palette( colors, MAX_COLOR / 2, c1, c2 );
	Util::blend_palette( colors + MAX_COLOR / 2, MAX_COLOR / 2, c2, c1 );

	Global::speed_counter = 0;

	Bitmap::Screen->Blit( string( "data/paintown-title.png" ) );
	Bitmap::Screen->Blit( load_x, load_y, load_width, load_height, 0, 0, work );
	Font::getDefaultFont().printf( 400, 480 - Font::getDefaultFont().getHeight() * 5 / 2, Bitmap::makeColor( 192, 192, 192 ), *Bitmap::Screen, "Made by Jon Rafkind" );
	bool quit = false;

	/* keeps the colors moving */
	static unsigned mover = 0;

	while ( ! quit ){

		bool draw = false;
		if ( Global::speed_counter > 0 ){
			int think = Global::speed_counter;	
			Global::speed_counter = 0;
			draw = true;
			while ( think > 0 ){
				mover = (mover + 1) % MAX_COLOR;
				think -= 1;
			}
		} else {
			rest( 1 );
		}

		if ( draw ){
			for ( vector< ppair >::iterator it = pairs.begin(); it != pairs.end(); it++ ){
				int color = colors[ (it->x - mover + MAX_COLOR) % MAX_COLOR ];
				work.putPixel( it->x, it->y, color );
			}
			/* work already contains the correct background */
			work.Blit( load_x, load_y, *Bitmap::Screen );
		}

		pthread_mutex_lock( &Global::loading_screen_mutex );
		quit = Global::done_loading;
		pthread_mutex_unlock( &Global::loading_screen_mutex );
	}

	return NULL;
}

void * loadingScreen2( void * arg ){
	
	cout<<"Loading"<<endl;
	// Bitmap Screen( screen );
	// Bitmap work( Screen.getWidth(), Screen.getHeight() );

	int load_x = 80;
	int load_y = 120;
	string name = "tmp/comic.ttf";
	name = "tmp/arial.ttf";
	const Font & myFont = Font::getFont( name, 24, 24 );
	const char * the_string = "Loading Paintown";
	// const char * the_string = (const char *)arg;
	int load_width = myFont.textLength( the_string );
	// int load_height = myFont.getHeight() * 3 / 2;
	int load_height = myFont.getHeight( the_string );

	Bitmap work( load_width, load_height );

	static vector< int > caps( strlen( the_string ) );
	// cout<<"Caps size = "<<caps.size()<<endl;
	const int MAX_COLOR = 30;
	int xp = MAX_COLOR - 1;

	int colors[ MAX_COLOR ];
	/* blend from dark grey to light red */
	Util::blend_palette( colors, MAX_COLOR, Bitmap::makeColor( 16, 16, 16 ), Bitmap::makeColor( 192, 8, 8 ) );

	static unsigned int which_letter = 0xfff;
	if ( which_letter == 0xfff ){
		for ( vector< int >::iterator it = caps.begin(); it != caps.end(); it++ ){
			*it = xp;
			xp -= 3;
			if ( xp < -(MAX_COLOR - 1))
				xp = xp + (MAX_COLOR - 1) * 2;
		}
	}

	bool quit = false;
	Global::speed_counter = 0;

	Bitmap::Screen->Blit( string( "data/paintown-title.png" ) );
	Bitmap::Screen->Blit( load_x, load_y, load_width, load_height, 0, 0, work );
	/* I made this :p */
	// Bitmap::Screen->printfNormal( 400, 470, Bitmap::makeColor( 192, 0, 0 ), "Made by Jon Rafkind" );
	Font::getDefaultFont().printf( 400, 480 - Font::getDefaultFont().getHeight() * 5 / 2, Bitmap::makeColor( 192, 192, 192 ), *Bitmap::Screen, "Made by Jon Rafkind" );
	drawLetters( work, caps, the_string, colors, myFont );

	while ( !quit ){

		bool draw = false;
		if ( Global::speed_counter > 0 ){

			int think = Global::speed_counter;
			while ( think-- ){
		
				draw = true;
				caps[ which_letter ] += MAX_COLOR / 10;
				if ( caps[ which_letter ] > MAX_COLOR - 1 ){
					caps[ which_letter ] = -(MAX_COLOR - 1) + (caps[which_letter] - (MAX_COLOR - 1));
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

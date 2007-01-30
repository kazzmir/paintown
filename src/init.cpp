#include "init.h"
#include <pthread.h>
#include <allegro.h>
#include <iostream>
#include <loadpng.h>
#include "util/bitmap.h"

using namespace std;

volatile int speed_counter = 0;
volatile int second_counter = 0;

pthread_mutex_t loading_screen_mutex;
bool done_loading = false;

DATAFILE * all_fonts;

void inc_speed_counter() {
	speed_counter++;
}
END_OF_FUNCTION( inc_speed_counter );

void inc_second_counter() {
	second_counter++;
}
END_OF_FUNCTION( inc_second_counter );

void init( int gfx ){

	cout<<"Allegro init: "<<allegro_init()<<endl;
	cout<<"Loadpng init: "<<loadpng_init()<<endl;
	cout<<"Install timer: "<<install_timer()<<endl;
	
	set_volume_per_voice( 0 );
	cout<<"Install sound: "<<install_sound( DIGI_AUTODETECT, MIDI_NONE, "" )<<endl;

	cout<<"Install keyboard: "<<install_keyboard()<<endl;
	cout<<"Install mouse: "<<install_mouse()<<endl;
	loadpng_init();
	set_color_depth( 16 );
	// int gfx = GFX_AUTODETECT_WINDOWED;
	// gfx = GFX_AUTODETECT_FULLSCREEN;
	cout<<"Set gfx mode: "<<set_gfx_mode( gfx, GFX_X, GFX_Y, 0, 0 )<<endl;
	cout<<"Can triple buffer = "<<(gfx_capabilities & GFX_CAN_TRIPLE_BUFFER)<<endl;

	LOCK_VARIABLE( speed_counter );
	LOCK_VARIABLE( second_counter );
	LOCK_FUNCTION( (void *)inc_speed_counter );
	LOCK_FUNCTION( (void *)inc_second_counter );
	cout<<"Install timer: "<<install_int_ex( inc_speed_counter, BPS_TO_TIMER( 90 ) )<<endl;
	cout<<"Install second timer: "<<install_int_ex( inc_second_counter, BPS_TO_TIMER( 1 ) )<<endl;
	srand( time( NULL ) );
	all_fonts = load_datafile( "data/fonts.dat" );
	if ( ! all_fonts ){
		cout << "Could not load data/fonts.dat!" << endl;
		exit( 1 );
	}
	cout<<"Loaded fonts " << endl;

	cout<<"-- END init --"<<endl;

	Bitmap::Screen = new Bitmap( screen );
}

#include <allegro.h>
#ifdef WINDOWS
#include <winalleg.h>
#endif

#include "init.h"
#include <pthread.h>

#include <iostream>
#include <dumb.h>
#include <aldumb.h>
#include <loadpng.h>
#include "util/bitmap.h"

using namespace std;

volatile int Global::speed_counter = 0;
volatile int Global::second_counter = 0;

static const int TICS_PER_SECOND = 90;

pthread_mutex_t Global::loading_screen_mutex;
bool Global::done_loading = false;

const int Global::WINDOWED = GFX_AUTODETECT_WINDOWED;
const int Global::FULLSCREEN = GFX_AUTODETECT_FULLSCREEN;

void inc_speed_counter() {
	Global::speed_counter += 1;
}
END_OF_FUNCTION( inc_speed_counter );

void inc_second_counter() {
	Global::second_counter += 1;
}
END_OF_FUNCTION( inc_second_counter );

bool init( int gfx ){

	cout << "Allegro version: " << ALLEGRO_VERSION_STR << endl;
	cout<<"Allegro init: "<<allegro_init()<<endl;
	cout<<"Loadpng init: "<<loadpng_init()<<endl;
	cout<<"Install timer: "<<install_timer()<<endl;
	
	set_volume_per_voice( 0 );
	cout<<"Install sound: "<<install_sound( DIGI_AUTODETECT, MIDI_NONE, "" )<<endl;

	cout<<"Install keyboard: "<<install_keyboard()<<endl;
	cout<<"Install mouse: "<<install_mouse()<<endl;
	loadpng_init();
	set_color_depth( 16 );
	cout<<"Set gfx mode: "<<set_gfx_mode( gfx, GFX_X, GFX_Y, 0, 0 )<<endl;

	LOCK_VARIABLE( speed_counter );
	LOCK_VARIABLE( second_counter );
	LOCK_FUNCTION( (void *)inc_speed_counter );
	LOCK_FUNCTION( (void *)inc_second_counter );
	cout<<"Install game timer: "<<install_int_ex( inc_speed_counter, BPS_TO_TIMER( TICS_PER_SECOND ) )<<endl;
	cout<<"Install second timer: "<<install_int_ex( inc_second_counter, BPS_TO_TIMER( 1 ) )<<endl;
	srand( time( NULL ) );
	set_display_switch_mode( SWITCH_BACKGROUND );
	
	atexit( &dumb_exit );
	dumb_register_packfiles();

	cout<<"-- END init --"<<endl;

	Bitmap::Screen = new Bitmap( screen );
	if ( ! Bitmap::Screen ){
		return false;
	}
	return true;
}

#ifndef _paintown_init_h
#define _paintown_init_h

#include <pthread.h>
#include <allegro.h>

void init( int gfx );

/* global vars */

namespace Global{
extern volatile int speed_counter;
extern volatile int second_counter;

extern pthread_mutex_t loading_screen_mutex;
extern bool done_loading;

extern DATAFILE * all_fonts;
}

#define GFX_X 640
#define GFX_Y 480

#endif

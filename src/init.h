#ifndef _paintown_init_h
#define _paintown_init_h

/* global vars */

#define GFX_X 640
#define GFX_Y 480

namespace Global{
    extern volatile int speed_counter;
    extern volatile unsigned int second_counter;

    extern const double LOGIC_MULTIPLIER;
    extern const int TICS_PER_SECOND;

    extern const int WINDOWED;
    extern const int FULLSCREEN;

    bool init( int gfx );
}

/*
#define GFX_X 320
#define GFX_Y 240
*/

#endif

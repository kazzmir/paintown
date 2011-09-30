#ifdef USE_ALLEGRO
#include <allegro.h>
#endif
#ifdef USE_SDL
#include <SDL/SDL.h>
#endif
#ifdef USE_ALLEGRO5
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#endif

#include "init.h"
#include "util/bitmap.h"

namespace Global{
    volatile int speed_counter4;
    volatile int second_counter;
    int FULLSCREEN;
    int WINDOWED;
    int TICS_PER_SECOND;
    double LOGIC_MULTIPLIER;
    int rateLimit;
    void close(){
    }
}


void Screen::init(){
#ifdef USE_ALLEGRO
    install_allegro(SYSTEM_NONE, &errno, atexit);
    set_color_depth(16);
    set_color_conversion(COLORCONV_NONE);
#elif USE_SDL
    SDL_Init(SDL_INIT_VIDEO);
    Graphics::Bitmap::setFakeGraphicsMode(640, 480);
#elif USE_ALLEGRO5
    al_init();
    al_init_image_addon();
    al_init_primitives_addon();
#endif
}
    
void Screen::finish(){
#ifdef USE_SDL
    SDL_Quit();
#endif
}
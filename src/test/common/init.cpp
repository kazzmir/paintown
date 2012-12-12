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

#include "configuration.h"
#include "init.h"
#include "util/graphics/bitmap.h"

namespace Global{
    /*
    volatile int speed_counter4;
    volatile int second_counter;
    int FULLSCREEN;
    int WINDOWED;
    int TICS_PER_SECOND = 40;
    double LOGIC_MULTIPLIER;
    int rateLimit;

    void close(){
    }
    */
    extern int FULLSCREEN;
    extern int WINDOWED;
}


void Screen::fakeInit(int x, int y){
#ifdef USE_ALLEGRO
    install_allegro(SYSTEM_NONE, &errno, atexit);
    set_color_depth(16);
    set_color_conversion(COLORCONV_NONE);
#elif USE_SDL
    SDL_Init(SDL_INIT_VIDEO);
    Graphics::Bitmap::setFakeGraphicsMode(x, y);
#elif USE_ALLEGRO5
    al_init();
    al_init_image_addon();
    al_init_primitives_addon();
#endif
}

void Screen::fakeFinish(){
    Graphics::Bitmap::shutdown();
#ifdef USE_SDL
    SDL_Quit();
#endif
}

void Screen::realInit(int x, int y){
#ifdef USE_ALLEGRO
    install_allegro(GFX_AUTODETECT_WINDOWED, &errno, atexit);
    // TODO add keyboard, timers
    set_color_depth(16);
    //set_color_conversion(COLORCONV_NONE);
#elif USE_SDL
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_EnableUNICODE(1);
    Graphics::setGfxModeWindowed(x, y);
#elif USE_ALLEGRO5
    al_init();
    al_init_image_addon();
    al_init_primitives_addon();
#endif
}

void Screen::realFinish(){
    Graphics::Bitmap::shutdown();
#ifdef USE_SDL
    SDL_Quit();
#endif
}

void Screen::change(int x, int y){
    int gfx = Configuration::getFullscreen() ? Global::FULLSCREEN : Global::WINDOWED;
    int ok = Graphics::setGraphicsMode(gfx, x, y);
    if (ok == 0){
        Configuration::setScreenWidth(x);
        Configuration::setScreenHeight(y);
    } else {
        int ok = Graphics::setGraphicsMode(gfx, Configuration::getScreenWidth(), Configuration::getScreenHeight());
    }
}

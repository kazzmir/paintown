#ifdef USE_SDL2

#include <iostream>
#include "r-tech1/system/init.h"
#include "r-tech1/debug.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

namespace System{

SDL_TimerID timer;

void initSystem(const Global::InitConditions & conditions, Global::stream_type & out){
    // initialize SDL
    int ok = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER);
    DebugLog << "SDL2 initialization: " << ok << std::endl;
    if (ok != 0){
        DebugLog << "  error: " << SDL_GetError() << std::endl;
        exit(1);
    }

    if (TTF_Init() != 0){
        DebugLog << "SDL2 TTF initialization failed: " << SDL_GetError() << std::endl;
        exit(1);
    }
}

static unsigned int timerCallback(unsigned int, void * param){
    Global::speed_counter4 += 1;
    /* zero callback means the timer should cancel. non-zero return value means that the timer should be restarted */
    return 1;
}

void startTimers(){
    timer = SDL_AddTimer(1000/Global::TICS_PER_SECOND, timerCallback, NULL);
    if (timer == 0){
        DebugLog << "SDL2 timer initialization failed" << std::endl;
        exit(1);
    }
}

void shutdown(){
    if (timer != 0){
        if (!SDL_RemoveTimer(timer)){
            DebugLog << "SDL2: unable to remove timer" << std::endl;
        }
    }

    DebugLog << "Shutting down SDL2" << std::endl;
    TTF_Quit();
    SDL_Quit();
}

}

#endif

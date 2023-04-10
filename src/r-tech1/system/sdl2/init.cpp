#ifdef USE_SDL2

#include <iostream>
#include "r-tech1/system/init.h"
#include "r-tech1/debug.h"
#include <SDL2/SDL.h>

namespace System{

void initSystem(const Global::InitConditions & conditions, Global::stream_type & out){
    // initialize SDL
    int ok = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER);
    DebugLog << "SDL2 initialization: " << ok << std::endl;
    if (ok != 0){
        DebugLog << "  error: " << SDL_GetError() << std::endl;
        exit(1);
    }
}

void startTimers(){
}

void shutdown(){
    DebugLog << "Shutting down SDL2" << std::endl;
    SDL_Quit();
}

}

#endif

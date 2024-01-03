#ifdef USE_SDL2

#include "../sound.h"

#include <SDL2/SDL_mixer.h>

Sound::Sound():
own(nullptr){
}
    
Sound::Sound(const std::string & path):
own(nullptr){
    loadFromMemory(path.c_str(), 0);
}

Sound::Sound(const char * data, int length):
own(nullptr){
    loadFromMemory(data, length);
}

void Sound::initialize(){
    // Already handled 
    // r-tech1/system/sdl2/init.cpp:78:    int ok = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER | SDL_INIT_EVENTS | SDL_INIT_GAMECONTROLLER);
}

void Sound::uninitialize(){
}
    
void Sound::loadFromMemory(const char * data, int length){
    this->data.sample = Mix_LoadWAV(data);
    own = new int;
    *own = 1;
}

void Sound::destroy(){
    if (own){
        *own -= 1;
        if ( *own == 0 ){
            delete own;
            Mix_FreeChunk(data.sample);
            own = nullptr;
        }
    }
}

void Sound::stop(){
}
    
void Sound::play(){
    Mix_PlayChannel(-1, data.sample, 0);
}

void Sound::play(double volume, int pan){
    Mix_PlayChannel(-1, data.sample, 0);
}

void Sound::setVolume(double volume){
}

void Sound::playLoop(){
}

#endif

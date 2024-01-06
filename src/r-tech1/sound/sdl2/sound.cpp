#ifdef USE_SDL2

#include "../sound.h"
#include "debug.h"

#include <SDL2/SDL_mixer.h>

Sound::SoundInfo::SoundInfo():
frequency(44100),
channels(2),
format(AUDIO_S16SYS){
}

Sound::Sound():
own(nullptr){
}
    
Sound::Sound(const std::string & path):
own(nullptr){
    loadFromFile(path);
}

Sound::Sound(const char * data, int length):
own(nullptr){
    loadFromMemory(data, length);
}

void Sound::initialize(){
}

void Sound::uninitialize(){
}

void Sound::loadFromFile(const std::string & path){
    this->data.sample = Mix_LoadWAV(path.c_str());
    if (this->data.sample == NULL){
        DebugLog << "Unable to load sound from memory: " << SDL_GetError() << std::endl;
    }
    own = new int;
    *own = 1;
}

void Sound::loadFromMemory(const char * data, int length){
    SDL_RWops* ops = SDL_RWFromConstMem(data, length);
    this->data.sample = Mix_LoadWAV_RW(ops, 1);
    if (this->data.sample == NULL){
        DebugLog << "Unable to load sound from memory: " << SDL_GetError() << std::endl;
    }
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
    if (data.sample != NULL){
        Mix_HaltChannel(this->data.channel);
    }
}
    
void Sound::play(){
    if (data.sample != NULL){
        this->data.channel = Mix_PlayChannel(-1, data.sample, 0);
    }
}

void Sound::play(double volume, int pan){
    if (data.sample != NULL){
        this->data.channel = Mix_PlayChannel(-1, data.sample, 0);
    }
}

void Sound::setVolume(double volume){
}

void Sound::playLoop(){
}

#endif

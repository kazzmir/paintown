#ifdef USE_SDL2

#include "../sound.h"

Sound::Sound():
own(nullptr){
}
    
Sound::Sound(const std::string & path):
own(nullptr){
    loadFromMemory(nullptr, 0);
}

Sound::Sound(const char * data, int length):
own(nullptr){
    loadFromMemory(data, length);
}

void Sound::initialize(){
}

void Sound::uninitialize(){
}
    
void Sound::loadFromMemory(const char * data, int length){
    own = new int;
    *own = 1;
}

void Sound::destroy(){
    if (own){
        *own -= 1;
        if ( *own == 0 ){
            delete own;
            // al_destroy_sample(data.sample);
            own = nullptr;
        }
    }
}

void Sound::stop(){
}
    
void Sound::play(){
}

void Sound::play(double volume, int pan){
}

void Sound::setVolume(double volume){
}

void Sound::playLoop(){
}

#endif

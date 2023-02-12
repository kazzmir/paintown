#ifdef USE_SDL

#include "../sound.h"

Sound::Sound(){
}
    
Sound::Sound(const std::string & path){
}

Sound::Sound(const char * data, int length){
}

void Sound::initialize(){
}

void Sound::uninitialize(){
}
    
void Sound::loadFromMemory(const char * data, int length){
}

void Sound::destroy(){
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

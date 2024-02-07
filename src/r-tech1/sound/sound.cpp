#include <stdlib.h>
#include "r-tech1/sound/sound.h"

#ifdef USE_ALLEGRO5
#include "sound/allegro5/sound.cpp"
#endif

#include "r-tech1/configuration.h"
#include "r-tech1/file-system.h"
        
Sound::SoundInfo Sound::Info;

Sound::Sound(const Sound & copy):
own(NULL){
    own = copy.own;
    if (own){
        *own += 1;
    }
    data = copy.data;
}

Sound::Sound(Storage::File & file):
own(NULL){
    int length = file.getSize();
    char * data = new char[length];
    file.readLine(data, length);
    try{
        loadFromMemory(data, length);
        delete[] data;
    } catch (const LoadException & fail){
        delete[] data;
        throw;
    }
}

Sound & Sound::operator=(const Sound & rhs){
    destroy();
    own = rhs.own;
    if (own){
        *own += 1;
    }

    data = rhs.data;

    return *this;
}

double Sound::scale(double in){
    return in * Configuration::getSoundVolume() / 100.0;
}

Sound::~Sound(){
    destroy();
}

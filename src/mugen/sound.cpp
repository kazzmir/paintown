#include <string.h>
#include "sound.h"
#include "util/sound.h"

namespace Mugen{

Sound::Sound():
next(0),
length(0),
groupNumber(0),
sampleNumber(0),
sample(0),
sound(0){
    //Nothing
}

void Sound::load(){
    sound = new ::Sound(sample, length);
}

void Sound::play(){
    if (sound){
        sound->play();
    }
}

void Sound::stop(){
    if (sound){
        sound->stop();
    }
}

Sound::Sound( const Sound &copy ){
    this->next = copy.next;
    this->length = copy.length;
    this->groupNumber = copy.groupNumber;
    this->sampleNumber = copy.sampleNumber;
    this->sample = new char[this->length];
    memcpy(this->sample, copy.sample, this->length);
    load();
}

Sound::~Sound(){
    if (sample){
        delete[] sample;
    }
    if (sound){
        delete sound;
    }
}

}

#include <string.h>
#include "mugen_sound.h"
#include "util/sound.h"

MugenSound::MugenSound():
next(0),
length(0),
groupNumber(0),
sampleNumber(0),
sample(0),
sound(0){
    //Nothing
}

void MugenSound::load(){
    sound = new Sound(sample, length);
}

void MugenSound::play(){
    if (sound){
        sound->play();
    }
}

void MugenSound::stop(){
    if (sound){
        sound->stop();
    }
}

MugenSound::MugenSound( const MugenSound &copy ){
    this->next = copy.next;
    this->length = copy.length;
    this->groupNumber = copy.groupNumber;
    this->sampleNumber = copy.sampleNumber;
    this->sample = new char[this->length];
    memcpy(this->sample, copy.sample, this->length);
    load();
}

MugenSound::~MugenSound(){
    if (sample){
        delete[] sample;
    }
    if (sound){
        delete sound;
    }
}


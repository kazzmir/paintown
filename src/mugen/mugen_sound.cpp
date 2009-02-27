#include <string.h>
#include "mugen_sound.h"

MugenSound::MugenSound():
next(0),
length(0),
groupNumber(0),
sampleNumber(0){
    //Nothing
}

MugenSound::MugenSound( const MugenSound &copy ){
    this->next = copy.next;
    this->length = copy.length;
    this->groupNumber = copy.groupNumber;
    this->sampleNumber = copy.sampleNumber;
    this->sample = new char[this->length];
    memcpy( this->sample, copy.sample, this->length );
}

MugenSound::~MugenSound(){
    if( sample ) delete sample;
}


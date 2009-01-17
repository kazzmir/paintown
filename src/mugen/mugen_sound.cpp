#include <string.h>
#include "mugen_sound.h"

MugenSound::MugenSound():
length(0),
groupNumber(0),
sampleNumber(0){
    //Nothing
}

MugenSound::MugenSound( const MugenSound &copy ){
    this->length = copy.length;
    this->groupNumber = copy.groupNumber;
    this->sampleNumber = copy.sampleNumber;
    this->sample = copy.sample;
}

MugenSound::~MugenSound(){
    if( sample ) delete sample;
}


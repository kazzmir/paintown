#ifndef mugen_sound_h
#define mugen_sound_h

#include <string>

class MugenSound{
public:
    MugenSound();
    MugenSound( const MugenSound &copy );
    ~MugenSound();
    
    int length;
    int groupNumber;
    int sampleNumber;
    char * sample;

};

#endif

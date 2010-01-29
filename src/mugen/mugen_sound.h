#ifndef mugen_sound_h
#define mugen_sound_h

#include <string>

class Sound;

class MugenSound{
public:
    MugenSound();
    MugenSound(const MugenSound &copy);
    virtual ~MugenSound();

    void load();
    void play();
    void stop();
    
    int next;
    int length;
    int groupNumber;
    int sampleNumber;
    char * sample;
    Sound * sound;

};

#endif

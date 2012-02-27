#ifndef mugen_sound_h
#define mugen_sound_h

#include <string>

class Sound;

namespace Mugen{

class Sound{
public:
    Sound();
    Sound(const Sound &copy);
    virtual ~Sound();

    void load();
    void play();
    void stop();
    
    int next;
    int length;
    int groupNumber;
    int sampleNumber;
    char * sample;
    ::Sound * sound;
};

}

#endif

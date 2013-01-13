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

    static void enableSounds();
    static void disableSounds();

protected:
    /* For globally disabling sounds, such as during replay */
    static bool enabled;
};

}

#endif

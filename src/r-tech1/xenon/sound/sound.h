#ifndef _rtech1_sound_h
#define _rtech1_sound_h

#include <string>
#include "r-tech1/exceptions/load_exception.h"

#ifdef USE_ALLEGRO5
#include "allegro5/sound.h"
#endif

struct SAMPLE;

namespace Storage{
    class File;
}

/* a sound! */
class Sound{
public:
    Sound();
    /* create from wav file (riff header + pcm) */
    Sound(const char * data, int length);
    /* load from abstract file */
    Sound(Storage::File & file);
    /* load from path */
    Sound(const std::string & path);
    Sound(const Sound & copy);

    /* do any global initialization necessary */
    static void initialize();
    /* cleanup */
    static void uninitialize();

    Sound & operator=( const Sound & rhs );

    void play();
    void play(double volume, int pan);
    void playLoop();
    void stop();

    void setVolume(double volume);

    virtual ~Sound();

    /* global frequency to use */
    // static const int FREQUENCY = 22050;
    struct SoundInfo{
        SoundInfo():
            frequency(44100),
            channels(2),
            format(0){
            }

        int frequency;
        int channels;

        /* format is mostly for SDL */
        int format;
    };
    static SoundInfo Info;

protected:

    void loadFromMemory(const char * data, int length);

    /* scale to the configuration sound level */
    static double scale(double in);

    void destroy();

    // SAMPLE * my_sound;
    SoundData data;

    /* reference counting */
    int * own;
};

#endif

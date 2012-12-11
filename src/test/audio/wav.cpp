#include <iostream>
#include <string>
#include <SDL/SDL.h>
#include "util/debug.h"
#include "util/sound/sound.h"
#include "util/sound/sdl/mixer/SDL_mixer.h"

#if 0
class Sound{
public:
    struct SoundInfo{
        SoundInfo():
            frequency(22050),
            channels(2),
            format(0){
            }

        int frequency;
        int channels;

        /* format is mostly for SDL */
        int format;
    };
    static SoundInfo Info;
};
#endif

// Sound::SoundInfo Sound::Info;

class Configuration{
public:
    static int getSoundVolume(){
        return 100;
    }
};

using namespace std;

void play(const string & path){
    Sound sound(path);
    sound.play();

    /*
    while (true){
        SDL_Delay(1);
    }
    */
    SDL_Delay(8000 * 2);
}

void initialize(int rate){
    Configuration::getSoundVolume();
    SDL_Init(SDL_INIT_AUDIO);
    atexit(SDL_Quit);

    int audio_rate = rate;
    // int audio_rate = 22050;
    Uint16 audio_format = AUDIO_S16; 
    // audio_format = AUDIO_U8;
    // Uint16 audio_format = MIX_DEFAULT_FORMAT; 
    int audio_channels = 2;
    int audio_buffers = 4096;
    // int audio_buffers = 44100;
    if (Mix_OpenAudio(audio_rate, audio_format, audio_channels, audio_buffers)) {
        printf("Unable to open audio: %s!\n", Mix_GetError());
        // exit(1);
    }

    /* use the frequency enforced by the audio system */
    Mix_QuerySpec(&audio_rate, &audio_format, &audio_channels);
    Sound::Info.frequency = audio_rate;
    Sound::Info.channels = audio_channels;
    Sound::Info.format = audio_format;

    Global::debug(0) << "Opened audio frequency " << audio_rate << " channels " << audio_channels << " format " << audio_format << endl;
}

int main(int argc, char ** argv){
    if (argc < 2){
        Global::debug(0) << "Give an audio file as an argument" << endl;
        return 0;
    }
    initialize(48000);
    try{
        play(argv[1]);
    } catch (...){
        Global::debug(0) << "Failed" << endl;
    }
}

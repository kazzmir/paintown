#include <iostream>
#include <string>
#include "r-tech1/debug.h"
#include "r-tech1/file-system.h"
#include "r-tech1/sound/sound.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

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

#ifndef WINDOWS
/* FIXME: dont put these methods in this test file */
Filesystem::AbsolutePath Filesystem::configFile(){
    std::ostringstream str;
    /* what if HOME isn't set? */
    str << getenv("HOME") << "/.paintownrc";
    return Filesystem::AbsolutePath(str.str());
}

Filesystem::AbsolutePath Filesystem::userDirectory(){
    std::ostringstream str;
    char * home = getenv("HOME");
    if (home == NULL){
        str << "/tmp/paintown";
    } else {
        str << home << "/.paintown/";
    }
    return Filesystem::AbsolutePath(str.str());
}

#endif

class Configuration{
public:
    static int getSoundVolume2(){
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
    Configuration::getSoundVolume2();
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
#ifndef WINDOWS
int main(int argc, char ** argv){
#else
#include <SDL2/SDL.h>
int main(int argc, char *argv[]){
#endif
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

    return 0;
}


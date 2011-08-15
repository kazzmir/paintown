#include <iostream>
#include <string>
#include "util/music-player.h"

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

Sound::SoundInfo Sound::Info;

using namespace std;

void play(const string & path){
}

void initialize(){
}

int main(int argc, char ** argv){
    if (argc < 2){
        cout << "Give an audio file as an argument" << endl;
        return 0;
    }
    initialize();
    play(argv[1]);
}

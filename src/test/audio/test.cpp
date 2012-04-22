#include <iostream>
#include <string>
#include <algorithm>
#include "util/music-player.h"
#include "util/file-system.h"
#include "util/debug.h"

using std::string;

namespace Util{

Filesystem::AbsolutePath getDataPath2(){
    return Filesystem::AbsolutePath("data");
}

void rest(int x){
}

static int lowerCase(int c){
    return tolower(c);
}

static int upperCase(int c){
    return toupper(c);
}

std::string upperCaseAll(std::string str){
    std::transform(str.begin(), str.end(), str.begin(), upperCase);
    return str;
}

std::string lowerCaseAll(std::string str){
    std::transform(str.begin(), str.end(), str.begin(), lowerCase);
    return str;
}

}

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
    Filesystem::AbsolutePath absolute(path);
    Util::Mp3Player player(absolute);
    player.play();

    while (true){
        SDL_Delay(1);
    }
    // SDL_Delay(8000 * 3);
}

static bool isZip(const string & path){
    return path.find(".zip") != string::npos;
}

static vector<string> filterMp3(const vector<string> & input){
    vector<string> out;

    for (vector<string>::const_iterator it = input.begin(); it != input.end(); it++){
        if (it->find(".mp3") != string::npos){
            out.push_back(*it);
        }
    }

    return out;
}

static void playZip(const string & path, const string & what = ""){
    Storage::instance().addOverlay(Filesystem::AbsolutePath(path), Filesystem::AbsolutePath("test/zip5"));
    if (what == ""){
        std::vector<string> mp3s = filterMp3(Storage::instance().containerFileList(Filesystem::AbsolutePath(path)));
        if (mp3s.size() > 0){
            play(string("test/zip5/") + mp3s[0]);
        } else {
            Global::debug(0) << "No mp3's found in " << path << std::endl;
        }
    } else {
        play(string("test/zip5/") + what);
    }
}

void initialize(int rate){
    SDL_Init(SDL_INIT_AUDIO);
    atexit(SDL_Quit);

    int audio_rate = rate;
    // int audio_rate = 22050;
    Uint16 audio_format = AUDIO_S16; 
    audio_format = AUDIO_U8;
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
        if (isZip(argv[1])){
            if (argc > 2){
                playZip(argv[1], argv[2]);
            } else {
                playZip(argv[1]);
            }
        } else {
            play(argv[1]);
        }
    } catch (const Exception::Base & fail){
        Global::debug(0) << "Failed: " << fail.getTrace() << endl;
    } catch (...){
        Global::debug(0) << "Failed" << std::endl;
    }
}

#include "util/sound.h"
#include "util/music.h"
#include "configuration.h"
#include "util/file-system.h"
#include "../common/init.h"
#include <string>
#include <algorithm>

using std::string;

int Configuration::getSoundVolume(){
    return 100;
}

int Configuration::getMusicVolume(){
    return 100;
}

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

string upperCaseAll(std::string str){
    std::transform(str.begin(), str.end(), str.begin(), upperCase);
    return str;
}

string lowerCaseAll(std::string str){
    std::transform(str.begin(), str.end(), str.begin(), lowerCase);
    return str;
}

}

int main(){
    Sound::initialize();
    Music music(true);
    while (true){
        music.loadSong("music/Aurora.ogg");
        music.loadSong("music/Techtopia.ogg");
    }
}

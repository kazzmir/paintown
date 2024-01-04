#include "r-tech1/sound/sound.h"
#include "r-tech1/sound/music.h"
#include "r-tech1/configuration.h"
#include "r-tech1/file-system.h"
#include <string>
#include <algorithm>

/*
#ifdef USE_SDL
#include <SDL/SDL.h>
#endif
*/

using std::string;

int Configuration::getSoundVolume(){
    return 100;
}

int Configuration::getMusicVolume(){
    return 100;
}

#if 0
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
#endif



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

int main(){
    Sound::initialize();
    Music music(true);
    // music.loadSong("data/music/Aurora.ogg");
    while (true){
        music.loadSong("data/music/Aurora.ogg");
        music.loadSong("data/music/Techtopia.ogg");
    }
}

#include "mod.h"
#include "level/utils.h"
#include "debug.h"
#include <vector>

using namespace std;

namespace Paintown{

OpenborMod::OpenborMod(const Filesystem::AbsolutePath & path):
reader(path){
}

static void findPlayers(Bor::PackReader & reader){
    vector<string> paths = reader.findPaths("data/chars/*/*.txt");
    for (vector<string>::iterator it = paths.begin(); it != paths.end(); it++){
        Global::debug(0) << "Openbor player: " << *it << endl;
    }
}

Filesystem::AbsolutePath OpenborMod::selectPlayer(const std::string & message, const Level::LevelInfo & info, int & remap){
    findPlayers(reader);
    return Mod::selectPlayer(message, info, remap);
}

const string OpenborMod::getMenu(){
    return "menu/main.txt";
}
    
std::vector<Level::LevelInfo> OpenborMod::getLevels(){
    vector<Level::LevelInfo> levels;
    Level::LevelInfo level;
    levels.push_back(level);
    return levels;
}

}

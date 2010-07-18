#include "mod.h"
#include "level/utils.h"
#include "util.h"
#include "debug.h"
#include "pack-reader.h"
#include "util/tokenreader.h"
#include "util/token.h"
#include <vector>

using namespace std;

namespace Paintown{

OpenborMod::OpenborMod(const Filesystem::AbsolutePath & path):
reader(path){
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

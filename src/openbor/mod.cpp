#include "mod.h"
#include "level/utils.h"
#include "util/bitmap.h"
#include "util.h"
#include "debug.h"
#include "pack-reader.h"
#include "util/tokenreader.h"
#include "util/token.h"
#include "util/load_exception.h"
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
    
Bitmap * OpenborMod::createBitmap(const Filesystem::RelativePath & path){
    try{
        char * data = reader.readFile(reader.getFile(path.path()));
        Bitmap * bitmap = new Bitmap(data, reader.getFileLength(path.path()));
        delete[] data;
        return bitmap;
    } catch (const Bor::PackError & error){
        throw LoadException(__FILE__, __LINE__, error, "Could not create bitmap");
    }
}

}

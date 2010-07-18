#include <string>
#include <vector>
#include "mod.h"
#include "level/utils.h"
#include "util/file-system.h"
#include "util/tokenreader.h"
#include "util/token.h"
#include "util/load_exception.h"
#include "openbor/mod.h"
#include "globals.h"

using namespace std;

namespace Paintown{

Mod * Mod::currentMod = NULL;
Mod::Mod(const Filesystem::AbsolutePath & path) throw (LoadException){
    try{
        TokenReader reader(path.path());
        Token * head = reader.readToken();

        Token * name = head->findToken("game/name");
        Token * token_menu = head->findToken("game/menu");
        if (token_menu == NULL){
            throw LoadException(__FILE__, __LINE__, "No game/menu token found. Add (menu some/path) to the mod file.");
        }
        *token_menu >> menu;

        vector<Token*> token_level = head->findTokens("game/level-set");
        Global::debug(1) << "Found " << token_level.size() << " level sets" << endl;
        for (vector<Token*>::iterator it = token_level.begin(); it != token_level.end(); it++){
            Token * set = *it;
            levels.push_back(Level::readLevel(set));
        }

    } catch (const TokenException & e){
        Global::debug(0) << "Error while reading mod " << path.path() << ":" << e.getTrace() << endl;
    }
}
    
Mod::Mod(){
}

Mod::~Mod(){
}
    
vector<Level::LevelInfo> & Mod::getLevels(){
    return levels;
}
    
const string Mod::getMenu(){
    return menu;
}

void Mod::loadDefaultMod(){
    loadPaintownMod("paintown");
}

void Mod::setMod(Mod * mod){
    if (currentMod != NULL){
        delete currentMod;
    }
    currentMod = mod;
}

void Mod::loadOpenborMod(const Filesystem::AbsolutePath & path){
    setMod(new OpenborMod(path));
}

void Mod::loadPaintownMod(const string & name){
   string path = name + "/" + name + ".txt"; 
   setMod(new Mod(Filesystem::find(Filesystem::RelativePath(path))));
}

Mod * Mod::getCurrentMod(){
    return currentMod;
}

}

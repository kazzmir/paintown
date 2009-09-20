#include <string>
#include <vector>
#include "mod.h"
#include "level/utils.h"
#include "util/file-system.h"
#include "util/tokenreader.h"
#include "util/token.h"
#include "util/load_exception.h"
#include "globals.h"

using namespace std;

namespace Paintown{

Mod * Mod::currentMod = NULL;
Mod::Mod(const string & path) throw (LoadException){
    try{
        TokenReader reader(path);
        Token * head = reader.readToken();

        Token * name = head->findToken("game/name");
        Token * token_menu = head->findToken("game/menu");
        if (token_menu == NULL){
            throw LoadException("No game/menu token found. Add (menu some/path) to the mod file.");
        }
        *token_menu >> menu;

    } catch (const TokenException & e){
        Global::debug(0) << "Error while reading mod " << path << ":" << e.getReason() << endl;
    }
}

Mod::~Mod(){
    for (vector<Level::LevelInfo *>::iterator it = levels.begin(); it != levels.end(); it++){
        delete (*it);
    }
}
    
const string & Mod::getMenu(){
    return menu;
}

void Mod::loadMod(const string & name) throw (LoadException, Filesystem::NotFound){
   string path = name + "/" + name + ".txt"; 
   currentMod = new Mod(Filesystem::find(path));
}

Mod * Mod::getCurrentMod(){
    return currentMod;
}

}

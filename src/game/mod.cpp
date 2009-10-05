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

        vector<Token*> token_level = head->findTokens("game/level-set");
        Global::debug(1) << "Found " << token_level.size() << " level sets" << endl;
        for (vector<Token*>::iterator it = token_level.begin(); it != token_level.end(); it++){
            Token * set = *it;
            levels.push_back(Level::readLevel(set));
        }

    } catch (const TokenException & e){
        Global::debug(0) << "Error while reading mod " << path << ":" << e.getReason() << endl;
    }
}

Mod::~Mod(){
}
    
vector<Level::LevelInfo> & Mod::getLevels(){
    return levels;
}
    
const string & Mod::getMenu(){
    return menu;
}

void Mod::loadMod(const string & name) throw (LoadException, Filesystem::NotFound){
   string path = name + "/" + name + ".txt"; 
   Mod * newMod = new Mod(Filesystem::find(path));
   if (currentMod != NULL){
       delete currentMod;
   }
   currentMod = newMod;
}

Mod * Mod::getCurrentMod(){
    return currentMod;
}

}

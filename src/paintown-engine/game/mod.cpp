#include <string>
#include <vector>
#include "mod.h"
#include "paintown-engine/level/utils.h"
#include "util/gui/cutscene.h"
#include "util/file-system.h"
#include "util/tokenreader.h"
#include "util/token.h"
#include "util/load_exception.h"
#include "util/bitmap.h"
#include "openbor/mod.h"
#include "globals.h"

using namespace std;

namespace Paintown{

Mod * Mod::currentMod = NULL;
Mod::Mod(const string & name, const Util::ReferenceCount<Storage::File> & path):
name(name){
    try{
        TokenReader reader;
        Token * head = reader.readTokenFromFile(*path);

        const Token * name = head->findToken("game/name");
        const Token * token_menu = head->findToken("game/menu");
        if (token_menu == NULL){
            throw LoadException(__FILE__, __LINE__, "No game/menu token found. Add (menu some/path) to the mod file.");
        }
        token_menu->view() >> menu;

        const Token * introToken = head->findToken("game/intro");
        if (introToken != NULL){
            introToken->view() >> intro;
        }

        vector<const Token*> token_level = head->findTokens("game/level-set");
        Global::debug(1) << "Found " << token_level.size() << " level sets" << endl;
        for (vector<const Token*>::iterator it = token_level.begin(); it != token_level.end(); it++){
            const Token * set = *it;
            levels.push_back(Level::readLevel(set));
        }

    } catch (const TokenException & e){
        Global::debug(0) << "Error while reading mod: " << e.getTrace() << endl;
    }
}
    
Graphics::Bitmap * Mod::createBitmap(const Filesystem::RelativePath & path){
    return new Graphics::Bitmap(*Storage::instance().open(Storage::instance().find(path)));
}

Graphics::Bitmap Mod::makeBitmap(const Filesystem::RelativePath & path){
    Graphics::Bitmap * what = createBitmap(path);
    Graphics::Bitmap out(*what);
    delete what;
    return out;
}
    
void Mod::playIntro(){
    if (intro != ""){
        try{
            Gui::CutScene scene(Storage::instance().find(Filesystem::RelativePath(intro)));
            scene.playAll();
        } catch (const Filesystem::NotFound & fail){
            Global::debug(0) << "Could not play intro because " << fail.getTrace() << std::endl;
        }
    }
}
    
Mod::Mod(){
}

Mod::~Mod(){
}
    
vector<Level::LevelInfo> Mod::getLevels(){
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
   setMod(new Mod(name, Storage::instance().open(Storage::instance().find(Filesystem::RelativePath(path)))));
}
    
Filesystem::AbsolutePath Mod::find(const Filesystem::RelativePath & path){
    Storage::System & filesystem = Storage::instance();
    string totalFailure;
    /* first search in the mod directory */
    try{
        return filesystem.find(Filesystem::RelativePath(name).join(path));
    } catch (const Filesystem::NotFound & fail){
        totalFailure = fail.getTrace();
    }

    /* then search in the regular place */
    try{
        return filesystem.find(path);
    } catch (const Filesystem::NotFound & fail){
        /* if a file can't be found then combine the errors */
        totalFailure += fail.getTrace();
        throw Filesystem::NotFound(__FILE__, __LINE__, fail, totalFailure);
    }
}

Mod * Mod::getCurrentMod(){
    return currentMod;
}

}

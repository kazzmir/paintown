#include "utils.h"
#include "util/tokenreader.h"
#include "util/token.h"
#include "util/load_exception.h"
#include "util/funcs.h"
#include "globals.h"

namespace Level{

using namespace std;

LevelInfo::LevelInfo():
playerPath("players/"),
_loadingMessage("Loading Paintown"),
_loadingBackground(Global::titleScreen()){
}

LevelInfo::LevelInfo(const LevelInfo & info){
    this->levels = info.levels;
    this->_loadingMessage = info._loadingMessage;
    this->_loadingBackground = info._loadingBackground;
    this->playerPath = info.getPlayerPath();
}
    
LevelInfo & LevelInfo::operator=(const LevelInfo & info){
    this->levels = info.levels;
    this->_loadingMessage = info._loadingMessage;
    this->_loadingBackground = info._loadingBackground;
    this->playerPath = info.getPlayerPath();
    return *this;
}
    
const std::string & LevelInfo::loadingMessage() const {
    return this->_loadingMessage;
}

const std::string & LevelInfo::loadingBackground() const {
    return this->_loadingBackground;
}

const std::string & LevelInfo::getPlayerPath() const {
    return this->playerPath;
}

void LevelInfo::addLevel(const string & s){
    levels.push_back(s);
}
    
const vector<string> & LevelInfo::getLevels() const {
    return levels;
}
    
void LevelInfo::setPlayerPath(const std::string & s){
    this->playerPath = s;
}
    
LevelInfo::~LevelInfo(){
}

LevelInfo readLevels( const string & filename ){
    LevelInfo info;
    try{
        TokenReader tr( filename );
        Token * head = tr.readToken();

        if ( *head == "levels" ){
            while (head->hasTokens()){
                Token * next;
                *head >> next;
                /* old way */
                if (!next->hasTokens()){
                    Global::debug(1) << "Add level " << next->getName() << endl;
                    info.addLevel(next->getName());
                } else if (*next == "level"){
                } else if (*next == "player-path"){
                    string s;
                    *next >> s;
                    Global::debug(1) << "Set player path to " << s << endl;
                    info.setPlayerPath(s);
                }
            }
        }
        return info;

    } catch ( const TokenException & lex ){
        Global::debug( 0 ) << "Could not load " << filename << ". Reason: " << lex.getReason() << endl;
        return info;
    }
}

}

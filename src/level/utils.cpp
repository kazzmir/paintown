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
    
LevelInfo::~LevelInfo(){
}

LevelInfo readLevels( const string & filename ){
    LevelInfo info;
    try{
        TokenReader tr( filename );
        Token * head = tr.readToken();

        if ( *head == "levels" ){
            while (head->hasTokens()){
                string s;
                *head >> s;
                info.addLevel(s);
            }
        }
        return info;

    } catch ( const TokenException & lex ){
        Global::debug( 0 ) << "Could not load " << filename << ". Reason: " << lex.getReason() << endl;
        return info;
    }
}

}

#include "utils.h"
#include "util/tokenreader.h"
#include "util/token.h"
#include "util/load_exception.h"
#include "util/funcs.h"
#include "globals.h"
#include "util/debug.h"

namespace Level{

using namespace std;

LevelInfo::LevelInfo():
playerPath("players/"),
name("Level set"),
_loadingMessage("Loading Paintown"),
_loadingBackground(Global::titleScreen()),
background(0),
x(-1),
y(-1){
}

LevelInfo::LevelInfo(const LevelInfo & info){
    this->levels = info.levels;
    this->_loadingMessage = info._loadingMessage;
    this->_loadingBackground = info._loadingBackground;
    this->playerPath = info.getPlayerPath();
    this->name = info.getName();
    this->background = info.getBackground();
    this->x = info.getPositionX();
    this->y = info.getPositionY();
}
    
LevelInfo & LevelInfo::operator=(const LevelInfo & info){
    this->levels = info.levels;
    this->_loadingMessage = info._loadingMessage;
    this->_loadingBackground = info._loadingBackground;
    this->playerPath = info.getPlayerPath();
    this->name = info.getName();
    this->background = info.getBackground();
    this->x = info.getPositionX();
    this->y = info.getPositionY();
    return *this;
}
    
void LevelInfo::setLoadingMessage(const std::string & str){
    this->_loadingMessage = str;
}

void LevelInfo::setPosition(int x, int y){
    this->x = x;
    this->y = y;
}
    
const std::string & LevelInfo::loadingMessage() const {
    return this->_loadingMessage;
}

const Filesystem::AbsolutePath & LevelInfo::loadingBackground() const {
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
    
void LevelInfo::setName(const std::string & s){
    this->name = s;
}
    
const string & LevelInfo::getName() const {
    return name;
}
    
LevelInfo::~LevelInfo(){
}

LevelInfo readLevel(const Token * level){
    LevelInfo info;

    try{
        const Token * token_levels = level->findToken("level-set/levels");
        const Token * token_player = level->findToken("level-set/player-path");

        string name;
        if (level->match("level-set/name", name)){
            info.setName(name);
        }

        string playerPath;
        if (level->match("level-set/player-path", playerPath)){
            info.setPlayerPath(playerPath);
        }

        TokenView view = token_levels->view();
        while (view.hasMore()){
            const Token * next = 0;
            view >> next;
            Global::debug(1) << "Add level " << next->getName() << endl;
            info.addLevel(next->getName());
        }

    } catch (const TokenException & e){
        Global::debug( 0 ) << "Error while reading level set: " << e.getTrace() << endl;
    }

    return info;
}

#if 0
LevelInfo readLevels( const string & filename ){
    LevelInfo info;
    try{
        TokenReader tr( filename );
        Token * head = tr.readToken();

        readName(&info, head->findToken("level-set/name"));
        readLevels(&info, head->findToken("level-set/levels"));
        readPlayerPath(&info, head->findToken("level-set/player-path"));

        /*
        if ( *head == "levels" ){
            while (head->hasTokens()){
                Token * next;
                *head >> next;
                / * old way * /
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
        */

        return info;

    } catch ( const TokenException & lex ){
        Global::debug( 0 ) << "Could not load " << filename << ". Reason: " << lex.getReason() << endl;
        return info;
    }
}
#endif

}

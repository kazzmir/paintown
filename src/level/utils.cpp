#include "utils.h"
#include "util/tokenreader.h"
#include "util/token.h"
#include "util/load_exception.h"
#include "util/funcs.h"
#include "globals.h"

namespace Level{

using namespace std;

LevelInfo::LevelInfo(){
}

LevelInfo::LevelInfo(const LevelInfo & info){
    this->levels = info.levels;
}

void LevelInfo::addLevel(const string & s){
    levels.push_back(s);
}
    
const vector<string> & LevelInfo::getLevels(){
    return levels;
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

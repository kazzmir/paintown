#include "mod.h"
#include "level/utils.h"
#include "util.h"
#include "debug.h"
#include "pack-reader.h"
#include "util/tokenreader.h"
#include "util/token.h"
#include <vector>

using namespace std;

namespace Bor{

class ParseException: std::exception {
public:
    std::string getReason() const;
    virtual ~ParseException() throw();
};

}

namespace Paintown{

OpenborMod::OpenborMod(const Filesystem::AbsolutePath & path):
reader(path){
}

static bool isPlayer(Bor::PackReader & reader, const string & path){
    char * data = NULL;
    try{
        TokenReader tokens;
        char * data = reader.readFile(reader.getFile(path));
        string parsed = Bor::doParse(data, reader.getFileLength(path));
        Global::debug(0) << "Bor input: '" << parsed << "'" << endl;
        delete[] data;
        data = NULL;
        Token * start = tokens.readTokenFromString(parsed);
        Token * type = start->findToken("_/type");
        if (type != NULL){
            string kind;
            *type >> kind;
            return kind == "player";
        } else {
            return false;
        }
    } catch (const TokenException & t){
        Global::debug(0) << "Failed to parse pak file " << path << " " << t.getTrace() << endl;
    } catch (const Bor::PackError & p){
        Global::debug(0) << "Failed to parse pak file " << path << " " << p.getTrace() << endl;
    } catch (const Bor::ParseException & e){
        Global::debug(0) << "Failed to parse pak file " << path << " " << e.getReason() << endl;
    }

    delete[] data;
    return false;
}

static void findPlayers(Bor::PackReader & reader){
    vector<string> paths = reader.findPaths("data/chars/*/*.txt");
    for (vector<string>::iterator it = paths.begin(); it != paths.end(); it++){
        if (isPlayer(reader, *it)){
            Global::debug(0) << "Openbor player: " << *it << endl;
        }
    }
}

Filesystem::AbsolutePath OpenborMod::selectPlayer(const std::string & message, const Level::LevelInfo & info, int & remap){
    findPlayers(reader);
    return Mod::selectPlayer(message, info, remap);
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

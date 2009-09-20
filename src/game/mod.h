#ifndef _paintown_mod_h
#define _paintown_mod_h

#include <string>
#include <vector>

#include "util/file-system.h"
#include "util/load_exception.h"

namespace Level{
    class LevelInfo;
}

/* encapsulates a configuration for a game */
namespace Paintown{

class Mod{
public:
    Mod(const std::string & path) throw (LoadException);

    const std::string & getMenu();

    std::vector<Level::LevelInfo> & getLevels();

    ~Mod();

    static void loadMod(const std::string & path) throw (LoadException, Filesystem::NotFound);
    static Mod * getCurrentMod();
    
protected:
    std::vector<Level::LevelInfo> levels;
    std::string menu;

    static Mod * currentMod;
};

}

#endif

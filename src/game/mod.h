#ifndef _paintown_mod_h
#define _paintown_mod_h

#include <string>
#include <vector>

#include "util/load_exception.h"
#include "util/file-system.h"

namespace Level{
    class LevelInfo;
}

/* encapsulates a configuration for a game */
namespace Paintown{

class Mod{
public:
    Mod(const Filesystem::AbsolutePath & path) throw (LoadException);

    const std::string & getMenu();

    std::vector<Level::LevelInfo> & getLevels();

    virtual ~Mod();

    static void loadMod(const std::string & path);
    static void loadDefaultMod();
    static Mod * getCurrentMod();
    
protected:
    std::vector<Level::LevelInfo> levels;
    std::string menu;

    static Mod * currentMod;
};

}

#endif

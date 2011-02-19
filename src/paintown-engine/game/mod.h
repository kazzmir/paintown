#ifndef _paintown_mod_h
#define _paintown_mod_h

#include <string>
#include <vector>

#include "util/load_exception.h"
#include "util/file-system.h"

namespace Level{
    class LevelInfo;
}

namespace Graphics{
class Bitmap;
}

/* encapsulates a configuration for a game */
namespace Paintown{

class Mod{
public:
    Mod(const Filesystem::AbsolutePath & path) throw (LoadException);

    virtual const std::string getMenu();
    /* use the heap */
    virtual Graphics::Bitmap * createBitmap(const Filesystem::RelativePath & path);

    /* use the stack */
    virtual Graphics::Bitmap makeBitmap(const Filesystem::RelativePath & path);

    virtual std::vector<Level::LevelInfo> getLevels();

    virtual ~Mod();

    virtual Filesystem::AbsolutePath selectPlayer(const std::string & message, const Level::LevelInfo & info, int & remap);

    static void loadPaintownMod(const std::string & path);
    static void loadOpenborMod(const Filesystem::AbsolutePath & path);
    static void loadDefaultMod();
    static Mod * getCurrentMod();
    
protected:
    Mod();

    static void setMod(Mod * mod);

    std::vector<Level::LevelInfo> levels;
    std::string menu;

    static Mod * currentMod;
};

}

#endif

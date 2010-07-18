#ifndef _paintown_openbor_mod_h
#define _paintown_openbor_mod_h

#include "game/mod.h"
#include "util/file-system.h"
#include "pack-reader.h"

namespace Paintown{

class OpenborMod: public Mod {
public:
    OpenborMod(const Filesystem::AbsolutePath & path);

    const std::string getMenu();
    virtual Filesystem::AbsolutePath selectPlayer(const std::string & message, const Level::LevelInfo & info, int & remap);
    virtual std::vector<Level::LevelInfo> getLevels();
    virtual Bitmap * createBitmap(const Filesystem::RelativePath & path);

    Bor::PackReader reader;
};

}

#endif

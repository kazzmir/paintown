#ifndef _paintown_openbor_mod_h
#define _paintown_openbor_mod_h

#include "paintown-engine/game/mod.h"
#include "r-tech1/file-system.h"
#include "pack-reader.h"

namespace Paintown{

class OpenborMod: public Mod {
public:
    OpenborMod(const Filesystem::AbsolutePath & path);

    const std::string getMenu();
    virtual Filesystem::AbsolutePath selectPlayer(const std::string & message, const Level::LevelInfo & info, int & remap, const InputSource & source);
    virtual std::vector<Level::LevelInfo> getLevels();
    virtual Graphics::Bitmap * createBitmap(const Filesystem::RelativePath & path, bool keepInMemory=false);

    Bor::PackReader reader;
};

}

#endif

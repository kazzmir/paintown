#ifndef _paintown_openbor_mod_h
#define _paintown_openbor_mod_h

#include "game/mod.h"
#include "util/file-system.h"

namespace Paintown{

class OpenborMod: public Mod{
public:
    OpenborMod(const Filesystem::AbsolutePath & path);
};

}

#endif

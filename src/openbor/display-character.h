#ifndef _paintown_openbor_display_character_h
#define _paintown_openbor_display_character_h

#include <string>
#include "paintown-engine/object/display_character.h"

namespace Bor{

class PackReader;

class DisplayCharacter: public Paintown::DisplayCharacter {
public:
    DisplayCharacter(PackReader & reader, std::string file);
protected:
    /* called by the loader */
    virtual void load();
private:
    PackReader & reader;
};

}

#endif

#ifndef character_select_h
#define character_select_h

#include <string>

#include "util/gui/select-list.h"

class Token;

namespace Gui{
    class Animation;
}

class CharacterSelect{
public:
    CharacterSelect();
    CharacterSelect(std::string &);
    CharacterSelect(Token *);
    virtual ~CharacterSelect();
    
    virtual inline Gui::GridSelect & getGrid() {
        return this->grid;
    }
    
protected:
    void load(Token *);
    Gui::GridSelect grid;
    std::string name;
};

#endif
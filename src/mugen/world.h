#ifndef _paintown_mugen_world_h
#define _paintown_mugen_world_h

#include "common.h"
#include "character-state.h"
#include <map>

namespace Mugen{

class Character;

/* Maintains a snapshot of all state data */
class World{
public:
    World();
    virtual ~World();

    void addCharacter(const Character & who);

protected:
    std::map<CharacterId, StateData> characterData;
};

}

#endif

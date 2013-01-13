#ifndef _paintown_mugen_world_h
#define _paintown_mugen_world_h

#include "common.h"
#include "character-state.h"
#include "stage-state.h"
#include "random.h"
#include <map>

namespace Mugen{

class Character;

/* Maintains a snapshot of all state data */
class World{
public:
    World();
    virtual ~World();

    void addCharacter(const Character & who);
    void setStageData(const StageStateData & data);
    void setRandom(const Random & random);
    void setGameTime(int gameTime);

    const StageStateData & getStageData() const;
    const Random & getRandom() const;
    int getGameTime() const;

    const std::map<CharacterId, StateData> & getCharacterData() const;

protected:
    std::map<CharacterId, StateData> characterData;
    StageStateData stageData;
    Random random;

    int gameTime;
};

}

#endif

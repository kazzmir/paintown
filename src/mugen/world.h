#ifndef _paintown_mugen_world_h
#define _paintown_mugen_world_h

#include "common.h"
#include "character-state.h"
#include "stage-state.h"
#include "random.h"
#include <map>

class Token;

namespace Mugen{

class Character;

struct AllCharacterData{
    AllCharacterData(const StateData & character, const AnimationState & animation, const std::map<int, std::map<uint32_t, int> > & statePersistent);
    AllCharacterData();

    StateData character;
    AnimationState animation;
    std::map<int, std::map<uint32_t, int> > statePersistent;
};

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

    const std::map<CharacterId, AllCharacterData> & getCharacterData() const;

    bool operator==(const World & him) const;
    bool operator!=(const World & him) const;

    Token * serialize() const;
    static World * deserialize(Token * token);

protected:
    std::map<CharacterId, AllCharacterData> characterData;
    StageStateData stageData;
    Random random;

    int gameTime;
};

}

#endif

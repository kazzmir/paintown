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
    World(const World & copy);
    virtual ~World();

    void addCharacter(const Character & who);
    void addStagePlayerData(const CharacterId & id, const PlayerData & data);
    void setStageData(const StageStateData & data);
    void setRandom(const Random & random);
    void setGameInfo(Token * token);

    const StageStateData & getStageData() const;
    const Random & getRandom() const;
    const Token * getGameInfo() const;

    const std::map<CharacterId, AllCharacterData> & getCharacterData() const;
    const std::map<CharacterId, PlayerData> & getStagePlayerData() const;

    bool operator==(const World & him) const;
    bool operator!=(const World & him) const;

    Token * serialize() const;
    static World * deserialize(Token * token);

protected:
    std::map<CharacterId, AllCharacterData> characterData;
    std::map<CharacterId, PlayerData> stagePlayerData;
    StageStateData stageData;
    Random random;
    Token * gameInfo;
};

}

#endif

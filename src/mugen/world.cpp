#include "world.h"
#include "character.h"

namespace Mugen{

World::World(){
}

World::~World(){
}

void World::addCharacter(const Character & who){
    characterData[who.getId()] = who.getStateData();
}
    
void World::setGameTime(int gameTime){
    this->gameTime = gameTime;
}

int World::getGameTime() const {
    return gameTime;
}
    
void World::setStageData(const StageStateData & data){
    this->stageData = data;
}

void World::setRandom(const Random & random){
    this->random = random;
}
    
const StageStateData & World::getStageData() const {
    return stageData;
}

const Random & World::getRandom() const {
    return random;
}
    
const std::map<CharacterId, StateData> & World::getCharacterData() const {
    return this->characterData;
}

}

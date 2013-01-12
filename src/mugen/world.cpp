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
    
void World::setStageData(const StageStateData & data){
    this->stageData = data;
}

void World::setRandom(const Random & random){
    this->random = random;
}

}

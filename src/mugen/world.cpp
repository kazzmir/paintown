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

}

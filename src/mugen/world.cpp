#include "world.h"
#include "character.h"
#include "util/token.h"
#include <vector>
#include <string>
#include <map>

using std::vector;
using std::string;
using std::map;

namespace Mugen{

World::World(){
}

World::~World(){
}
        
AllCharacterData::AllCharacterData(const StateData & character, const AnimationState & animation):
character(character),
animation(animation){
}
    
AllCharacterData::AllCharacterData(){
}

void World::addCharacter(const Character & who){
    characterData[who.getId()] = AllCharacterData(who.getStateData(), who.getCurrentAnimationState());
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
    
const std::map<CharacterId, AllCharacterData> & World::getCharacterData() const {
    return this->characterData;
}

static Token * serialize(const AllCharacterData & data){
    Token * token = new Token();
    *token << "data";
    *token << serialize(data.character);
    *token << serialize(data.animation);
    return token;
}

static Token * serialize(const std::map<CharacterId, AllCharacterData> & characters){
    Token * token = new Token();
    *token << "characters";
    for (std::map<CharacterId, AllCharacterData>::const_iterator it = characters.begin(); it != characters.end(); it++){
        Token * character = new Token();
        *character << it->first.intValue();
        *character << serialize(it->second);
        *token << character;
    }
    return token;
}

static Token * serialize(const StageStateData & stage){
    Token * token = new Token();
    *token << "stage";

    return token;
}

static Token * serialize(const Random & random){
    Token * token = new Token();
    *token << "random";
    return token;
}

Token * World::serialize() const {
    Token * head = new Token();

    *head << "mugen-state";
    *head << Mugen::serialize(characterData);
    *head << Mugen::serialize(stageData);
    *head << Mugen::serialize(random);

    *head->newToken() << "game-time" << gameTime;

    return head;
}

World World::deserialize(Token * token){
    World out;

    return out;
}

}

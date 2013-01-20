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
        
AllCharacterData::AllCharacterData(const StateData & character, const AnimationState & animation, const std::map<int, std::map<unsigned int, int> > & statePersistent):
character(character),
animation(animation),
statePersistent(statePersistent){
}
    
AllCharacterData::AllCharacterData(){
}

void World::addCharacter(const Character & who){
    characterData[who.getId()] = AllCharacterData(who.getStateData(), who.getCurrentAnimationState(), who.getStatePersistent());
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

static Token * serialize(const map<int, map<unsigned int, int> > & statePersistent){
    Token * token = new Token();

    *token << "states";
    for (map<int, map<unsigned int, int> >::const_iterator it = statePersistent.begin(); it != statePersistent.end(); it++){
        const map<unsigned int, int> & controllers = it->second;
        Token * out = token->newToken();
        *out << it->first;
        for (map<unsigned int, int>::const_iterator it = controllers.begin(); it != controllers.end(); it++){
            Token * what = out->newToken();
            *what << it->first << it->second;
        }
    }

    return token;
}

static Token * serialize(const AllCharacterData & data){
    Token * token = new Token();
    *token << "data";
    *token << serialize(data.character);
    *token << serialize(data.animation);
    *token << serialize(data.statePersistent);
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
    return random.serialize();
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

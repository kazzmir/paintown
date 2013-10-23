#include "world.h"
#include "character.h"
#include "util/token.h"
#include "constraint.h"
#include <vector>
#include <string>
#include <sstream>
#include <map>

using std::vector;
using std::string;
using std::map;

namespace Mugen{

World::World():
gameInfo(NULL){
}
    
World::World(const World & copy):
characterData(copy.characterData),
stageData(copy.stageData),
random(copy.random),
gameInfo(NULL){
    if (copy.gameInfo != NULL){
        gameInfo = copy.gameInfo->copy();
    }
}

World::~World(){
    delete gameInfo;
}

AllCharacterData::AllCharacterData(const StateData & character, const AnimationState & animation, const std::map<int, std::map<uint32_t, int> > & statePersistent):
character(character),
animation(animation),
statePersistent(statePersistent){
}
    
AllCharacterData::AllCharacterData(){
}

void World::addCharacter(const Character & who){
    characterData[who.getId()] = AllCharacterData(who.getStateData(), who.getCurrentAnimationState(), who.getStatePersistent());
    AllCharacterData & data = characterData[who.getId()];
    const std::vector<Command2 *> & commands = who.getCommands();
    std::map<std::string, std::string > & commandState = data.character.commandState;
    for (vector<Command2*>::const_iterator it = commands.begin(); it != commands.end(); it++){
        Command2 * command = *it;
        Token * serialized = command->serialize();
        commandState[command->getName()] = serialized->toStringCompact();
        delete serialized;
    }
}
    
void World::setGameInfo(Token * token){
    delete gameInfo;
    gameInfo = token;
}

const Token * World::getGameInfo() const {
    return gameInfo;
}

void World::addStagePlayerData(const CharacterId & id, const PlayerData & data){
    stagePlayerData[id] = data;
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
    
const std::map<CharacterId, PlayerData> & World::getStagePlayerData() const {
    return this->stagePlayerData;
}

static Token * serialize(const map<int, map<uint32_t, int> > & statePersistent){
    Token * token = new Token();

    *token << "states";
    for (map<int, map<uint32_t, int> >::const_iterator it = statePersistent.begin(); it != statePersistent.end(); it++){
        const map<uint32_t, int> & controllers = it->second;
        Token * out = token->newToken();
        *out << it->first;
        for (map<uint32_t, int>::const_iterator it = controllers.begin(); it != controllers.end(); it++){
            Token * what = out->newToken();
            *what << it->first << it->second;
        }
    }

    return token;
}

static map<int, map<uint32_t, int> > deserializeStates(const Token * data){
    map<int, map<uint32_t, int> > out;
    /* TODO */

    return out;
}

static Token * serialize(const AllCharacterData & data){
    Token * token = new Token();
    *token << "data";
    *token << serialize(data.character);
    *token << serialize(data.animation);
    *token << serialize(data.statePersistent);
    return token;
}

static AllCharacterData deserializeAllCharacterData(const Token * token){
    AllCharacterData out;
    const Token * character = token->findToken("data/StateData");
    if (character != NULL){
        out.character = deserializeStateData(character);
    }

    const Token * animation = token->findToken("data/AnimationState");
    if (animation != NULL){
        out.animation = deserializeAnimationState(animation);
    }

    const Token * statePersistent = token->findToken("data/states");
    if (statePersistent != NULL){
        out.statePersistent = deserializeStates(statePersistent);
    }

    return out;
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

static Token * serialize(const Random & random){
    return random.serialize();
}

static Token * serialize(const std::map<CharacterId, PlayerData> & stagePlayerData){
    Token * token = new Token();
    *token << "stage-player-info";
    for (std::map<CharacterId, PlayerData>::const_iterator it = stagePlayerData.begin(); it != stagePlayerData.end(); it++){
        Token * info = new Token();
        *info << it->first.intValue();
        *info << serialize(it->second);
        *token << info;
    }
    return token;
}

Token * World::serialize() const {
    Token * head = new Token();

    *head << "mugen-state";
    *head << Mugen::serialize(characterData);
    *head << Mugen::serialize(stageData);
    *head << Mugen::serialize(random);
    *head << Mugen::serialize(stagePlayerData);

    if (gameInfo != NULL){
        *head << gameInfo->copy();
    }

    return head;
}

static int integer(const string & what){
    int out = 0;
    std::istringstream data(what);
    data >> out;
    return out;
}

static map<CharacterId, AllCharacterData> deserializeCharacters(const Token * data){
    map<CharacterId, AllCharacterData> out;
    TokenView look = data->view();
    while (look.hasMore()){
        const Token * use = NULL;
        look >> use;
        int id = integer(use->getName());

        const Token * tokenData = NULL;
        use->view() >> tokenData;

        out[CharacterId(id)] = deserializeAllCharacterData(tokenData);
    }
    return out;
}
    
std::map<CharacterId, PlayerData> deserializeStagePlayerInfo(const Token * data){
    map<CharacterId, PlayerData> out;
    TokenView look = data->view();
    while (look.hasMore()){
        const Token * use = NULL;
        look >> use;
        int id = integer(use->getName());

        const Token * tokenData = NULL;
        use->view() >> tokenData;

        out[CharacterId(id)] = deserializePlayerData(tokenData);
    }
    return out;
}

World * World::deserialize(Token * token){
    World * out = new World();

    const Token * characters = token->findToken("_/characters");
    if (characters != NULL){
        out->characterData = deserializeCharacters(characters);
    }

    const Token * stage = token->findToken("_/StageStateData");
    if (stage != NULL){
        out->stageData = deserializeStageStateData(stage);
    }

    const Token * random = token->findToken("_/random");
    if (random != NULL){
        out->random = Random::deserialize(random);
    }

    const Token * gameInfo = token->findToken("_/game-info");
    if (gameInfo != NULL){
        out->gameInfo = gameInfo->copy();
    }

    const Token * playerInfoToken = token->findToken("_/stage-player-info");
    if (playerInfoToken != NULL){
        out->stagePlayerData = deserializeStagePlayerInfo(playerInfoToken);
    }

    return out;
}

/* Checks that the serialized version matches. This depends on serialization being right */
bool World::operator==(const World & him) const {
    Token * meToken = serialize();
    Token * himToken = him.serialize();
    bool out = meToken->toString() == himToken->toString();
    delete meToken;
    delete himToken;
    return out;
}

bool World::operator!=(const World & him) const {
    return !(*this == him);
}

}

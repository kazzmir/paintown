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

static Token * serialize(const AnimationState & data){
    Token * token = new Token();
    *token << "animation";

    *token->newToken() << "position" << data.position;
    *token->newToken() << "looped" << data.looped;
    *token->newToken() << "started" << data.started;
    *token->newToken() << "ticks" << data.ticks;
    *token->newToken() << "virtual-ticks" << data.virtual_ticks;

    return token;
}

static Token * serialize(const std::string & name, const std::map<int, RuntimeValue > & data){
    Token * token = new Token();
    *token << name;
    for (std::map<int, RuntimeValue>::const_iterator it = data.begin(); it != data.end(); it++){
        int index = it->first;
        const RuntimeValue & value = it->second;
        switch (value.getType()){
            case RuntimeValue::Invalid: {
                break;
            }
            case RuntimeValue::Bool: {
                *token->newToken() << index << "bool" << value.getBoolValue();
                break;
            }
            case RuntimeValue::String: {
                *token->newToken() << index << "string" << value.getStringValue();
                break;
            }
            case RuntimeValue::Double: {
                *token->newToken() << index << "double" << value.getDoubleValue();
                break;
            }
            case RuntimeValue::ListOfString: {
                Token * use = token->newToken();
                *use << index << "list-string";
                for (vector<string>::const_iterator it = value.strings_value.begin(); it != value.strings_value.end(); it++){
                    *use << *it;
                }
                break;
            }
            case RuntimeValue::RangeType: {
                Token * use = token->newToken();
                *use << index << "range" << value.range.low << value.range.high;
                break;
            }
            case RuntimeValue::StateType: {
                Token * use = token->newToken();
                *use << index << "state" <<
                    value.attribute.standing <<
                    value.attribute.crouching <<
                    value.attribute.lying <<
                    value.attribute.aerial;
                break;
            }
            case RuntimeValue::AttackAttribute: {
                Token * use = token->newToken();
                *use << index << "attack";
                for (vector<AttackType::Attribute>::const_iterator it = value.attackAttributes.begin(); it != value.attackAttributes.end(); it++){
                    *use << *it;
                }
                break;
            }
            case RuntimeValue::ListOfInt: {
                Token * use = token->newToken();
                *use << index << "ints";
                for (vector<int>::const_iterator it = value.ints_value.begin(); it != value.ints_value.end(); it++){
                    *use << *it;
                }

                break;
            }
        }
    }
    return token;
}

static Token * serialize(const StateData & data){
    Token * token = new Token();
    *token << "character";

#define T(x) *token->newToken() << #x << data.x

    T(juggleRemaining);
    T(currentJuggle);
    T(currentState);
    T(previousState);
    T(currentAnimation);
    T(velocity_x);
    T(velocity_y);
    T(has_control);
    T(stateTime);
    T(currentPhysics);
    T(combo);
    T(hitCount);
    T(blocking);
    T(guarding);
    T(reversalActive);
    T(defenseMultiplier);
    T(attackMultiplier);
    T(frozen);
    T(pushPlayer);
    T(health);
    T(spritePriority);
    T(wasHitCounter);
    T(drawAngle);
    T(virtualx);
    T(virtualy);
    T(virtualz);
    T(facing);
    T(power);

    *token << serialize("variables", data.variables);
    *token << serialize("floatVariables", data.floatVariables);
    *token << serialize("systemVariables", data.systemVariables);
    
    T(stateType);
    T(moveType);

    /*
    HitDefinition hit;
    HitState hitState;
    
    struct WidthOverride{
        WidthOverride(){
            enabled = false;
            edgeFront = 0;
            edgeBack = 0;
            playerFront = 0;
            playerBack = 0;
        }
    
        bool enabled;
        int edgeFront;
        int edgeBack;
        int playerFront;
        int playerBack;
    } widthOverride;
    
    
    struct HitByOverride{
        HitByOverride(){
            standing = false;
            crouching = false;
            aerial = false;
            time = 0;
        }
    
        bool standing;
        bool crouching;
        bool aerial;
        int time;
        std::vector<AttackType::Attribute > attributes;
    } hitByOverride[2];
    
    ReversalData reversal;
    
    struct TransOverride{
        TransOverride(){
            enabled = false;
            alphaSource = 0;
            alphaDestination = 0;
        }
    
        bool enabled;
        TransType type;
        int alphaSource;
        int alphaDestination;
    } transOverride;
    
    
    struct SpecialStuff{
        SpecialStuff(){
            invisible = false;
            intro = false;
        }
    
        bool invisible;
        bool intro;
    } special;
    
    struct Bind{
        Bind(){
            time = 0;
            facing = 0;
            offsetX = 0;
            offsetY = 0;
        }
    
        CharacterId bound;
        int time;
        int facing;
        double offsetX;
        double offsetY;
    } bind;
    
    std::map<int, std::vector<CharacterId > > targets;
    
    struct CharacterData{
        CharacterData(){
            enabled = false;
        }
    
        CharacterId who;
        bool enabled;
    } characterData;
    
    struct DrawAngleEffect{
        DrawAngleEffect(){
            enabled = false;
            angle = 0;
            scaleX = 0;
            scaleY = 0;
        }
    
        bool enabled;
        double angle;
        double scaleX;
        double scaleY;
    } drawAngleData;
    
    std::vector<std::string > active;
    std::map<int, HitOverride > hitOverrides;
        */

#undef T

    return token;
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

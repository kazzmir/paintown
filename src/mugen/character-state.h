
#ifndef _serialize_Mugen_d9d6119a34afd096d0f6a858c325c80e
#define _serialize_Mugen_d9d6119a34afd096d0f6a858c325c80e

#include "common.h"
#include "compiler.h"
#include <map>

namespace Mugen{

struct StateData{
    StateData(){
        juggleRemaining = 0;
        currentJuggle = 0;
        currentState = 0;
        previousState = 0;
        currentAnimation = 0;
        velocity_x = 0;
        velocity_y = 0;
        has_control = false;
        stateTime = 0;
        combo = 0;
        hitCount = 0;
        blocking = false;
        guarding = false;
        defenseMultiplier = 0;
        attackMultiplier = 0;
        frozen = false;
        reversalActive = false;
        pushPlayer = 0;
        health = 0;
        spritePriority = 0;
        wasHitCounter = 0;
        drawAngle = 0;
        virtualx = 0;
        virtualy = 0;
        virtualz = 0;
        power = 0;
    }

    int juggleRemaining;
    int currentJuggle;
    int currentState;
    int previousState;
    int currentAnimation;
    double velocity_x;
    double velocity_y;
    bool has_control;
    int stateTime;
    std::map<int, RuntimeValue > variables;
    std::map<int, RuntimeValue > floatVariables;
    std::map<int, RuntimeValue > systemVariables;
    Physics::Type currentPhysics;
    std::string stateType;
    std::string moveType;
    HitDefinition hit;
    HitState hitState;
    int combo;
    int hitCount;
    bool blocking;
    bool guarding;
    
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
    
    double defenseMultiplier;
    double attackMultiplier;
    bool frozen;
    ReversalData reversal;
    bool reversalActive;
    
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
    
    int pushPlayer;
    
    struct SpecialStuff{
        SpecialStuff(){
            invisible = false;
            intro = false;
        }
    
        bool invisible;
        bool intro;
    } special;
    
    double health;
    
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
    int spritePriority;
    unsigned int wasHitCounter;
    
    struct CharacterData{
        CharacterData(){
            enabled = false;
        }
    
        CharacterId who;
        bool enabled;
    } characterData;
    
    double drawAngle;
    
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
    double virtualx;
    double virtualy;
    double virtualz;
    Facing facing;
    double power;
};

}

#endif



#ifndef _serialize_Mugen_0cf611f40fdec23bd9a6821dec031abe
#define _serialize_Mugen_0cf611f40fdec23bd9a6821dec031abe

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
    
};

}

#endif


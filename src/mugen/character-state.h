
#ifndef _serialize_Mugen_72816d3e6c70a5177f67751565159b1c
#define _serialize_Mugen_72816d3e6c70a5177f67751565159b1c

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
};

}

#endif


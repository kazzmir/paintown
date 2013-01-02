
#ifndef _serialize_Mugen_6a1de65a08ddc09c180b7a6b9bf898ec
#define _serialize_Mugen_6a1de65a08ddc09c180b7a6b9bf898ec

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
};

}

#endif


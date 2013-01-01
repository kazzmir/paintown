
#ifndef _serialize_Mugen_8f9d609bacb130c8d682d9332de0b59d
#define _serialize_Mugen_8f9d609bacb130c8d682d9332de0b59d

#include "common.h"

namespace Mugen{

struct StateData{
    StateData(){
        juggleRemaining = 0;
        currentJuggle = 0;
        currentState = 0;
        previousState = 0;
        currentAnimation = 0;
    }

    int juggleRemaining;
    int currentJuggle;
    int currentState;
    int previousState;
    int currentAnimation;
};

}

#endif


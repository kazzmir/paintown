
#ifndef _serialize_Mugen_0a020724969b38e0f98cea7f858a867e
#define _serialize_Mugen_0a020724969b38e0f98cea7f858a867e

#include "common.h"

namespace Mugen{

struct StateData{
    StateData(){
        juggleRemaining = 0;
        currentJuggle = 0;
    }

    int juggleRemaining;
    int currentJuggle;
};

}

#endif


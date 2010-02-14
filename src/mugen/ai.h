#ifndef _paintown_mugen_ai
#define _paintown_mugen_ai

#include "character.h"
#include <vector>
#include <string>

namespace Mugen{

class AI: public Character {
public:
    AI();
    
    /* ideas for input:
     *  randomly choose commands. when a hit occurs, note the state of the environment
     *  the character was in and keep track of that. use this information to help choose
     *  new moves in the future.
     *
     *  learning AI.
     */
    virtual std::vector<std::string> doInput();

    virtual ~AI();
};

}

#endif

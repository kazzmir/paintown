#ifndef _paintown_c566d5b5f2dc27b39785c29f660bef1a
#define _paintown_c566d5b5f2dc27b39785c29f660bef1a

#include "Value.h"

namespace Ast{

class CompiledValue: public Value {
public:
    CompiledValue(){
    }

    virtual void walk(Walker & walker) const {
        walker.onCompiledValue(*this);
    }
};

}

#endif

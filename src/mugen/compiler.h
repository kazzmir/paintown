#ifndef _paintown_mugen_compiler_h
#define _paintown_mugen_compiler_h

#include "evaluator.h"

namespace Ast{
    class Value;
}

namespace Mugen{

namespace Compiler{

    class Value{
    public:
        Value();
        RuntimeValue evaluate(const Environment & environment);
        virtual ~Value();
    };

    Value * compile(Ast::Value * input);
}

}

#endif

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
        virtual RuntimeValue evaluate(const Environment & environment) = 0;
        virtual ~Value();
    };

    Value * compile(Ast::Value * input);
}

}

#endif

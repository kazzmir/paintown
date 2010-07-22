#ifndef _paintown_mugen_compiler_h
#define _paintown_mugen_compiler_h

#include "evaluator.h"

namespace Ast{
    class Value;
}

namespace Mugen{

    class Compiled{
    public:
        Compiled();
        RuntimeValue evaluate(const Environment & environment);
        virtual ~Compiled();
    };

    CompiledValue * compile(Ast::Value * input);
}

#endif

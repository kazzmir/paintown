#ifndef _paintown_mugen_compiler_h
#define _paintown_mugen_compiler_h

#include "evaluator.h"
#include <string>

namespace Ast{
    class Value;
}

namespace Mugen{

namespace Compiler{

    class Value{
    public:
        Value();
        virtual RuntimeValue evaluate(const Environment & environment) const = 0;
        virtual std::string toString() const;
        virtual Value * copy() const = 0;
        virtual ~Value();
    };

    /* deletes `input' */
    Value * compileAndDelete(const Ast::Value * input);

    /* does not delete `input' */
    Value * compile(const Ast::Value * input);
    Value * compile(const Ast::Value & input);
    Value * compile(int immediate);
    Value * compile(double immediate);
    Value * copy(Value * value);
}

}

#endif

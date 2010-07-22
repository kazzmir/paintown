#include "ast/all.h"
#include "compiler.h"
#include "exception.h"
#include <sstream>
    
namespace Mugen{
namespace Compiler{

namespace{

class CompileWalker: public Ast::Walker{
public:

    CompileWalker():
    compiled(NULL){
    }

    Value * compileIdentifier(const Ast::Identifier & identifier){
        if (identifier == "command"){
            class Command: public Value{
            public:
                RuntimeValue evaluate(const Environment & environment){
                    return RuntimeValue(environment.getCommands());
                }
            };
        }

        std::ostringstream out;
        out << "Don't know how to compile '" << identifier.toString() << "'";
        throw MugenException(out.str());
    }
    
    virtual void onIdentifier(const Ast::Identifier & identifier){
        compiled = compileIdentifier(identifier);
    }

    Value * compiled;
};

}

Value * compile(Ast::Value * input){
    CompileWalker compiler;
    input->walk(compiler);
    if (compiler.compiled == NULL){
        std::ostringstream out;
        out << "Unable to compile expression '" << input->toString() << "'";
        throw MugenException(out.str());
    }
    return compiler.compiled;
}

}
}

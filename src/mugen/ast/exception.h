#ifndef _paintown_ast_exception_h
#define _paintown_ast_exception_h

#include <exception>
#include "../mugen_exception.h"
#include <string>

namespace Ast{
class Exception: public MugenException {
public:
    Exception(const std::string & str):
        MugenException(str){
    }

    virtual ~Exception() throw (){
    }

};
}

#endif

#ifndef _paintown_ast_exception_h
#define _paintown_ast_exception_h

#include <exception>
#include "../exception.h"
#include <string>

namespace Ast{
class Exception: public MugenException {
public:
    Exception(const std::string & str):
        /* FIXME: __FILE__ and __LINE__ should be passed in */
        MugenException(str, __FILE__, __LINE__){
    }

    virtual ~Exception() throw (){
    }

};
}

#endif

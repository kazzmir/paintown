#ifndef _paintown_ast_exception_h
#define _paintown_ast_exception_h

#include <exception>
#include <string>

namespace Ast{
class Exception: public std::exception {
public:
    Exception(const std::string & str):
        exception(),
        reason(str){
        }

    const std::string & getReason() const {
        return reason;
    }

    virtual ~Exception() throw (){
    }

protected:
    std::string reason;
};
}

#endif

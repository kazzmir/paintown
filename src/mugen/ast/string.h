#ifndef _paintown_ast_string_h_
#define _paintown_ast_string_h_

#include "Value.h"
#include <string>
#include <sstream>

namespace Ast{

class String: public Value{
public:
    String(const std::string * str):
    str(str){
    }

    virtual std::string toString() const {
        return "\"" + *str + "\"";
    }

    virtual ~String(){
    }
protected:
    const std::string * str;
};

}

#endif

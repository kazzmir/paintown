#ifndef _paintown_ast_string_h_
#define _paintown_ast_string_h_

#include "Value.h"
#include <string>

namespace Ast{

class String: public Value{
public:
    String(const std::string * str):
    str(str){
    }

    virtual std::string toString() const {
        return "\"" + *str + "\"";
    }

    using Value::operator>>;
    virtual const Value & operator>>(std::string & str) const {
        str = *(this->str);
        return *this;
    }
    
    virtual std::string getType() const {
        return "string";
    }
    
    virtual bool referenced(const void * value) const {
        return Value::referenced(value) ||
               str == value;
    }

    virtual ~String(){
        delete str;
    }

protected:
    const std::string * str;
};

}

#endif

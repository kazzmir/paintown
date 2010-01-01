#ifndef _paintown_ast_filename_h_
#define _paintown_ast_filename_h_

#include "Value.h"
#include <string>

namespace Ast{

class Filename: public Value{
public:
    Filename(const std::string * str):
    str(str){
    }

    virtual std::string toString() const {
        return *str;
    }

    using Value::operator>>;

    virtual const Value & operator>>(std::string & str) const {
        str = *(this->str);
        return *this;
    }
    
    virtual Element * copy() const {
        throw Exception("Not implemented yet");
    }

    virtual std::string getType() const {
        return "string";
    }
    
    virtual void mark(std::map<const void*, bool> & marks) const {
        marks[this] = true;
        marks[str] = true;
    }

    /*
    virtual bool referenced(const void * value) const {
        return Value::referenced(value) ||
               value == str;
    }
    */

    virtual ~Filename(){
        delete str;
    }

protected:
    const std::string * str;
};

}

#endif

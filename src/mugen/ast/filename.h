#ifndef _paintown_ast_filename_h_
#define _paintown_ast_filename_h_

#include "Value.h"
#include <string>
#include "util/funcs.h"

namespace Ast{

class Filename: public Value {
public:
    Filename(const std::string * str):
    str(str){
    }

    virtual std::string toString() const {
        return Util::trim(*str);
    }

    using Value::operator>>;

    virtual const Value & operator>>(std::string & str) const {
        str = Util::trim(*(this->str));
        return *this;
    }
    
    virtual Element * copy() const {
        throw Exception("Not implemented yet");
    }

    virtual std::string getType() const {
        return "filename";
    }

    Token * serialize() const {
        Token * token = new Token();
        *token << SERIAL_FILENAME << *str;
        return token;
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

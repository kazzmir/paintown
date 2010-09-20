#ifndef _paintown_ast_string_h_
#define _paintown_ast_string_h_

#include "Value.h"
#include <string>

namespace Ast{

class String: public Value {
public:
    String(const std::string * str):
    str(str){
    }
    
    virtual void walk(Walker & walker) const {
        walker.onString(*this);
    }

    virtual std::string toString() const {
        return "\"" + *str + "\"";
    }

    using Value::operator>>;
    virtual const Value & operator>>(std::string & str) const {
        str = *(this->str);
        return *this;
    }

    using Element::operator==;
    virtual bool operator==(const Value & him) const {
        return him == *this;
    }

    virtual bool operator==(const String & him) const {
        return *str == *him.str;
    }

    Token * serialize() const {
        Token * token = new Token();
        *token << SERIAL_STRING << *str;
        return token;
    }

    static String * deserialize(Token * token){
        std::string out;
        *token >> out;
        return new String(new std::string(out));
    }
    
    virtual std::string getType() const {
        return "string";
    }
    
    virtual Element * copy() const {
        return new String(new std::string(*str));
    }
    
    /*
    virtual bool referenced(const void * value) const {
        return Value::referenced(value) ||
               str == value;
    }
    */

    virtual void mark(std::map<const void*, bool> & marks) const {
        marks[this] = true;
        marks[str] = true;
    }

    virtual ~String(){
        delete str;
    }

protected:
    const std::string * str;
};

}

#endif

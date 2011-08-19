#ifndef _paintown_ast_string_h_
#define _paintown_ast_string_h_

#include "Value.h"
#include <string>

namespace Ast{

class String: public Value {
public:
    String(int line, int column, const std::string * str):
    Value(line, column),
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
        *token << SERIAL_STRING << getLine() << getColumn() << *str;
        return token;
    }

    static String * deserialize(const Token * token){
        std::string out;
        int line, column;
        token->view() >> line >> column >> out;
        return new String(line, column, new std::string(out));
    }
    
    virtual std::string getType() const {
        return "string";
    }
    
    virtual Element * copy() const {
        return new String(getLine(), getColumn(), new std::string(*str));
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

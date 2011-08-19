#ifndef _paintown_ast_filename_h_
#define _paintown_ast_filename_h_

#include "Value.h"
#include <string>
#include "util/funcs.h"

namespace Ast{

class Filename: public Value {
public:
    Filename(int line, int column, const std::string * str):
    Value(line, column),
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
        return new Filename(getLine(), getColumn(), new std::string(*str));
    }

    virtual std::string getType() const {
        return "filename";
    }

    Token * serialize() const {
        Token * token = new Token();
        *token << SERIAL_FILENAME << getLine() << getColumn() << *str;
        return token;
    }

    static Filename * deserialize(const Token * token){
        std::string out;
        int line, column;
        token->view() >> line >> column >> out;
        return new Filename(line, column, new std::string(out));
    }

    using Element::operator==;
    virtual bool operator==(const Value & him) const {
        return him == *this;
    }

    virtual bool operator==(const Filename & him) const {
        return *str == *him.str;
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

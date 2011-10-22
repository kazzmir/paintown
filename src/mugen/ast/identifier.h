#ifndef _paintown_ast_identifier_h_
#define _paintown_ast_identifier_h_

#include <list>
#include <string>
#include <sstream>
#include <algorithm>
#include <map>

#include "Value.h"
#include "util/funcs.h"
#include "util/token.h"

namespace Ast{

class Identifier: public Value {
public:
    static int lowerCase( int c ){
        return tolower(c);
    }

    static const std::string downcase(std::string str){
        std::transform(str.begin(), str.end(), str.begin(), lowerCase);
        return str;
    }

    Identifier(int line, int column, const std::list<std::string> & names):
    Value(line, column),
    names(names){
        stringed = toStringSlow();
        stringedLower = downcase(stringed);
    }

    Identifier(const std::list<std::string> & names):
    Value(-1, -1),
    names(names){
        stringed = toStringSlow();
        stringedLower = downcase(stringed);
    }

    
    virtual Element * copy() const {
        return new Identifier(line, column, names);
    }
    
    virtual void walk(Walker & walker) const {
        walker.onIdentifier(*this);
    }

    static Identifier * deserialize(const Token * token);

    using Element::operator==;
    virtual bool operator==(const Value & him) const {
        return him == *this;
    }

    virtual bool operator==(const Identifier & him) const {
        return getLine() == him.getLine() &&
               getColumn() == him.getColumn() &&
               toString() == him.toString();
    }

    bool operator==(const std::string & str) const {
        return toLowerString() == downcase(str);
    }
    
    virtual std::string getType() const {
        return "identifier";
    }

    Token * serialize() const {
        Token * token = new Token();
        *token << SERIAL_IDENTIFIER << getLine() << getColumn() << toString();
        return token;
    }

    virtual std::string toString() const {
        return stringed;
    }

    virtual const std::string & toLowerString() const {
        return stringedLower;
    }

    virtual std::string toStringSlow() const {
        std::ostringstream out;
        bool first = true;
        for (std::list<std::string>::const_iterator it = names.begin(); it != names.end(); it++){
            if (!first){
                out << ".";
            } else {
                first = false;
            }
            out << *it;
        }
        return Util::trim(out.str());
    }
    
    /*
    virtual bool referenced(const void * value) const {
        if (value == this){
            return true;
        }
        
        for (std::list<std::string*>::const_iterator it = names.begin(); it != names.end(); it++){
            if (value == *it){
                return true;
            }
        }

        return false;
    }
    */
    
    virtual void mark(std::map<const void*, bool> & marks) const {
        marks[this] = true;
    }

    virtual ~Identifier(){
    }

protected:
    std::list<std::string> names;
    std::string stringed;
    std::string stringedLower;
    int line, column;
};

static std::list<std::string> toList(const std::string & s){
    std::list<std::string> out;
    out.push_back(s);
    return out;
}

class SimpleIdentifier: public Identifier {
public:
    SimpleIdentifier(const std::string & name):
    Identifier(-1, -1, toList(name)){
    }
    
    SimpleIdentifier(int line, int column, const std::string & name):
    Identifier(line, column, toList(name)){
    }
};

}

#endif

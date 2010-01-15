#ifndef _paintown_ast_identifier_h_
#define _paintown_ast_identifier_h_

#include <list>
#include <string>
#include <sstream>
#include <algorithm>
#include <map>

#include "Value.h"

namespace Ast{

class Identifier: public Value {
public:
    /* theres no real reason to use pointers to strings.. */
    Identifier(const std::list<std::string> & names):
    names(names){
    }

    static int lowerCase( int c ){
        return tolower(c);
    }
    
    virtual Element * copy() const {
        return new Identifier(names);
    }

    static std::string downcase(std::string str){
        std::transform(str.begin(), str.end(), str.begin(), lowerCase);
        return str;
    }
    
    virtual void walk(Walker & walker) const {
        walker.onIdentifier(*this);
    }

    bool operator==(const std::string & str) const {
        return downcase(toString()) == downcase(str);
    }
    
    virtual std::string getType() const {
        return "identifier";
    }

    virtual std::string toString() const {
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
        return out.str();
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
};

static std::list<std::string> toList(const std::string & s){
    std::list<std::string> out;
    out.push_back(s);
    return out;
}

class SimpleIdentifier: public Identifier {
public:
    SimpleIdentifier(const std::string & name):
    Identifier(toList(name)){
    }
};

}

#endif

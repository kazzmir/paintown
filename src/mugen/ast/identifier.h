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
    Identifier(const std::list<std::string*> & names):
    names(names){
    }

    static int lowerCase( int c ){
        return tolower(c);
    }

    static std::string downcase(std::string str){
        std::transform(str.begin(), str.end(), str.begin(), lowerCase);
        return str;
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
        for (std::list<std::string*>::const_iterator it = names.begin(); it != names.end(); it++){
            if (!first){
                out << ".";
            } else {
                first = false;
            }
            std::string * str = *it;
            out << *str;
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
        for (std::list<std::string*>::const_iterator it = names.begin(); it != names.end(); it++){
            marks[*it] = true;
        }
    }

    virtual ~Identifier(){
        for (std::list<std::string*>::iterator it = names.begin(); it != names.end(); it++){
            delete *it;
        }
    }

protected:
    std::list<std::string*> names;
};

static std::list<std::string*> toList(std::string * s){
    std::list<std::string*> out;
    out.push_back(s);
    return out;
}

class SimpleIdentifier: public Identifier {
public:
    SimpleIdentifier(const std::string & name):
    Identifier(toList(new std::string(name))){
    }
};

}

#endif

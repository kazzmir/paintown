#ifndef _paintown_ast_identifier_h_
#define _paintown_ast_identifier_h_

#include <list>
#include <string>
#include <sstream>

namespace Ast{

class Identifier{
public:
    Identifier(const std::list<std::string*> & names):
    names(names){
    }

    bool operator==(const std::string & str) const {
        return toString() == str;
    }

    std::string toString() const {
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

    virtual ~Identifier(){
        for (std::list<std::string*>::iterator it = names.begin(); it != names.end(); it++){
            delete *it;
        }
    }

protected:
    std::list<std::string*> names;
};

}

#endif

#ifndef _paintown_attribute_h
#define _paintown_attribute_h

#include "walker.h"
#include <map>
#include <string>

namespace Ast{

class Attribute{
public:
    enum Kind{
        None = 0,
        Simple = 1,
        /* to avoid name clash with the Keyword class */
        KeywordAttribute = 2,
        Array = 3,
    };

    Attribute(Kind kind):
    kind(kind){
    }
    
    virtual void walk(Walker & walker){
        walker.onAttribute(*this);
    }

    virtual Kind getKind(){
        return kind;
    }

    virtual void debugExplain(){
    }

    virtual std::string toString() const = 0;

    virtual void mark(std::map<const void*, bool> & marks) const {
        marks[this] = true;
    }

    /*
    virtual bool referenced(const void * value) const {
        return value == this;
    }
    */

    virtual ~Attribute(){
    }
protected:
    Kind kind;
};

}

#endif

#ifndef _paintown_attribute_h
#define _paintown_attribute_h

#include "walker.h"

namespace Ast{

class Attribute{
public:
    enum Kind{
        None = 0,
        Simple = 1,
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

    virtual bool referenced(const void * value) const {
        return value == this;
    }

    virtual ~Attribute(){
    }
protected:
    Kind kind;
};

}

#endif

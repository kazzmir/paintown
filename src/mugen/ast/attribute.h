#ifndef _paintown_attribute_h
#define _paintown_attribute_h

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

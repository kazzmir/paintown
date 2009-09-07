#ifndef _paintown_attribute_h
#define _paintown_attribute_h

namespace Ast{

class Attribute{
public:
    Attribute(){
    }

    virtual void debugExplain(){
    }

    virtual bool referenced(const void * value) const {
        return value == this;
    }

    virtual ~Attribute(){
    }
};

}

#endif

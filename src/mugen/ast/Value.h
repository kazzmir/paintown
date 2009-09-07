#ifndef _paintown_value_h
#define _paintown_value_h

namespace Ast{

class Value {
public:

    Value(){
    }

public:
    virtual std::string toString() const {
        return "";
    }

    virtual void debugExplain(){
    }

    virtual bool referenced(const void * value) const {
        return value == this;
    }

    virtual ~Value(){
    }
};

}

#endif

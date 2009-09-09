#ifndef _paintown_value_h
#define _paintown_value_h

#include <iostream>

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
        std::cout << toString() << std::endl;
    }

    virtual bool referenced(const void * value) const {
        return value == this;
    }

    virtual ~Value(){
    }
};

}

#endif

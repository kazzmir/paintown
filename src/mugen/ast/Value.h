#ifndef _paintown_value_h
#define _paintown_value_h

#include <iostream>
#include "exception.h"

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

    virtual const Value & operator>>(std::string & str) const {
        str = toString();
        return *this;
    }
    
    virtual const Value & operator>>(int & x) const {
        throw Exception("Cannot read an int");
        return *this;
    }

    virtual const Value & operator>>(double & x) const {
        throw Exception("Cannot read an int");
        return *this;
    }

    virtual bool referenced(const void * value) const {
        return value == this;
    }

    virtual ~Value(){
    }
};

}

#endif

#ifndef _paintown_value_h
#define _paintown_value_h

#include <iostream>
#include "walker.h"
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
    
    virtual void walk(Walker & walker){
        walker.onValue(*this);
    }

    virtual void debugExplain(){
        std::cout << toString() << std::endl;
    }

    virtual const Value & operator>>(std::string & str) const {
        str = toString();
        return *this;
    }
    
    virtual const Value & operator>>(int & x) const {
        fail("int");
        return *this;
    }

    virtual const Value & operator>>(double & x) const {
        fail("double");
        return *this;
    }
    
    virtual const Value & operator>>(bool & x) const {
        fail("bool");
        return *this;
    }

    virtual bool referenced(const void * value) const {
        return value == this;
    }

    virtual std::string getType() const = 0;

    virtual ~Value(){
    }

private:
    void fail(const std::string & what) const {
        throw Exception("Cannot read a " + what + " from a " + getType());
    }
};

}

#endif

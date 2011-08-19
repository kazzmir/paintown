#ifndef _paintown_value_h
#define _paintown_value_h

#include <iostream>
#include <map>
#include <vector>
#include "walker.h"
#include "exception.h"
#include "ast.h"

class Token;

namespace Ast{

class Value: public Element {
public:

    Value(int line, int column):
    Element(line, column){
    }

public:
    virtual std::string toString() const {
        return "";
    }
    
    virtual void walk(Walker & walker) const {
        walker.onValue(*this);
    }

    /* true if this value has multiple values in it
     * should only be true for value-list
     */
    virtual bool hasMultiple() const {
        return false;
    }

    /* resets the internals so that operator>> starts over */
    virtual void reset() const {
    }

    virtual void debugExplain(){
        std::cout << toString() << std::endl;
    }

    static Value * deserialize(const Token * token);

    /*
    using Element::operator==;
    virtual bool operator==(const Value & him) const {
        throw Exception("Override this method");
    }
    */
    
    virtual const Value & operator>>(const Value *& value) const {
        value = this;
        // fail("value");
        return *this;
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
    
    virtual const Value & operator>>(std::vector<int> & x) const {
        fail("bool");
        return *this;
    }
    
    virtual const Value & operator>>(std::vector<double> & x) const {
        fail("bool");
        return *this;
    }

    /*
    virtual bool referenced(const void * value) const {
        return value == this;
    }
    */
    
    virtual void mark(std::map<const void*, bool> & marks) const {
        marks[this] = true;
    }

    virtual std::string getType() const = 0;

    virtual ~Value(){
    }

protected:
    virtual void fail(const std::string & what) const {
        throw Exception("Cannot read a " + what + " from a " + getType());
    }
};

}

#endif

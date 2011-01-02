#ifndef _paintown_value_attribute_h
#define _paintown_value_attribute_h

#include <iostream>
#include <map>
#include <vector>
#include "attribute.h"
#include "walker.h"
#include "Value.h"
#include "exception.h"
#include "ast.h"

class Token;

namespace Ast{

class ValueAttribute: public Value {
public:

    ValueAttribute(Attribute * attribute):
    attribute(attribute){
    }

public:
    virtual std::string toString() const {
        return attribute->toString();
    }
    
    virtual void walk(Walker & walker) const {
        walker.onValue(*this);
        attribute->walk(walker);
    }

    /* resets the internals so that operator>> starts over */
    virtual void reset() const {
    }

    static ValueAttribute * deserialize(const Token * token){
        TokenView view = token->view();
        const Token * next;
        view >> next;
        Attribute * attribute = Attribute::deserialize(next);
        return new ValueAttribute(attribute);

    }
   
    virtual void mark(std::map<const void*, bool> & marks) const {
        marks[this] = true;
        attribute->mark(marks);
    }

    virtual std::string getType() const {
        return "value attribute";
    }

    virtual Element * copy() const {
        return new ValueAttribute((Attribute*) attribute->copy());
    }

    Token * serialize() const {
        Token * token = new Token();
        *token << SERIAL_VALUE_ATTRIBUTE;
        *token << attribute->serialize();
        return token;
    }

    virtual ~ValueAttribute(){
        delete attribute;
    }

    Attribute * attribute;
};

}

#endif

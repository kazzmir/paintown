#ifndef _paintown_ast_attribute_simple_h_
#define _paintown_ast_attribute_simple_h_

#include "attribute.h"
#include "identifier.h"
#include "Value.h"
#include <iostream>

namespace Ast{

class AttributeSimple: public Attribute {
public:
    
    AttributeSimple(const Identifier * name, const Value * value):
    name(name),
    value(value){
    }
    
    AttributeSimple(const Identifier * name):
    name(name),
    value(0){
    }

    void debugExplain(){
        if (value != 0){
            std::cout << name->toString() << " = " << value->toString() << std::endl;
        } else {
            std::cout << name->toString() << " = " << std::endl;
        }
    }

    virtual ~AttributeSimple(){
        delete name;
        delete value;
    }

protected:
    const Identifier * name;
    const Value * value;
};

}

#endif

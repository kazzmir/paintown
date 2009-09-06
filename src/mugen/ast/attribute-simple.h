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

    void debugExplain(){
        std::cout << name->toString() << " = " << value->toString() << std::endl;
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

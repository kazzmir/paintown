#ifndef _paintown_ast_attribute_simple_h_
#define _paintown_ast_attribute_simple_h_

#include "attribute.h"
#include "identifier.h"
#include "Value.h"
#include "walker.h"
#include <sstream>

namespace Ast{

class AttributeSimple: public Attribute {
public:
    
    AttributeSimple(const Identifier * name, const Value * value):
    Attribute(Simple),
    name(name),
    value(value){
    }
    
    AttributeSimple(const Identifier * name):
    Attribute(Simple),
    name(name),
    value(0){
    }

    virtual void walk(Walker & walker){
        walker.onAttributeSimple(*this);
    }

    bool operator==(const std::string & str) const {
        return *name == str;
    }

    std::string valueAsString() const {
        std::string str;
        *this >> str;
        return str;
    }

    template <typename X>
    const AttributeSimple & operator>>(X & v) const {
        if (value != 0){
            try{
                *value >> v;
            } catch (const Exception & ex){
                throw Exception("tried to read the wrong type for '" + this->toString() + "' : " + ex.getReason());
            }
        }
        return *this;
    }

    /*
    virtual bool referenced(const void * value) const {
        return Attribute::referenced(value) ||
               name->referenced(value) ||
               (this->value != 0 && this->value->referenced(value));
    }
    */
    
    virtual void mark(std::map<const void*, bool> & marks) const {
        marks[this] = true;
        if (value != 0){
            value->mark(marks);
        }
        name->mark(marks);
    }

    /* just the identifier */
    std::string idString() const {
        return name->toString();
    }

    std::string toString() const {
        std::ostringstream out;
        if (value != 0){
            out << name->toString() << " = " << value->toString();
        } else {
            out << name->toString() << " = <none>";
        }

        return out.str();
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

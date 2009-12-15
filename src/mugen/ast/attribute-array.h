#ifndef _paintown_ast_attribute_array_h_
#define _paintown_ast_attribute_array_h_

#include "attribute.h"
#include "keyword.h"
#include "Value.h"
#include "walker.h"
#include <sstream>

namespace Ast{

class AttributeArray: public Attribute {
public:
    
    AttributeArray(const Keyword * name, const Value * index, const Value * value):
    Attribute(Array),
    name(name),
    index(index),
    value(value){
    }
   
    virtual void walk(Walker & walker){
        walker.onAttributeArray(*this);
    }

    bool operator==(const std::string & str) const {
        return *name == str;
    }

    std::string valueAsString() const {
        std::string str;
        *this >> str;
        return str;
    }

    int getIndex(){
        int i;
        *index >> i;
        return i;
    }

    template <typename X>
    const AttributeArray & operator>>(X & v) const {
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
        index->mark(marks);
        value->mark(marks);
        name->mark(marks);
    }

    /* just the identifier */
    std::string idString() const {
        return name->toString();
    }

    std::string toString() const {
        std::ostringstream out;
        out << name->toString() << "[" << index->toString() << "] = " << value->toString();

        return out.str();
    }

    virtual ~AttributeArray(){
        delete name;
        delete index;
        delete value;
    }

protected:
    const Keyword * name;
    const Value * index;
    const Value * value;
};

}

#endif

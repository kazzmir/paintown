#ifndef _paintown_ast_attribute_array_h_
#define _paintown_ast_attribute_array_h_

#include "attribute.h"
#include "keyword.h"
#include "identifier.h"
#include "exception.h"
#include "Value.h"
#include "walker.h"
#include <sstream>

namespace Ast{

class AttributeArray: public Attribute {
public:
    
    AttributeArray(const Keyword * name, const Value * index, const Value * value):
    Attribute(Array),
    keyword_name(name),
    identifier_name(0),
    index(index),
    value(value){
    }

    AttributeArray(const Identifier * name, const Value * index, const Value * value):
    Attribute(Array),
    keyword_name(0),
    identifier_name(name),
    index(index),
    value(value){
    }

    virtual Element * copy() const {
        if (keyword_name != 0){
            return new AttributeArray((Keyword*) keyword_name->copy(), (Value*) index->copy(), (Value*) value->copy());
        } else if (identifier_name != 0){
            return new AttributeArray((Identifier*) identifier_name->copy(), (Value*) index->copy(), (Value*) value->copy());
        }
        throw Exception("don't copy that floppy!");
    }
   
    virtual void walk(Walker & walker){
        walker.onAttributeArray(*this);
    }

    using Attribute::operator==;
    virtual bool operator==(const std::string & str) const {
        if (keyword_name != 0){
            return *keyword_name == str;
        } else if (identifier_name != 0){
            return *identifier_name == str;
        }
        return false;
    }

    std::string valueAsString() const {
        std::string str;
        *this >> str;
        return str;
    }

    int getIndex() const {
        int i;
        *index >> i;
        return i;
    }

    const Value * getValue() const {
        return value;
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
        if (keyword_name != 0){
            keyword_name->mark(marks);
        }
        if (identifier_name != 0){
            identifier_name->mark(marks);
        }
    }

    /* just the identifier */
    std::string idString() const {
        if (keyword_name != 0){
            return keyword_name->toString();
        } else if (identifier_name != 0){
            return identifier_name->toString();
        }

        return "";
    }

    virtual std::string toString() const {
        std::ostringstream out;
        if (keyword_name != 0){
            out << keyword_name->toString();
        } else if (identifier_name != 0){
            out << identifier_name->toString();
        }
        out << "[" << index->toString() << "] = " << value->toString();

        return out.str();
    }

    virtual ~AttributeArray(){
        delete keyword_name;
        delete identifier_name;
        delete index;
        delete value;
    }

protected:
    const Keyword * keyword_name;
    const Identifier * identifier_name;
    const Value * index;
    const Value * value;
};

}

#endif

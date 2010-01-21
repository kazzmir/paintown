#ifndef _paintown_ast_attribute_keyword_h_
#define _paintown_ast_attribute_keyword_h_

#include "attribute.h"
#include "keyword.h"
#include "Value.h"
#include "walker.h"
#include <sstream>

namespace Ast{

class AttributeKeyword: public Attribute {
public:
    
    AttributeKeyword(const Keyword * name, const Value * value):
    Attribute(KeywordAttribute),
    name(name),
    value(value){
    }
    
    AttributeKeyword(const Keyword * name):
    Attribute(KeywordAttribute),
    name(name),
    value(0){
    }

    virtual void walk(Walker & walker){
        walker.onAttributeKeyword(*this);
    }

    using Attribute::operator==;
    virtual bool operator==(const std::string & str) const {
        return *name == str;
    }

    std::string valueAsString() const {
        std::string str;
        *this >> str;
        return str;
    }

    template <typename X>
    const AttributeKeyword & operator>>(X & v) const {
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

    virtual std::string toString() const {
        std::ostringstream out;
        if (value != 0){
            out << name->toString() << " = " << value->toString();
        } else {
            out << name->toString() << " = <none>";
        }

        return out.str();
    }

    virtual ~AttributeKeyword(){
        delete name;
        delete value;
    }

protected:
    const Keyword * name;
    const Value * value;
};

}

#endif

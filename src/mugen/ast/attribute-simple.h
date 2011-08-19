#ifndef _paintown_ast_attribute_simple_h_
#define _paintown_ast_attribute_simple_h_

#include "attribute.h"
#include "identifier.h"
#include "Value.h"
#include "walker.h"
#include <sstream>
#include "util/token.h"

namespace Ast{

class AttributeSimple: public Attribute {
public:
    AttributeSimple(int line, int column, const Identifier * name, const Value * value):
    Attribute(line, column, Simple),
    name(name),
    value(value){
    }

    AttributeSimple(int line, int column, const Identifier * name):
    Attribute(line, column, Simple),
    name(name),
    value(NULL){
    }
    
    AttributeSimple(const Identifier * name, const Value * value):
    Attribute(-1, -1, Simple),
    name(name),
    value(value){
    }
    
    AttributeSimple(const Identifier * name):
    Attribute(-1, -1, Simple),
    name(name),
    value(0){
    }

    virtual void walk(Walker & walker){
        walker.onAttributeSimple(*this);
    }

    using Attribute::operator==;
    virtual bool operator==(const Attribute & him) const {
        return him == *this;
    }

    virtual bool operator==(const AttributeSimple & him) const {
        bool same_values = true;
        if (value != NULL && him.value != NULL){
            same_values = *value == *him.value;
        } else if (value != NULL){
            same_values = false;
        } else if (him.value != NULL){
            same_values = false;
        }

        return getLine() == him.getLine() &&
               getColumn() == him.getColumn() &&
               *name == *him.name &&
               same_values;
    }

    virtual bool operator==(const std::string & str) const {
        return *name == str;
    }

    virtual Element * copy() const {
        if (value != 0){
            return new AttributeSimple(getLine(), getColumn(), (Identifier*) name->copy(), (Value*) value->copy());
        } else {
            return new AttributeSimple((Identifier*) name->copy());
        }
    }

    Token * serialize() const {
        Token * token = new Token();
        *token << SERIAL_ATTRIBUTE_SIMPLE << getLine() << getColumn() << name->serialize();
        if (value != NULL){
            *token << value->serialize();
        }
        return token;
    }
    
    static AttributeSimple * deserialize(const Token * token);

    std::string valueAsString() const {
        std::string str;
        *this >> str;
        return str;
    }

    const Value * getValue() const {
        return value;
    }

    template <typename X>
    const AttributeSimple & operator>>(X & v) const {
        if (value != 0){
            try{
                *value >> v;
            } catch (const Exception & ex){
                std::ostringstream out;
                out << "tried to read the wrong type for '" << this->toString() << "' : " << ex.getReason();
                if (getLine() != -1 && getColumn() != -1){
                    out << " at line " << getLine() << " column " << getColumn();
                }
                throw Exception(out.str());
            }
        } else {
            std::ostringstream out;
            out << "no values available for attribute '" << this->toString() << "'";
            if (getLine() != -1 && getColumn() != -1){
                out << " at line " << getLine() << " column " << getColumn();
            }
            throw Exception(out.str());
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
    virtual std::string idString() const {
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

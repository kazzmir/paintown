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
    Attribute(Simple),
    name(name),
    value(value),
    line(line),
    column(column){
    }
    
    AttributeSimple(const Identifier * name, const Value * value):
    Attribute(Simple),
    name(name),
    value(value),
    line(-1),
    column(-1){
    }
    
    AttributeSimple(const Identifier * name):
    Attribute(Simple),
    name(name),
    value(0),
    line(-1),
    column(-1){
    }

    virtual void walk(Walker & walker){
        walker.onAttributeSimple(*this);
    }

    using Attribute::operator==;
    virtual bool operator==(const std::string & str) const {
        return *name == str;
    }

    virtual Element * copy() const {
        if (value != 0){
            return new AttributeSimple(line, column, (Identifier*) name->copy(), (Value*) value->copy());
        } else {
            return new AttributeSimple((Identifier*) name->copy());
        }
    }

    virtual int getLine() const {
        return line;
    }

    virtual int getColumn() const {
        return column;
    }

    Token * serialize() const {
        Token * token = new Token();
        *token << "attribute-simple" << line << column << name->serialize();
        if (value != NULL){
            *token << value->serialize();
        }
        return token;
    }

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
                if (line != -1 && column != -1){
                    out << " at line " << line << " column " << column;
                }
                throw Exception(out.str());
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
    int line;
    int column;
};

}

#endif

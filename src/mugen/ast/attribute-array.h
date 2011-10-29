#ifndef _paintown_ast_attribute_array_h_
#define _paintown_ast_attribute_array_h_

#include "attribute.h"
#include "keyword.h"
#include "identifier.h"
#include "exception.h"
#include "Value.h"
#include "walker.h"
#include <sstream>
#include "util/token.h"

namespace Ast{

class AttributeArray: public Attribute {
public:
    
    AttributeArray(int line, int column, const Keyword * name, const Value * index, const Value * value):
    Attribute(line, column, Array),
    keyword_name(name),
    identifier_name(0),
    index(index),
    value(value){
    }

    AttributeArray(int line, int column, const Identifier * name, const Value * index, const Value * value):
    Attribute(line, column, Array),
    keyword_name(0),
    identifier_name(name),
    index(index),
    value(value){
    }

    virtual Element * copy() const {
        if (keyword_name != 0){
            return new AttributeArray(getLine(), getColumn(), (Keyword*) keyword_name->copy(), (Value*) index->copy(), (Value*) value->copy());
        } else if (identifier_name != 0){
            return new AttributeArray(getLine(), getColumn(), (Identifier*) identifier_name->copy(), (Value*) index->copy(), (Value*) value->copy());
        }
        throw Exception("don't copy that floppy!");
    }
   
    virtual void walk(Walker & walker){
        walker.onAttributeArray(*this);
    }

    using Element::operator==;
    virtual bool operator==(const Attribute & him) const {
        return him == *this;
    }

    virtual bool operator==(const AttributeArray & him) const {
        if (keyword_name != NULL && him.keyword_name != NULL){
            if (*keyword_name != *him.keyword_name){
                return false;
            }
        }

        if ((keyword_name == NULL && him.keyword_name != NULL) ||
            (keyword_name != NULL && him.keyword_name == NULL)){
            return false;
        }

        if (identifier_name != NULL && him.identifier_name != NULL){
            if (*identifier_name != *him.identifier_name){
                return false;
            }
        }

        if ((identifier_name == NULL && him.identifier_name != NULL) ||
            (identifier_name != NULL && him.identifier_name == NULL)){
            return false;
        }

        return *index == *him.index &&
               *value == *him.value;
    }

    virtual bool operator==(const std::string & str) const {
        if (keyword_name != 0){
            return *keyword_name == str;
        } else if (identifier_name != 0){
            return *identifier_name == str;
        }
        return false;
    }

    static AttributeArray * deserialize(const Token * token);

    Token * serialize() const {
        Token * token = new Token();
        *token << SERIAL_ATTRIBUTE_ARRAY << getLine() << getColumn();
        if (keyword_name != NULL){
            *token << keyword_name->serialize();
        }
        if (identifier_name != NULL){
            *token << identifier_name->serialize();
        }
        *token << index->serialize();
        *token << value->serialize();
        return token;
    }

    std::string valueAsString() const {
        std::string str;
        view() >> str;
        return str;
    }

    int getIndex() const {
        int i = 0;
        index->view() >> i;
        return i;
    }

    const Value * getValue() const {
        return value;
    }

    virtual View view() const {
        if (value != NULL){
            return value->view();
        } else {
            std::ostringstream out;
            out << "no values available for attribute '" << this->toString() << "'";
            if (getLine() != -1 && getColumn() != -1){
                out << " at line " << getLine() << " column " << getColumn();
            }
            throw Exception(out.str());
        }
    }

    /*
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
    */

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

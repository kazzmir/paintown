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
    
    AttributeKeyword(int line, int column, const Keyword * name, const Value * value):
    Attribute(line, column, KeywordAttribute),
    name(name),
    value(value){
    }
    
    AttributeKeyword(int line, int column, const Keyword * name):
    Attribute(line, column, KeywordAttribute),
    name(name),
    value(0){
    }

    virtual void walk(Walker & walker){
        walker.onAttributeKeyword(*this);
    }
    
    virtual Element * copy() const {
        if (value == NULL){
            return new AttributeKeyword(getLine(), getColumn(), (Keyword*) name->copy());
        } else {
            return new AttributeKeyword(getLine(), getColumn(), (Keyword*) name->copy(), (Value*) value->copy());
        }
    }

    using Attribute::operator==;
    virtual bool operator==(const std::string & str) const {
        return *name == str;
    }

    virtual bool operator==(const Attribute & him) const {
        return him == *this;
    }

    virtual bool operator==(const AttributeKeyword & him) const {
        if (value != NULL && him.value != NULL){
            if (*value != *him.value){
                return false;
            }
        }
        if ((value != NULL && him.value == NULL) ||
            (value == NULL && him.value != NULL)){
            return false;
        }
        return *name == *him.name;
    }

    std::string valueAsString() const {
        std::string str;
        *this >> str;
        return str;
    }

    Token * serialize() const {
        Token * token = new Token();
        *token << SERIAL_ATTRIBUTE_KEYWORD << getLine() << getColumn() << name->serialize();
        if (value != NULL){
            *token << value->serialize();
        }
        return token;
    }

    static AttributeKeyword * deserialize(const Token * token){
        const Token * keyword;
        int line, column;
        TokenView view = token->view();
        view >> line >> column >> keyword;
        try{
            const Token * value;
            view >> value;
            return new AttributeKeyword(line, column, Keyword::deserialize(keyword), Value::deserialize(value));
        } catch (const TokenException & e){
            return new AttributeKeyword(line, column, Keyword::deserialize(keyword));
        }
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

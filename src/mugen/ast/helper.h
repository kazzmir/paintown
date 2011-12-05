#ifndef _paintown_ast_helper_h_
#define _paintown_ast_helper_h_

#include <list>
#include <string>
#include <sstream>
#include <algorithm>
#include <map>

#include "Value.h"
#include "util/funcs.h"
#include "util/token.h"

namespace Ast{

class Helper: public Value {
public:
    Helper(int line, int column, const std::string & name, Value * expression, Value * original):
    Value(line, column),
    name(name),
    expression(expression),
    original(original){
    }

    static int lowerCase( int c ){
        return tolower(c);
    }
    
    virtual Element * copy() const {
        Value * expression = NULL;
        if (this->expression != NULL){
            expression = (Value*) this->expression->copy();
        }
        return new Helper(getLine(), getColumn(), name, expression, (Value*) original->copy());
    }

    static std::string downcase(std::string str){
        std::transform(str.begin(), str.end(), str.begin(), lowerCase);
        return str;
    }

    virtual Value * getOriginal() const {
        return original;
    }

    /* might be NULL */
    virtual Value * getArgument() const {
        return expression;
    }
    
    virtual void walk(Walker & walker) const {
        walker.onHelper(*this);
    }

    Token * serialize() const {
        Token * token = new Token();
        *token << SERIAL_HELPER << getLine() << getColumn() << name << original->serialize();
        *token << (expression != NULL);
        if (expression != NULL){
            *token << expression->serialize();
        }
        return token;
    }

    static Helper * deserialize(const Token * token){
        std::string name;
        const Token * next;
        const Token * expression = NULL;
        int line, column;
        bool hasExpression = false;
        TokenView view = token->view();
        view >> line >> column >> name >> next >> hasExpression;
        if (hasExpression){
            view >> expression;
            return new Helper(line, column, name, Value::deserialize(expression), Value::deserialize(next));
        }
        return new Helper(line, column, name, NULL, Value::deserialize(next));
    }

    using Element::operator==;
    bool operator==(const Value & him) const {
        return him == *this;
    }

    bool operator==(const Helper & him) const {
        return name == him.name &&
               *original == *him.original &&
               ((expression == NULL && him.expression == NULL) ||
                ((expression != NULL && him.expression != NULL) &&
                 (*expression == *him.expression)));
    }

    bool operator==(const std::string & str) const {
        return downcase(name) == downcase(str);
    }
    
    virtual std::string getType() const {
        return "helper";
    }

    virtual std::string toString() const {
        std::ostringstream out;
        out << name;
        if (expression != NULL){
            out << "(" << expression->toString() << ")";
        }
        out << ", " << original->toString();
        return Util::trim(out.str());
    }

    class HelperView: public ViewImplementation {
    public:
        HelperView(const Helper * owner):
        owner(owner){
        }

        const Helper * owner;

        virtual std::string getType() const {
            return owner->getType();
        }
        
        virtual const Value * self() const {
            return owner;
        }

        virtual std::string toString() const {
            return owner->toString();
        }
    };

    virtual View view() const {
        return View(Util::ReferenceCount<ViewImplementation>(new HelperView(this)));
    }
    
    /*
    virtual bool referenced(const void * value) const {
        if (value == this){
            return true;
        }
        
        for (std::list<std::string*>::const_iterator it = names.begin(); it != names.end(); it++){
            if (value == *it){
                return true;
            }
        }

        return false;
    }
    */
    
    virtual void mark(std::map<const void*, bool> & marks) const {
        marks[this] = true;
        original->mark(marks);
        if (expression != NULL){
            expression->mark(marks);
        }
    }

    virtual ~Helper(){
        delete original;
        delete expression;
    }

protected:
    std::string name;
    Value * expression;
    Value * original;
};

}

#endif

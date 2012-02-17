#ifndef _paintown_mugen_ast_argument_h
#define _paintown_mugen_ast_argument_h

#include "Value.h"
#include <string>
#include <sstream>

/* Argument will extract the argN field from the Environment at runtime and evaluate it
 * where N is the 'argument' field.
 */

namespace Ast{

class Argument: public Value {
public:
    Argument(int line, int column, int argument):
    Value(line, column),
    argument(argument){
    }

    class ArgumentView: public ViewImplementation {
    public:
        ArgumentView(const Argument * owner):
        owner(owner){
        }

        const Argument * owner;

        virtual std::string getType() const {
            return owner->getType();
        }
    
        virtual const Value * self() const {
            return owner;
        }
        
        virtual std::string toString() const {
            return owner->toString();
        }

        using ViewImplementation::operator>>;
        virtual ArgumentView & operator>>(const Value *& v){
            v = owner;
            return *this;
        }

        virtual ArgumentView & operator>>(int & v){
            v = owner->argument;
            return *this;
        }
    };

    using Value::view;
    virtual View view() const {
        return View(Util::ReferenceCount<ViewImplementation>(new ArgumentView(this)));
    }

    virtual void walk(Walker & walker) const {
        walker.onArgument(*this);
    }
    
    virtual Element * copy() const {
        return new Argument(getLine(), getColumn(), argument);
    }

    using Element::operator==;
    virtual bool operator==(const Value & him) const {
        return him == *this;
    }

    virtual bool operator==(const Argument & him) const {
        return argument == him.argument;
    }
   
    static Argument * deserialize(const Token * token){
        int line, column;
        int argument;
        token->view() >> line >> column >> argument;
        return new Argument(line, column, argument);
    }

    Token * serialize() const {
        Token * token = new Token();
        *token << SERIAL_ARGUMENT << getLine() << getColumn() << argument;
        return token;
    }
        
    virtual std::string getType() const {
        return "argument";
    }

    virtual std::string toString() const {
        std::ostringstream out;
        out << "x" << argument;
        return out.str();
    }

    virtual ~Argument(){
    }
protected:
    int argument;
};

}

#endif

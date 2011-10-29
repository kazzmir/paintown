#ifndef _paintown_ast_number_h_
#define _paintown_ast_number_h_

#include "Value.h"
#include <string>
#include <sstream>
#include <math.h>

namespace Ast{

class Number: public Value {
public:
    Number(int line, int column, double value):
    Value(line, column),
    value(value){
        /* arbitrary limit */
        /*
        if (value > 999999){
            value = 999999;
        }
        */
    }

    class NumberView: public ViewImplementation {
    public:
        NumberView(const Number * owner):
        owner(owner){
        }

        const Number * owner;

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
        virtual NumberView & operator>>(const Value *& v){
            v = owner;
            return *this;
        }

        virtual NumberView & operator>>(int & x){
            x = (int) owner->value;
            return *this;
        }

        virtual NumberView & operator>>(double & x){
            x = owner->value;
            return *this;
        }


        virtual NumberView & operator>>(bool & item){
            /* cast to int here? probably not because then 0.1 would become false
             * can an IEEE floating point value be exactly 0 in binary?
             */
            if (owner->value == 0){
                item = false;
            } else {
                item = true;
            }
            return *this;
        }
    };

    using Value::view;
    virtual View view() const {
        return View(Util::ReferenceCount<ViewImplementation>(new NumberView(this)));
    }

    virtual void walk(Walker & walker) const {
        walker.onNumber(*this);
    }
    
    virtual Element * copy() const {
        return new Number(getLine(), getColumn(), value);
    }

    using Element::operator==;
    virtual bool operator==(const Value & him) const {
        return him == *this;
    }

    virtual bool operator==(const Number & him) const {
        /* doubles are printed with a precision of 6 in the tokens */
        double epsilon = 0.00001;
        return fabs(value - him.value) < epsilon;
    }
   
    static Number * deserialize(const Token * token){
        double value;
        int line, column;
        token->view() >> line >> column >> value;
        return new Number(line, column, value);
    }

    Token * serialize() const {
        Token * token = new Token();
        *token << SERIAL_NUMBER << getLine() << getColumn() << value;
        return token;
    }
    
        
    virtual std::string getType() const {
        return "number";
    }

    virtual std::string toString() const {
        std::ostringstream out;
        // out << '[' << value << ']';
        out << value;
        return out.str();
    }

    virtual ~Number(){
    }
protected:
    double value;
};

}

#endif

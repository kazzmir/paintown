#ifndef _ast_EXPRESSION_H_
#define _ast_EXPRESSION_H_

#include "Value.h"

namespace Ast{

class Expression: public Value {
public:
    Expression():
        Value(){
    }

    virtual std::string getType() const {
        return "expression";
    }
};

class ExpressionInfix: public Expression {
public:
    enum InfixType{
        Or,
        And,
    };

    ExpressionInfix(InfixType type, const Value * left, const Value * right):
    Expression(),
    type(type),
    left(left),
    right(right){
    }

    virtual void mark(std::map<const void*, bool> & marks) const {
        marks[this] = true;
        left->mark(marks);
        right->mark(marks);
    }

    virtual std::string toString() const {
        std::stringstream out;
        out << "(" << left->toString() << " " << infixName(type) << " " << right->toString() << ")";
        return out.str();
    }

    virtual const char * infixName(InfixType type) const {
        switch (type){
            case Or : return "||";
            case And : return "&&";
        }
    }

    virtual std::string getType() const {
        return "infix expression";
    }

    virtual ~ExpressionInfix(){
        delete left;
        delete right;
    }

protected:
    InfixType type;
    const Value * left;
    const Value * right;
};

}

#endif

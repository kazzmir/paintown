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

    virtual ~Expression(){
    }
};

class ExpressionUnary: public Expression {
public:
    enum UnaryType{
        Not,
        Minus,
        Negation,
    };
    
    ExpressionUnary(UnaryType type, const Value * expression):
    type(type),
    expression(expression){
    }

    virtual std::string getType() const {
        return "unary expression";
    }

    virtual void mark(std::map<const void*, bool> & marks) const {
        marks[this] = true;
        expression->mark(marks);
    }

    virtual std::string toString() const {
        std::stringstream out;
        out << prefixName(type) << expression->toString();
        return out.str();
    }

    virtual const char * prefixName(UnaryType type) const {
        switch (type){
            case Not : return "!";
            case Minus : return "-";
            case Negation : return "~";
            default : return "?";
        }
    }

    virtual ~ExpressionUnary(){
        delete expression;
    }

protected:
    UnaryType type;
    const Value * expression;
};

class ExpressionInfix: public Expression {
public:
    enum InfixType{
        Or,
        XOr,
        And,
        BitwiseOr,
        BitwiseXOr,
        BitwiseAnd,
        Assignment,
        Equals,
        Unequals,
        GreaterThanEquals,
        GreaterThan,
        LessThanEquals,
        LessThan,
        Add,
        Subtract,
        Multiply,
        Divide,
        Modulo,
        Power,
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
            case XOr : return "^^";
            case BitwiseOr : return "|";
            case BitwiseXOr : return "^";
            case BitwiseAnd : return "&";
            case Assignment : return "!=";
            case Equals : return "=";
            case Unequals : return "!=";
            case GreaterThanEquals : return ">=";
            case GreaterThan : return ">";
            case LessThanEquals : return "<=";
            case LessThan : return "<";
            case Add : return "+";
            case Subtract : return "-";
            case Multiply : return "*";
            case Divide : return "/";
            case Modulo : return "%";
            case Power : return "**";
            default : return "?";
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

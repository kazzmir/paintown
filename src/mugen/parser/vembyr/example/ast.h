#ifndef _math_ast_h
#define _math_ast_h

/* Forward declaration of Visitor lets us declare all the Ast nodes before declaring
 * the visitor interface.
 */
class Visitor;

/* AST Interface / Base class */
class AstNode{
public:
    AstNode();
    virtual ~AstNode();

    /* Nodes allow for the visitor pattern to be used */
    virtual void visit(Visitor & visitor) const = 0;
};

class BinaryOperator: public AstNode {
public:
    enum Type{
        Addition,
        Subtraction,
        Multiply,
        Division
    };

    BinaryOperator(Type type, AstNode * left, AstNode * right);
    virtual ~BinaryOperator();
    virtual void visit(Visitor & visitor) const ;

    const AstNode * getLeft() const;
    const AstNode * getRight() const;

    const Type getType() const;

protected:
    /* what mathematical function this operator does */
    Type type;

    /* left-hand expression */
    AstNode * left;

    /* right-hand expression */
    AstNode * right;
};

class Number: public AstNode {
public:
    Number(int value);
    virtual ~Number();
    virtual void visit(Visitor & visitor) const;
    int getNumber() const;
protected:
    int value;
};

/* An interface/base class for the visitor pattern */
class Visitor{
public:
    Visitor();
    virtual ~Visitor();

    /* All visit methods default to do nothing so any base class must override them */
    virtual void visitBinaryOperator(const BinaryOperator & binary);
    virtual void visitNumber(const Number & number);
};

#endif

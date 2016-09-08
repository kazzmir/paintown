#include "ast.h"

/* AST Interface / Base class */
AstNode::AstNode(){
}

AstNode::~AstNode(){
}

BinaryOperator::BinaryOperator(Type type, AstNode * left, AstNode * right):
type(type),
left(left),
right(right){
}

BinaryOperator::~BinaryOperator(){
}

void BinaryOperator::visit(Visitor & visitor) const {
    visitor.visitBinaryOperator(*this);
}
    
const BinaryOperator::Type BinaryOperator::getType() const {
    return type;
}
    
const AstNode * BinaryOperator::getLeft() const {
    return left;
}

const AstNode * BinaryOperator::getRight() const {
    return right;
}

Number::Number(int value):
value(value){
}

Number::~Number(){
}

void Number::visit(Visitor & visitor) const {
    visitor.visitNumber(*this);
}
   
int Number::getNumber() const {
    return value;
}

Visitor::Visitor(){
}

Visitor::~Visitor(){
}

void Visitor::visitBinaryOperator(const BinaryOperator & binary){
}
    
void Visitor::visitNumber(const Number & number){
}

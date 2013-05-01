#include "ast.h"
#include "math.h"
#include <string>
#include <iostream>
#include <sstream>

class PrintAst: public Visitor {
public:
    std::string result;

    void visitBinaryOperator(const BinaryOperator & binary){
        std::ostringstream out;
        out << "(";
        switch (binary.getType()){
            case BinaryOperator::Addition: {
                out << "+";
                break;
            }
            case BinaryOperator::Subtraction: {
                out << "-";
                break;
            }
            case BinaryOperator::Multiply: {
                out << "*";
                break;
            }
            case BinaryOperator::Division: {
                out << "/";
                break;
            }
        }
        
        binary.getLeft()->visit(*this);
        out << " " << result;
        binary.getRight()->visit(*this);
        out << " " << result << ")";
        result = out.str();
    }

    virtual void visitNumber(const Number & number){
        std::ostringstream out;
        out << number.getNumber();
        result = out.str();
    }
};

/* A simple interpreter for the math ast */
class Calculator: public Visitor {
public:
    Calculator():
    result(0){
    }

    double result;

    void visitBinaryOperator(const BinaryOperator & binary){
        double left = 0;
        double right = 0;
        binary.getLeft()->visit(*this);
        left = result;
        binary.getRight()->visit(*this);
        right = result;

        switch (binary.getType()){
            case BinaryOperator::Addition: {
                result = left + right;
                break;
            }
            case BinaryOperator::Subtraction: {
                result = left - right;
                break;
            }
            case BinaryOperator::Multiply: {
                result = left * right;
                break;
            }
            case BinaryOperator::Division: {
                result = left / right;
                break;
            }
        }
    }
    
    virtual void visitNumber(const Number & number){
        result = number.getNumber();
    }
};

int main(){
    try{
        std::string input = "1 * 2 + 3 - 12 * 9 / 4";
        AstNode * top = (AstNode*) Parser::parse(input.c_str());
        std::cout << "Ast node " << top << std::endl;
        PrintAst printer;
        top->visit(printer);
        std::cout << "Input: " << input << std::endl;
        std::cout << "Ast tree: " << printer.result << std::endl;

        Calculator calculator;
        top->visit(calculator);
        std::cout << "Result: " << calculator.result << std::endl;
    } catch (const Parser::ParseException & fail){
        std::cout << "Parse failure: " << fail.getReason() << std::endl;
    }
}

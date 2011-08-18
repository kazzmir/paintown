#ifndef mugen_converter_triggers
#define mugen_converter_triggers

#include <string>
#include <vector>

#include "tools.h"

namespace Ast{
    class Value;
}

namespace Mugen{
   
class Content;
class PythonDefinition;

class Expression{
    public:
        Expression();
        Expression(const Expression &);
        Expression(const std::string &);
        Expression(const std::string &, const std::vector<Expression> &);
        virtual ~Expression();
        
        virtual const Expression & operator=(const Expression &);
        virtual void setKeyword(const std::string &, bool constant = false);
        virtual void addArguments(const Expression &);
        virtual const std::string get();
        inline virtual bool isConstant() const {
            return this->constant;
        }
    protected:
        std::string keyword;
        std::vector <Expression> arguments;
        bool constant;
};

/* Expression Builder houses Expressions and pieces them together for output in python 
 */
class ExpressionBuilder{
    public:
        ExpressionBuilder();
        ExpressionBuilder(const ExpressionBuilder &);
        virtual ~ExpressionBuilder();
        
        virtual const ExpressionBuilder & operator=(const ExpressionBuilder &);
        
        virtual void setLeft(const Expression &);
        virtual void setRight(const Expression &);
        virtual void setOperator(const std::string &);
        
        enum Type{
            Unary,
            Infix,
        };
        
        virtual const std::string get();
        
        virtual const Content & getFunction();
        
        inline virtual void setType(const Type & type){
            this->type = type;
        }
        
    protected:
        Expression left;
        Expression right;
        std::string expressionOperator;
        Type type;
        std::string expression;
        Content content;
};

/*! Trigger Handling converts triggers into python code */
namespace TriggerHandler{

/* Convert trigger identifier */
ExpressionBuilder convert(const Ast::Value &);

}
}
#endif
#ifndef mugen_converter_triggers
#define mugen_converter_triggers

#include <string>
#include <vector>

namespace Ast{
    class Value;
}

namespace Mugen{
   
class Content;
    
class Expression{
    public:
        Expression();
        Expression(const std::string &, bool constant = false);
        Expression(const std::string &, const std::vector<std::string> &);
        Expression(const Expression &);
        virtual ~Expression();
        
        const Expression & operator=(const Expression &);
        
        virtual const std::string get();
        
        virtual void addArguments(const std::string &);
        
        inline virtual void setKeyword(const std::string & keyword, bool constant = false){
            this->keyword = keyword;
            this->constant = constant;
        }
        
        
    protected:
        std::string keyword;
        std::vector <std::string> arguments;
        bool constant;
};
    
class ExpressionBuilder{
    public:
        ExpressionBuilder();
        ExpressionBuilder(const ExpressionBuilder &);
        virtual ~ExpressionBuilder();
        
        virtual const ExpressionBuilder & operator=(const ExpressionBuilder &);
        
        virtual void setLeft(const Expression &);
        virtual void addRight(const Expression &);
        virtual void setOperator(const std::string &);
        
        enum Type{
            Unary,
            Infix,
        };
        
        virtual const std::string get();
        
        inline virtual void setType(const Type & type){
            this->type = type;
        }
        
        inline virtual bool hasRange(){
            if (this->right.size() > 1){
                return true;
            } else {
                return false;
            }
        }
        
        inline virtual std::vector<Expression> getRight(){
            return this->right;
        }
        
    protected:
        Expression left;
        // To accomodate ranges
        std::vector<Expression> right;
        std::string expressionOperator;
        Type type;
};

/*! Trigger Handling converts triggers into python code */
namespace TriggerHandler{

/* Convert trigger identifier */
ExpressionBuilder convert(const Ast::Value &);

}
}
#endif
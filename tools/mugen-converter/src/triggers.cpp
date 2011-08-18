#include "triggers.h"

#include <iostream>
#include <sstream>
#include <deque>
#include <map>

#include "ast/all.h"

using namespace Mugen;
using namespace TriggerHandler;

Expression::Expression():
constant(false){
}

Expression::Expression(const std::string & keyword):
keyword(keyword),
constant(true){
}

Expression::Expression(const std::string & keyword, const std::vector<Expression> & arguments):
keyword(keyword),
arguments(arguments),
constant(false){
}

Expression::Expression(const Expression & copy){
    keyword = copy.keyword;
    arguments = copy.arguments;
    constant = copy.constant;
}

Expression::~Expression(){
}

const Expression & Expression::operator=(const Expression & copy){
    keyword = copy.keyword;
    arguments = copy.arguments;
    constant = copy.constant;
    
    return *this;
}

const std::string Expression::get(){

    if (keyword.empty()){
        return "";
    }
    if (constant){
        return keyword;
    }
    std::string expression = keyword + "(";
    if (!arguments.empty()){
        std::string args;
        for (std::vector<Expression>::iterator i = arguments.begin(); i != arguments.end(); ++i){
            args += (*i).get() + ", ";
        }
        expression += args.substr(0,args.size()-2) + ")";
    } else {
        expression += ")";
    }
    
    return expression;
}

void Expression::setKeyword(const std::string & keyword, bool constant){
    this->keyword = keyword;
    this->constant = constant;
}

void Expression::addArguments(const Expression & argument){
    arguments.push_back(argument);
}

/* TODO FIXME and all that
 * Assuming that an object named player and an object named world is going to be passed to States for evaluation
 * And also assuming that the names of the functions are going to be what they are below (most likely most of it will change)
 */;
std::string handleKeyWord(const std::string & keyword){
    if (match("AILevel", keyword)){
    } else if (match("Abs", keyword)){
        //return new Function("math.fabs");
    } else if (match("Acos", keyword)){
        //return new Function("math.acos");
    } else if (match("Alive", keyword)){
        //return new Function("player.isAlive");
    } else if (match("Anim", keyword)){
        //return new Function("player.currentAnimation");
    } else if (match("AnimElem", keyword)){
    } else if (match("AnimElemNo", keyword)){
    } else if (match("AnimElemTime", keyword)){
    } else if (match("AnimExist", keyword)){
    } else if (match("AnimTime", keyword)){
    } else if (match("Asin", keyword)){
        //return new Function("math.asin");
    } else if (match("Atan", keyword)){
        //return new Function("math.atan");
    } else if (match("AuthorName", keyword)){
        //return new Function("player.getAuthor");
    } else if (match("BackEdgeBodyDist", keyword)){
    } else if (match("BackEdgeDist", keyword)){

    } else if (match("CanRecover", keyword)){
        //return new Function("player.canRecover");
    } else if (match("Ceil", keyword)){
        //return new Function("math.ceil");
    } else if (match("Command", keyword)){
        //return new Function("player.EvaluateCommand");
    } else if (match("Const", keyword)){
    } else if (match("Const240p", keyword)){
    } else if (match("Const480p", keyword)){
    } else if (match("Const720p", keyword)){
    } else if (match("Cos", keyword)){
        //return new Function("math.cos");
    } else if (match("Ctrl", keyword)){
        //return new Function("player.hasControl");
    } else if (match("DrawGame", keyword)){

    } else if (match("E", keyword)){
        //return new Keyword("math.e");
    } else if (match("Exp", keyword)){
        //return new Function("math.exp");
    } else if (match("FVar", keyword)){
    } else if (match("Facing", keyword)){
        //return new Function("player.getFacing");
    } else if (match("Floor", keyword)){
        //return new Function("math.floor");
    } else if (match("FrontEdgeBodyDist", keyword)){
    } else if (match("FrontEdgeDist", keyword)){

    } else if (match("GameTime", keyword)){
        //return new Function("world.getTime");
    } else if (match("GetHitVar", keyword)){

    } else if (match("HitCount", keyword)){
    } else if (match("HitDefAttr", keyword)){
    } else if (match("HitFall", keyword)){
    } else if (match("HitOver", keyword)){
    } else if (match("HitPauseTime", keyword)){
    } else if (match("HitShakeOver", keyword)){
    } else if (match("HitVel", keyword)){

    } else if (match("ID", keyword)){
    } else if (match("IfElse", keyword)){
        //return Expression("mugen.ifelse");
    } else if (match("InGuardDist", keyword)){
    } else if (match("IsHelper", keyword)){
    } else if (match("IsHomeTeam", keyword)){

    } else if (match("Life", keyword)){
    } else if (match("LifeMax", keyword)){
    } else if (match("Ln", keyword)){
        //return new Function("mugen.ln");
    } else if (match("Log", keyword)){
        //return new Function("math.log");
    } else if (match("Lose", keyword)){

    } else if (match("MatchNo", keyword)){
    } else if (match("MatchOver", keyword)){
    } else if (match("MoveContact", keyword)){
    } else if (match("MoveGuarded", keyword)){
    } else if (match("MoveHit", keyword)){
    } else if (match("MoveReversed", keyword)){
    } else if (match("MoveType", keyword)){

    } else if (match("Name", keyword)){
    } else if (match("NumEnemy", keyword)){
    } else if (match("NumExplod", keyword)){
    } else if (match("NumHelper", keyword)){
    } else if (match("NumPartner", keyword)){
    } else if (match("NumProj", keyword)){
    } else if (match("NumProjID", keyword)){
    } else if (match("NumTarget", keyword)){

    } else if (match("P1Name", keyword)){
    } else if (match("P2BodyDist", keyword)){
    } else if (match("P2Dist", keyword)){
    } else if (match("P2Life", keyword)){
    } else if (match("P2MoveType", keyword)){
    } else if (match("P2Name", keyword)){
    } else if (match("P2StateNo", keyword)){
    } else if (match("P2StateType", keyword)){
    } else if (match("P3Name", keyword)){
    } else if (match("P4Name", keyword)){
    } else if (match("PalNo", keyword)){
    } else if (match("ParentDist", keyword)){
    } else if (match("Pi", keyword)){
        //return new Keyword("math.pi");
    } else if (match("PlayerIDExist", keyword)){
    } else if (match("Pos", keyword)){
    } else if (match("Power", keyword)){
    } else if (match("PowerMax", keyword)){
    } else if (match("PrevStateNo", keyword)){
    } else if (match("ProjCancelTime", keyword)){
    } else if (match("ProjContact", keyword)){
    } else if (match("ProjContactTime", keyword)){
    } else if (match("ProjGuarded", keyword)){
    } else if (match("ProjGuardedTime", keyword)){
    } else if (match("ProjHit", keyword)){
    } else if (match("ProjHitTime", keyword)){

    } else if (match("Random", keyword)){
        //return new Function("random");
    } else if (match("RootDist", keyword)){
    } else if (match("RoundNo", keyword)){
        //return new Function("world.currentRound");
    } else if (match("RoundState", keyword)){
        //return new Function("world.roundState");
    } else if (match("RoundsExisted", keyword)){
        //return new Function("player.roundsExisted");
    } else if (match("ScreenPos", keyword)){
    } else if (match("SelfAnimExist", keyword)){
    } else if (match("Sin", keyword)){
        //return new Function("math.sin");
    } else if (match("StateNo", keyword)){
    } else if (match("StateType", keyword)){
    } else if (match("SysFVar", keyword)){
    } else if (match("SysVar", keyword)){

    } else if (match("Tan", keyword)){
        //return new Function("math.tan");
    } else if (match("TeamMode", keyword)){
    } else if (match("TeamSide", keyword)){
    } else if (match("TicksPerSecond", keyword)){
    } else if (match("Time", keyword)){
    } else if (match("TimeMod", keyword)){

    } else if (match("UniqHitCount", keyword)){

    } else if (match("Var", keyword)){
    } else if (match("Vel", keyword)){

    } else if (match("Win", keyword)){
    } else {
        std::cout << "Unhandled keyword: " << keyword << std::endl;
    }
    
    return "";
}

ExpressionBuilder::ExpressionBuilder():
type(Infix){
}
ExpressionBuilder::ExpressionBuilder(const ExpressionBuilder & copy):
type(Infix){
    left = copy.left;
    right = copy.right;
    expressionOperator = copy.expressionOperator;
    type = copy.type;
}
ExpressionBuilder::~ExpressionBuilder(){
}
const ExpressionBuilder & ExpressionBuilder::operator=(const ExpressionBuilder & copy){
    left = copy.left;
    right = copy.right;
    expressionOperator = copy.expressionOperator;
    type = copy.type;
    return *this;
}
void ExpressionBuilder::setLeft(const Expression & expression){
    left = expression;
}
void ExpressionBuilder::setRight(const Expression & expression){
    right = expression;
}
void ExpressionBuilder::setOperator(const std::string & op){
    expressionOperator = op;
}

const std::string ExpressionBuilder::get(){
    switch (type){
        case Infix:{
            return "(" + left.get() + expressionOperator + right.get() + ")";
            
        }
        case Unary:{
            return expressionOperator + "(" + left.get() + ")";
        }
        default:
            break;
    }
    return "";
}

const Content & ExpressionBuilder::getFunction(){
    return content;
}

class Evaluator{
    public:
        Evaluator(){
        }
        virtual ~Evaluator(){
        }
        
        /* This will be set to statedefnumber_count in TriggerHandler::convert         */
        void setName(const std::string & name){
            this->name = name;
        }
        
        const std::string & getName(){
            return name;
        }
        
        Content & get(){
            return content;
        }
        
    protected:
        std::string name;
        Content content;
};

ExpressionBuilder TriggerHandler::convert(const Ast::Value & value){
    class ExpressionWalker : public Ast::Walker{
    public:
        ExpressionWalker(ExpressionBuilder & exp):
        exp(exp),
        left(true){
        }
        ExpressionBuilder & exp;
        bool left;
        
        virtual void onValueList(const Ast::ValueList & values){
            for (unsigned int i = 0;;++i){
                Ast::Value * value = values.get(i);
                if (value){
                    convert(*value);
                    /*ExpressionWalker walker(exp);
                    value->walk(walker);*/
                } else {
                    break;
                }
            }
        }
        virtual void onRange(const Ast::Range & range){
            std::cout << "Found range: " << range.toString() << std::endl;
            /*double low = atof(range.getLow()->toString().c_str());
            double high = atof(range.getHigh()->toString().c_str());
            for (; low != high+1; ++low){
                std::ostringstream num;
                num << low;
                exp.addRight(new Keyword(num.str()));
            }*/
        }
        virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
            std::cout << "Found simple: " << simple.valueAsString() << std::endl;
        }
        virtual void onAttributeKeyword(const Ast::AttributeKeyword & simple){
            std::cout << "Found keyword: " << simple.valueAsString() << std::endl;
        }   
        virtual void onAttributeArray (const Ast::AttributeArray &simple){
            std::cout << "Found array: " << simple.toString() << std::endl;
        }
        virtual void onExpressionInfix(const Ast::ExpressionInfix & expression){
            std::cout << "Found Infix: " <<  expression.toString() << std::endl;
            ExpressionBuilder expLeft = convert(*expression.getLeft());
            exp.setLeft(Expression(expLeft.get()));
            left = false;
            ExpressionBuilder expRight = convert(*expression.getRight());
            exp.setRight(Expression(expRight.get()));
            
            exp.setOperator(expression.infixName(expression.getExpressionType()));
        }
        virtual void onExpressionUnary(const Ast::ExpressionUnary & expression){
            std::cout << "Found Unary: " <<  expression.toString() << std::endl;
            exp.setLeft(Expression(convert(*expression.getExpression()).get()));
            exp.setOperator(expression.prefixName(expression.getExpressionType()));
        }
        virtual void onIdentifier(const Ast::Identifier & identifier){
            // Triggers or Constants
            std::cout << "Found Identifier: " << identifier.toString() << std::endl;
            if (left){
                exp.setLeft(Expression(identifier.toString()));
            } else {
                exp.setRight(Expression(identifier.toString()));
            }
        }
        
        virtual void onHelper(const Ast::Helper & helper){
            std::cout << "Found Helper: " << helper.toString() << std::endl;
        }

        virtual void onString(const Ast::String & string){
            // Commands
            std::cout << "Found String: " << string.toString() << std::endl;
            if (left){
                exp.setLeft(Expression(string.toString()));
            } else {
                exp.setRight(Expression(string.toString()));
            }
        }
        
        virtual void onFunction(const Ast::Function & function){
            // Triggers w/ arguments
            std::cout << "Found Function: " << function.toString() << std::endl;
            Expression func = Expression(function.getName());//handleKeyWord(function.getName());
            for (unsigned int i = 0;;++i){
                const Ast::Value * value = function.getArg(i);
                if (value){
                    func.addArguments(Expression(convert(*value).get()));
                } else {
                    break;
                }
            }
            if (left){
                exp.setLeft(func);
            } else {
                exp.setRight(func);
            }
        }

        virtual void onKeyword(const Ast::Keyword & keyword){
            // Triggers or constants
            Expression word = Expression(keyword.toString());//handleKeyWord(keyword.toString());
            std::cout << "Found Keyword: " << keyword.toString() << "' translated as: " << word.get() << std::endl;
            if (left){
                exp.setLeft(word);
            } else {
                exp.setRight(word);
            }
        }
        
        virtual void onNumber(const Ast::Number & number){
            // Numerals
            std::cout << "Found Number: " << number.toString() << std::endl;
            if (left){
                exp.setLeft(Expression(number.toString()));
            } else {
                exp.setRight(Expression(number.toString()));
            }
        }
        
        virtual void finish(){
            //exp.push_back(builder);
        }
        
    };
    
    ExpressionBuilder expression;
    
    ExpressionWalker walker(expression);
    value.walk(walker);
    
    return expression;
}
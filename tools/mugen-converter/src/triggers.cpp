#include "triggers.h"

#include <iostream>
#include <sstream>
#include <deque>
#include <map>

#include "ast/all.h"

using namespace Mugen;
using namespace TriggerHandler;

/* Used to generate a useable function to handle infix's and unary's */
static int FUNCTION_NUMBER = 0;

static const std::string getNextFunctionName(){
    FUNCTION_NUMBER++;
    std::ostringstream stream;
    //stream << "def function" << FUNCTION_NUMBER << "(self, player, world):";
    stream << "evaluator" << FUNCTION_NUMBER;
    return stream.str();
}


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
    /* Adding values disqualifies constant */
    constant = false;
    arguments.push_back(argument);
}

/* TODO FIXME and all that
 * Assuming that an object named player and an object named world is going to be passed to States for evaluation
 * And also assuming that the names of the functions are going to be what they are below (most likely most of it will change)
 */;
static std::string handleKeyWord(const std::string & keyword){
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
type(NotSet),
leftComplex(NULL),
rightComplex(NULL){
}
ExpressionBuilder::ExpressionBuilder(const ExpressionBuilder & copy){
    expression = copy.expression;
    if (copy.leftComplex != NULL){
        leftComplex = new ExpressionBuilder(*copy.leftComplex);
    }
    if (copy.rightComplex != NULL){
        rightComplex = new ExpressionBuilder(*copy.rightComplex);
    }
    for (std::vector<ExpressionBuilder *>::const_iterator i = copy.valueList.begin(); i != copy.valueList.end(); ++i){
        ExpressionBuilder * builder = *i;
        if (builder != NULL){
            valueList.push_back(new ExpressionBuilder(*builder));
        }
    }
    expressionOperator = copy.expressionOperator;
    type = copy.type;
}
ExpressionBuilder::~ExpressionBuilder(){
    if (leftComplex != NULL){
        delete leftComplex;
    }
    if (rightComplex != NULL){
        delete rightComplex;
    }
    for (std::vector<ExpressionBuilder *>::iterator i = valueList.begin(); i != valueList.end(); ++i){
        if ((*i) != NULL){
            delete *i;
        }
    }
}
const ExpressionBuilder & ExpressionBuilder::operator=(const ExpressionBuilder & copy){
    expression = copy.expression;
    if (copy.leftComplex != NULL){
        leftComplex = new ExpressionBuilder(*copy.leftComplex);
    }
    if (copy.rightComplex != NULL){
        rightComplex = new ExpressionBuilder(*copy.rightComplex);
    }
    for (std::vector<ExpressionBuilder *>::const_iterator i = copy.valueList.begin(); i != copy.valueList.end(); ++i){
        ExpressionBuilder * builder = *i;
        if (builder != NULL){
            valueList.push_back(new ExpressionBuilder(*builder));
        }
    }
    expressionOperator = copy.expressionOperator;
    
    type = copy.type;
    return *this;
}
void ExpressionBuilder::setExpression(const Expression & expression){
    this->expression = expression;
}
void ExpressionBuilder::setLeft(ExpressionBuilder * builder){
    leftComplex = builder;;
}
void ExpressionBuilder::setRight(ExpressionBuilder * builder){
    rightComplex = builder;
}
void ExpressionBuilder::addToList(ExpressionBuilder * builder){
    valueList.push_back(builder);
}
void ExpressionBuilder::setOperator(const std::string & op){
    expressionOperator = op;
}

const std::string ExpressionBuilder::get(){
   switch (type){
        case ValueList:
            break;
        case Range:
            return expression.get();
            break;
        case Unary:
            return expressionOperator + "(" + crawlComplex(leftComplex) + ")";
            break;
        case Infix:{
            return crawlComplex(leftComplex) + expressionOperator + crawlComplex(rightComplex);
            break;
        }
        case Identifier:
            return expression.get();
            break;
        case Helper:
            return expression.get();
            break;
        case String:
            return expression.get();
            break;
        case Function:
            return expression.get();
            break;
        case Keyword:
            return expression.get();
            break;
        case Number:
            return expression.get();
            break;
        case NotSet:
        default:
            break;
    }
    
    
    return "";
}
Expression & ExpressionBuilder::getExpression(){
    return expression;
}
ExpressionBuilder * ExpressionBuilder::getLeftComplex(){
    return leftComplex;
}
ExpressionBuilder * ExpressionBuilder::getRightComplex(){
    return rightComplex;
}
std::vector<ExpressionBuilder *> & ExpressionBuilder::getValueList(){
    return valueList;
}
void ExpressionBuilder::setType(const Type & type){
    this->type = type;
}

const std::string ExpressionBuilder::crawlComplex(ExpressionBuilder * builder){
    if (builder != NULL){
        
        if (builder->leftComplex != NULL && builder->rightComplex != NULL){
            return crawlComplex(builder->leftComplex) + builder->expressionOperator + crawlComplex(builder->rightComplex);
        } else if (builder->leftComplex != NULL && builder->rightComplex == NULL){
            return crawlComplex(builder->leftComplex);
        } else if (builder->leftComplex == NULL && builder->rightComplex != NULL){
            return crawlComplex(builder->rightComplex);
        } else if (builder->leftComplex == NULL && builder->rightComplex == NULL){
            return builder->get();
        } 
    }
    return "";
}

class Evaluator{
    public:
        Evaluator(ExpressionBuilder & builder, std::vector<Content> & functions):
        builder(builder),
        functions(functions){
            name = getNextFunctionName() + "()";
            std::string main = handleBuilder(crawl(&builder));
        }
        
        virtual ~Evaluator(){
            for (std::vector<ExpressionBuilder *>::iterator i = deletable.begin(); i != deletable.end(); ++i){
                if (*i != NULL){
                    delete *i;
                }
            }
        }
        
        const std::string & getName(){
            return name;
        }
        
        const std::string convertInfixOperator(const std::string & op){
            if (op == "="){
                return "==";
            }
            
            return op;
        }

        const std::string convertUnaryOperator(const std::string & op){
            if (op == "!"){
                return "not";
            }
            
            return op;
        }
        const std::string createRange(const std::string & against, ExpressionBuilder * range){
            std::string function = getNextFunctionName();
            std::vector<Expression> args = range->getExpression().getArguments();
            Content checker(3, "def " + function + "(self):");
                checker.addLine(4, "return (" + against + " in range(" + args[0].get() + ", " + args[1].get() + "))");
            functions.push_back(checker);
            return function + "()";
        }
        
        ExpressionBuilder * crawl(ExpressionBuilder * builder){
            if (builder != NULL){
                if (builder->getLeftComplex() != NULL && builder->getRightComplex() != NULL){
                    if (builder->getLeftComplex()->getType() == ExpressionBuilder::Range){
                        /* Range is on the left side, swap it with the other side */
                        std::string rightExpression;
                        ExpressionBuilder * right = crawl(builder->getRightComplex());
                        if (right != NULL){
                            rightExpression = handleBuilder(crawl(right));
                        }
                        std::string range = createRange(rightExpression, builder->getLeftComplex());
                        ExpressionBuilder * newBuilder = new ExpressionBuilder();
                        deletable.push_back(newBuilder);
                        newBuilder->setType(ExpressionBuilder::String);
                        newBuilder->setExpression(Expression(range));
                        return newBuilder;
                    } else if (builder->getRightComplex()->getType() == ExpressionBuilder::Range){
                        /* Handle range  */
                        std::string leftExpression;
                        ExpressionBuilder * left = crawl(builder->getLeftComplex());
                        if (left != NULL){
                            leftExpression = handleBuilder(crawl(left));
                        }
                        std::string range = createRange(leftExpression, builder->getRightComplex());
                        ExpressionBuilder * newBuilder = new ExpressionBuilder();
                        deletable.push_back(newBuilder);
                        newBuilder->setType(ExpressionBuilder::String);
                        newBuilder->setExpression(Expression(range));
                        return newBuilder;
                    } else if (builder->getRightComplex()->getType() == ExpressionBuilder::ValueList){
                        /* TODO */
                    } else {
                        /* Handle infix */
                        std::string leftExpression, rightExpression;
                        ExpressionBuilder * left =  crawl(builder->getLeftComplex());
                        if (left != NULL){
                            leftExpression = handleBuilder(crawl(left));
                        }
                        const std::string & op = convertInfixOperator(builder->getOperator());
                        ExpressionBuilder * right = crawl(builder->getRightComplex());
                        if (right != NULL){
                            rightExpression = handleBuilder(crawl(right));
                        }
                        std::string function = getNextFunctionName();
                        Content infix(3, "def " + function + "(self):");
                            infix.addLine(4, "return (" + leftExpression + op + rightExpression + ")");
                        functions.push_back(infix);
                        ExpressionBuilder * newBuilder = new ExpressionBuilder();
                        deletable.push_back(newBuilder);
                        newBuilder->setType(ExpressionBuilder::String);
                        newBuilder->setExpression(Expression(function+"()"));
                        return newBuilder;
                    }
                } else if (builder->getLeftComplex() != NULL && builder->getRightComplex() == NULL){
                    /* This should be a unary operation */
                    ExpressionBuilder * left =  crawl(builder->getLeftComplex());
                    if (left != NULL){
                        const std::string & leftExpression = handleBuilder(crawl(left));
                        const std::string & op = convertUnaryOperator(builder->getOperator());
                        std::string function = getNextFunctionName();
                        Content unary(3, "def " + function + "(self):");
                            unary.addLine(4, "return (" + op + "(" + leftExpression + "))");
                        functions.push_back(unary);
                        ExpressionBuilder * newBuilder = new ExpressionBuilder();
                        deletable.push_back(newBuilder);
                        newBuilder->setType(ExpressionBuilder::String);
                        newBuilder->setExpression(Expression(function + "()"));
                        return newBuilder;
                    }
                } else if (builder->getLeftComplex() == NULL && builder->getRightComplex() != NULL){
                    /* NOTE This shouldn't happen but it is here just so that it can be monitored */
                    ExpressionBuilder * right = crawl(builder->getRightComplex());
                    if (right != NULL){
                        std::cout << "Found a right expression with no left one. Contents: " << handleBuilder(crawl(right));
                    }
                } else if (builder->getLeftComplex() == NULL && builder->getRightComplex() == NULL){
                    return builder;
                } 
            }
            return NULL;
        }
        
        /* Pushes back a function and returns the function name */
        const std::string handleBuilder(ExpressionBuilder * builder){
            if (builder != NULL){
                std::string function = getNextFunctionName();
                Content content(3, "def " + function + "(self):");
                    /* TODO FIXME */
                    content.addLine(4, "# Temporary - " + builder->get());
                    content.addLine(4, "pass");
                functions.push_back(content);
                
                return function + "()";
            }
            return "";
        }
        
        const ExpressionBuilder & builder;
        std::vector<Content> & functions;
        std::string name;
        
        std::vector<ExpressionBuilder *> deletable;
};

Trigger::Trigger(ExpressionBuilder & builder){
    Evaluator eval(builder, functions);
    name = eval.getName();
}

Trigger::Trigger(const Trigger & copy){
    functions = copy.functions;
    name = copy.name;
}

Trigger::~Trigger(){
}

const Trigger & Trigger::operator=(const Trigger & copy){
    functions = copy.functions;
    name = copy.name;
    
    return *this;
}

const std::vector<Content> & Trigger::getFunctions(){
    return functions;
}

const std::string & Trigger::getName(){
    return name;
}

void Trigger::addToDefinition(PythonDefinition & definition){
    for (std::vector<Content>::iterator i = functions.begin(); i != functions.end(); ++i){
        definition.addContent((*i));
    }
}

ExpressionBuilder TriggerHandler::convert(const Ast::Value & value){
    class ExpressionWalker : public Ast::Walker{
    public:
        ExpressionWalker(ExpressionBuilder & exp):
        exp(exp){
        }
        ExpressionBuilder & exp;
        
        virtual void onValueList(const Ast::ValueList & values){
            std::cout << "Found Value List: " << values.toString() << std::endl;
            exp.setType(ExpressionBuilder::ValueList);
            for (unsigned int i = 0;;++i){
                Ast::Value * value = values.get(i);
                if (value){
                    ExpressionBuilder * item = new ExpressionBuilder(convert(*value));
                    exp.addToList(item);
                } else {
                    break;
                }
            }
        }
        virtual void onRange(const Ast::Range & range){
            std::cout << "Found range: " << range.toString() << std::endl;
            exp.setType(ExpressionBuilder::Range);
            exp.getExpression().addArguments(Expression(range.getLow()->toString()));
            exp.getExpression().addArguments(Expression(range.getHigh()->toString()));
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
            
            exp.setType(ExpressionBuilder::Infix);
            ExpressionBuilder * expLeft = new ExpressionBuilder(convert(*expression.getLeft()));
            exp.setLeft(expLeft);
            
            ExpressionBuilder * expRight = new ExpressionBuilder(convert(*expression.getRight()));
            exp.setRight(expRight);
            
            exp.setOperator(expression.infixName(expression.getExpressionType()));
        }
        virtual void onExpressionUnary(const Ast::ExpressionUnary & expression){
            std::cout << "Found Unary: " <<  expression.toString() << std::endl;
            exp.setType(ExpressionBuilder::Unary);
            ExpressionBuilder * left = new ExpressionBuilder(convert(*expression.getExpression()));
            exp.setLeft(left);
            exp.setOperator(expression.prefixName(expression.getExpressionType()));
        }
        virtual void onIdentifier(const Ast::Identifier & identifier){
            // Triggers or Constants
            std::cout << "Found Identifier: " << identifier.toString() << std::endl;
            exp.setType(ExpressionBuilder::Identifier);
            exp.setExpression(Expression(identifier.toString()));
        }
        
        virtual void onHelper(const Ast::Helper & helper){
            std::cout << "Found Helper: " << helper.toString() << std::endl;
        }

        virtual void onString(const Ast::String & string){
            // Commands
            std::cout << "Found String: " << string.toString() << std::endl;
            exp.setType(ExpressionBuilder::String);
            exp.setExpression(Expression(string.toString()));
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
            exp.setType(ExpressionBuilder::Function);
            exp.setExpression(func);
        }

        virtual void onKeyword(const Ast::Keyword & keyword){
            // Triggers or constants
            Expression word = Expression(keyword.toString());//handleKeyWord(keyword.toString());
            std::cout << "Found Keyword: " << keyword.toString() << "' translated as: " << word.get() << std::endl;
            exp.setType(ExpressionBuilder::Keyword);
            exp.setExpression(word);
        }
        
        virtual void onNumber(const Ast::Number & number){
            // Numerals
            std::cout << "Found Number: " << number.toString()  <<  std::endl;
            exp.setType(ExpressionBuilder::Number);
            exp.setExpression(Expression(number.toString()));
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
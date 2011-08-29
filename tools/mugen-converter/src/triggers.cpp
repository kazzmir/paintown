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

bool stringIdentifiers(const std::string & word){
    return (match("xveladd", word) ||
         match("yveladd", word) ||
         match("type", word) ||
         match("animtype", word) ||
         match("airtype", word) ||
         match("groundtype", word) ||
         match("damage", word) ||
         match("hitcount", word) ||
         match("fallcount", word) ||
         match("hitshaketime", word) ||
         match("hittime", word) ||
         match("slidetime", word) ||
         match("ctrltime", word) ||
         match("recovertime", word) ||
         match("xoff", word) ||
         match("yoff", word) ||
         match("xvel", word) ||
         match("yvel", word) ||
         match("yaccel", word) ||
         match("chainid", word) ||
         match("guarded", word) ||
         match("isbound", word) ||
         match("fall", word) ||
         match("fall.damage", word) ||
         match("fall.xvel", word) ||
         match("fall.yvel", word) ||
         match("fall.recover", word) ||
         match("fall.recovertime", word) ||
         match("fall.kill", word) ||
         match("fall.envshake.time", word) ||
         match("fall.envshake.freq", word) ||
         match("fall.envshake.ampl", word) ||
         match("fall.envshake.phase", word));
}

/* TODO FIXME and all that
 * Assuming that an object named player and an object named world is going to be passed to States for evaluation
 * And also assuming that the names of the functions are going to be what they are below (most likely most of it will change)
 */;
static std::string handleKeyWord(const Expression & expression){
    const std::string & keyword = expression.getKeyword();
    if (match("AILevel", keyword)){
    } else if (match("Abs", keyword)){
        return "math.fabs";
    } else if (match("Acos", keyword)){
        return "math.acos";
    } else if (match("Alive", keyword)){
        return "self.player.isAlive()";
    } else if (match("Anim", keyword)){
        return "self.player.currentAnimation()";
    } else if (match("AnimElem", keyword)){
    } else if (match("AnimElemNo", keyword)){
    } else if (match("AnimElemTime", keyword)){
        return "self.player.animationElementTime";
    } else if (match("AnimExist", keyword)){
    } else if (match("AnimTime", keyword)){
        return "self.player.getAnimationTime()";
    } else if (match("Asin", keyword)){
        return "math.asin";
    } else if (match("Atan", keyword)){
        return "math.atan";
    } else if (match("AuthorName", keyword)){
        return "self.player.getAuthor()";
    } else if (match("BackEdgeBodyDist", keyword)){
        return "self.player.getBackEdgeBodyDistance()";
    } else if (match("BackEdgeDist", keyword)){
        return "self.player.getBackEdgeDistance()";
    } else if (match("CanRecover", keyword)){
        return "self.player.canRecover()";
    } else if (match("Ceil", keyword)){
        return "math.ceil";
    } else if (match("Command", keyword)){
        return "self.player.evaluateCommand()";
    } else if (match("Const", keyword)){
    } else if (match("Const240p", keyword)){
    } else if (match("Const480p", keyword)){
    } else if (match("Const720p", keyword)){
    } else if (match("Cos", keyword)){
        return "math.cos";
    } else if (match("Ctrl", keyword)){
        return "self.player.hasControl()";
    } else if (match("DrawGame", keyword)){

    } else if (match("E", keyword)){
        //return new Keyword("math.e");
    } else if (match("Exp", keyword)){
        return "math.exp";
    } else if (match("FVar", keyword)){
    } else if (match("Facing", keyword)){
        return "self.player.getFacing()";
    } else if (match("Floor", keyword)){
        return "math.floor";
    } else if (match("FrontEdgeBodyDist", keyword)){
        return "self.player.getFrontEdgeBodyDistance()";
    } else if (match("FrontEdgeDist", keyword)){
        return "self.player.getFrontEdgeDistance()";
    } else if (match("GameTime", keyword)){
        return "world.getTime";
    } else if (match("GetHitVar", keyword)){
        return "self.player.getHitVariable";
    } else if (match("HitCount", keyword)){
    } else if (match("HitDefAttr", keyword)){
    } else if (match("HitDefAttr:state", keyword)){
        return "self.player.getHitDefAttributeState()";
    } else if (match("HitDefAttr:attribute", keyword)){
        return "self.player.getHitDefAttribute()";
    } else if (match("HitFall", keyword)){
        return "self.player.getHitFall()";
    } else if (match("HitOver", keyword)){
        return "self.player.getHitOver()";
    } else if (match("HitPauseTime", keyword)){
    } else if (match("HitShakeOver", keyword)){
        return "self.player.getHitShakeOver()";
    } else if (match("HitVel", keyword)){

    } else if (match("ID", keyword)){
    } else if (match("IfElse", keyword)){
        return "ifelse";
    } else if (match("InGuardDist", keyword)){
    } else if (match("IsHelper", keyword)){
    } else if (match("IsHomeTeam", keyword)){

    } else if (match("Life", keyword)){
    } else if (match("LifeMax", keyword)){
    } else if (match("Ln", keyword)){
        return "mugen.ln(" + expression.getArguments()[0].getKeyword() + ")";
    } else if (match("Log", keyword)){
        return "math.log" + expression.getArguments()[0].getKeyword() + ")";
    } else if (match("Lose", keyword)){

    } else if (match("MatchNo", keyword)){
    } else if (match("MatchOver", keyword)){
        return "world.isMatchOver()";
    } else if (match("MoveContact", keyword)){
        return "self.player.getMoveContact()";
    } else if (match("MoveGuarded", keyword)){
        return "self.player.getMoveGuarded()";
    } else if (match("MoveHit", keyword)){
        return "self.player.getMoveHit()";
    } else if (match("MoveReversed", keyword)){
    } else if (match("MoveType", keyword)){
        return "self.player.getMoveType()";
    } else if (match("Name", keyword)){
    } else if (match("NumEnemy", keyword)){
    } else if (match("NumExplod", keyword)){
    } else if (match("NumHelper", keyword)){
    } else if (match("NumPartner", keyword)){
    } else if (match("NumProj", keyword)){
    } else if (match("NumProjID", keyword)){
    } else if (match("NumTarget", keyword)){

    } else if (match("P1Name", keyword)){
    } else if (match("P2BodyDist x", keyword)){
        return "world.getPlayerBodyXDistance()";
    } else if (match("P2BodyDist y", keyword)){
        return "world.getPlayerBodyFDistance()";
    } else if (match("P2Dist x", keyword)){
        return "world.getPlayerXDistance()";
    } else if (match("P2Dist y", keyword)){
        return "world.getPlayerYDistance()";
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
        return "math.pi";
    } else if (match("PlayerIDExist", keyword)){
    } else if (match("Pos x", keyword)){
        return "self.player.getPositionX()";
    } else if (match("Pos y", keyword)){
        return "self.player.getPositionY()";
    } else if (match("Power", keyword)){
    } else if (match("PowerMax", keyword)){
    } else if (match("PrevStateNo", keyword)){
        return "self.player.getPreviousStateNumber()";
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
        return "world.getCurrentRound()";
    } else if (match("RoundState", keyword)){
        //return new Function("world.roundState");
    } else if (match("RoundsExisted", keyword)){
        return "self.player.roundsExisted()";
    } else if (match("ScreenPos", keyword)){
    } else if (match("SelfAnimExist", keyword)){
        return "self.player.selfAnimExist";
    } else if (match("Sin", keyword)){
        return "math.sin";
    } else if (match("StateNo", keyword)){
        return "self.player.getStateNumber()";
    } else if (match("StateType", keyword)){
        return "self.player.getStateType()";
    } else if (match("SysFVar", keyword)){
        return "self.player.getSystemFloatVariable";
    } else if (match("SysVar", keyword)){
        return "self.player.getSystemVariable";
    } else if (match("Tan", keyword)){
        return "math.tan";
    } else if (match("TeamMode", keyword)){
    } else if (match("TeamSide", keyword)){
    } else if (match("TicksPerSecond", keyword)){
    } else if (match("Time", keyword)){
        return "world.getTime()";
    } else if (match("TimeMod", keyword)){

    } else if (match("UniqHitCount", keyword)){

    } else if (match("Var", keyword)){
        return "self.player.getVariable";
    } else if (match("Vel x", keyword)){
        return "self.player.getVelocityX()";
    } else if (match("Vel y", keyword)){
        return "self.player.getVelocityY()";
    } else if (match("Win", keyword)){
    } else if (stringIdentifiers(keyword)){
        return "'" + keyword + "'";
    } else {
        std::cout << "Unhandled keyword: " << keyword << std::endl;
    }
    
    return keyword;
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
    for (std::vector<ExpressionBuilder *>::const_iterator i = copy.arguments.begin(); i != copy.arguments.end(); ++i){
        ExpressionBuilder * builder = *i;
        if (builder != NULL){
            arguments.push_back(new ExpressionBuilder(*builder));
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
    for (std::vector<ExpressionBuilder *>::iterator i = arguments.begin(); i != arguments.end(); ++i){
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
    for (std::vector<ExpressionBuilder *>::const_iterator i = copy.arguments.begin(); i != copy.arguments.end(); ++i){
        ExpressionBuilder * builder = *i;
        if (builder != NULL){
            arguments.push_back(new ExpressionBuilder(*builder));
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
        case ValueList:{
            std::string array = "[";
            for(std::vector<ExpressionBuilder *>::iterator i = valueList.begin(); i != valueList.end(); ++i){
                array += crawlComplex(*i) + ", ";
            }
            return array.substr(0,array.size()-2) + "]"; 
            break;
        }
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
        case Identifier:{
            return handleKeyWord(expression);
            break;
        }
        case Helper:
            return expression.get();
            break;
        case String:
            return "'" + expression.get() + "'";
            break;
        case Function:
            return handleKeyWord(expression);
            break;
        case Keyword:
            return handleKeyWord(expression);
            break;
        case Number:
            return expression.get();
            break;
        case Custom:
            return expression.get();
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
std::vector<ExpressionBuilder *> & ExpressionBuilder::getArguments(){
    return arguments;
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
            //name = getNextFunctionName() + "()";
            //std::string main = handleBuilder(crawl(&builder));
            name = handleBuilder(crawl(&builder));
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
            } else if (op == "&&"){
                return "and";
            } else if (op == "||"){
                return "or";
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
            Content checker(2, function + " = lambda : " + "(" + against + " in range(" + args[0].get() + ", " + args[1].get() + "))");
            functions.push_back(checker);
            return function + "()";
        }
        
        const std::string createIfElse(ExpressionBuilder * ifelse){
            std::string function = getNextFunctionName();
            std::vector<ExpressionBuilder *> arguments = ifelse->getArguments();
            ExpressionBuilder * condition = crawl(arguments[0]);
            ExpressionBuilder * ifTrue = crawl(arguments[1]);
            ExpressionBuilder * ifFalse = crawl(arguments[2]);
            Content content(2, "def " + function + "():");
                content.addLine(3, "if " + condition->get() + ":");
                    content.addLine(4, "return " + ifTrue->get());
                content.addLine(3, "else:");
                    content.addLine(4, "return " + ifFalse->get());
            functions.push_back(content);
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
                        newBuilder->setType(ExpressionBuilder::Custom);
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
                        newBuilder->setType(ExpressionBuilder::Custom);
                        newBuilder->setExpression(Expression(range));
                        return newBuilder;
                    } else if (builder->getRightComplex()->getType() == ExpressionBuilder::ValueList){
                        /* TODO */
                        std::vector<ExpressionBuilder *> & values = builder->getRightComplex()->getValueList();
                        std::string array;
                        for(std::vector<ExpressionBuilder *>::iterator i = values.begin(); i != values.end(); ++i){
                            ExpressionBuilder * value = crawl(*i);
                            array += value->get() + ", ";
                        }
                        std::string function = getNextFunctionName();
                        Content checker(2, function + " = lambda : " + "(" + crawl(builder->getLeftComplex())->get() + " in [" + (array.empty() ? "" : array.substr(0,array.size()-2) ) + "])");
                        functions.push_back(checker);
                        ExpressionBuilder * newBuilder = new ExpressionBuilder();
                        deletable.push_back(newBuilder);
                        newBuilder->setType(ExpressionBuilder::Custom);
                        newBuilder->setExpression(Expression(function + "()"));
                        return newBuilder;
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
                        Content infix(2, function + " = lambda : " + "(" + leftExpression + " " + op + " " + rightExpression + ")");
                        functions.push_back(infix);
                        ExpressionBuilder * newBuilder = new ExpressionBuilder();
                        deletable.push_back(newBuilder);
                        newBuilder->setType(ExpressionBuilder::Custom);
                        newBuilder->setExpression(Expression(function+"()"));
                        return newBuilder;
                    }
                } else if (builder->getLeftComplex() != NULL && builder->getRightComplex() == NULL){
                    if (builder->getType() == ExpressionBuilder::Function){
                        ExpressionBuilder * left =  builder->getLeftComplex();
                        if (left != NULL){
                            std::string keyword = handleKeyWord(left->getExpression().getKeyword());
                            if (keyword == "ifelse"){
                                std::string ifelse = createIfElse(builder);
                                ExpressionBuilder * newBuilder = new ExpressionBuilder();
                                deletable.push_back(newBuilder);
                                newBuilder->setType(ExpressionBuilder::Custom);
                                newBuilder->setExpression(Expression(ifelse));
                                return newBuilder;
                            } else {
                                std::string function = getNextFunctionName();
                                std::string arguments;
                                for (std::vector<ExpressionBuilder *>::iterator i = builder->getArguments().begin(); i != builder->getArguments().end(); ++i){
                                    ExpressionBuilder * ex = crawl(*i);
                                    if (ex != NULL){
                                        arguments += ex->get() + ", ";
                                    }
                                }
                                Content content(2, function + " = lambda : " + keyword + "("+ (arguments.empty() ? "" : arguments.substr(0,arguments.size()-2)) + ")" );
                                functions.push_back(content);
                                ExpressionBuilder * newBuilder = new ExpressionBuilder();
                                deletable.push_back(newBuilder);
                                newBuilder->setType(ExpressionBuilder::Custom);
                                newBuilder->setExpression(Expression(function+"()"));
                                return newBuilder;
                            }
                        }
                    } else if (builder->getType() == ExpressionBuilder::Unary){
                        /* This should be a unary operation */
                        ExpressionBuilder * left =  crawl(builder->getLeftComplex());
                        if (left != NULL){
                            const std::string & leftExpression = handleBuilder(crawl(left));
                            const std::string & op = convertUnaryOperator(builder->getOperator());
                            std::string function = getNextFunctionName();
                            Content unary(2, "def " + function + "():");
                                unary.addLine(3, "return (" + op + "(" + leftExpression + "))");
                            functions.push_back(unary);
                            ExpressionBuilder * newBuilder = new ExpressionBuilder();
                            deletable.push_back(newBuilder);
                            newBuilder->setType(ExpressionBuilder::Custom);
                            newBuilder->setExpression(Expression(function + "()"));
                            return newBuilder;
                        }
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
                Content content(2, function + " = lambda : " + builder->get());
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
        
        virtual void onValue(const Ast::Value & value){
            std::cout << "Found Value: " << value.toString() << std::endl;
        }
        
        virtual void onValueList(const Ast::ValueList & values){
            //std::cout << "Found Value List: " << values.toString() << std::endl;
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
        virtual void onHitDefAttribute(const Ast::HitDefAttribute & attribute){
            //std::cout << "Found Hitdef Attribute: " << attribute.toString() << std::endl;
            exp.setType(ExpressionBuilder::String);
            exp.setExpression(Expression(attribute.toString()));
        }
        virtual void onHitDefAttackAttribute(const Ast::HitDefAttackAttribute & attribute){
            //std::cout << "Found Hitdef Attack Attribute: " << attribute.toString() << std::endl;
            exp.setType(ExpressionBuilder::ValueList);
            while (true){
                std::string value;
                try {
                    attribute >> value;
                    ExpressionBuilder * item = new ExpressionBuilder();
                    item->setType(ExpressionBuilder::String);
                    item->setExpression(Expression(value));
                    exp.addToList(item);
                } catch (const Ast::Exception & ex) {
                    break;
                }
            }
        }
        virtual void onRange(const Ast::Range & range){
            //std::cout << "Found range: " << range.toString() << std::endl;
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
            //std::cout << "Found Infix: " <<  expression.toString() << std::endl;
            
            exp.setType(ExpressionBuilder::Infix);
            ExpressionBuilder * expLeft = new ExpressionBuilder(convert(*expression.getLeft()));
            exp.setLeft(expLeft);
            
            ExpressionBuilder * expRight = new ExpressionBuilder(convert(*expression.getRight()));
            exp.setRight(expRight);
            
            exp.setOperator(expression.infixName(expression.getExpressionType()));
        }
        virtual void onExpressionUnary(const Ast::ExpressionUnary & expression){
            //std::cout << "Found Unary: " <<  expression.toString() << std::endl;
            exp.setType(ExpressionBuilder::Unary);
            ExpressionBuilder * left = new ExpressionBuilder(convert(*expression.getExpression()));
            exp.setLeft(left);
            exp.setOperator(expression.prefixName(expression.getExpressionType()));
        }
        virtual void onIdentifier(const Ast::Identifier & identifier){
            // Triggers or Constants
            //std::cout << "Found Identifier: " << identifier.toString() << std::endl;
            // Check for state types, convert to string
            if (match(identifier.toString(), "s") || 
                match(identifier.toString(), "c") || 
                match(identifier.toString(), "a") || 
                match(identifier.toString(), "i") ||  
                match(identifier.toString(), "l") || 
                match(identifier.toString(), "h") || 
                match(identifier.toString(), "n") || 
                match(identifier.toString(), "u")){
                exp.setType(ExpressionBuilder::String);
                exp.setExpression(Expression(identifier.toString()));
            } else {
                exp.setType(ExpressionBuilder::Identifier);
                exp.setExpression(Expression(identifier.toString()));
            }
        }
        
        virtual void onHelper(const Ast::Helper & helper){
            //std::cout << "Found Helper: " << helper.toString() << std::endl;
        }

        virtual void onString(const Ast::String & string){
            // Commands
            //std::cout << "Found String: " << string.toString() << std::endl;
            exp.setType(ExpressionBuilder::String);
            exp.setExpression(Expression(string.toString()));
        }
        
        virtual void onFunction(const Ast::Function & function){
            // Triggers w/ arguments
            exp.setType(ExpressionBuilder::Function);
            ExpressionBuilder * expLeft = new ExpressionBuilder();
            expLeft->getExpression().setKeyword(function.getName());
            exp.setLeft(expLeft);
            
            std::vector<ExpressionBuilder *> & arguments = exp.getArguments();
            for (unsigned int i = 0;;++i){
                const Ast::Value * value = function.getArg(i);
                if (value){
                    arguments.push_back(new ExpressionBuilder(convert(*value)));
                } else {
                    break;
                }
            }
        }

        virtual void onKeyword(const Ast::Keyword & keyword){
            // Triggers or constants
            Expression word = Expression(keyword.toString());//handleKeyWord(keyword.toString());
            //std::cout << "Found Keyword: " << keyword.toString() << "' translated as: " << word.get() << std::endl;
            exp.setType(ExpressionBuilder::Keyword);
            exp.setExpression(word);
        }
        
        virtual void onNumber(const Ast::Number & number){
            // Numerals
            //std::cout << "Found Number: " << number.toString()  <<  std::endl;
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
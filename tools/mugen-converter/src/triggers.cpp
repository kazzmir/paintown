#include "triggers.h"
#include "tools.h"

#include <iostream>

#include "ast/all.h"

using namespace Mugen;
using namespace TriggerHandler;

const std::string handleKeyWord(const std::string & keyword){
    if (match("AILevel", keyword)){
    } else if (match("Abs", keyword)){
        return "math.fabs";
    } else if (match("Acos", keyword)){
        return "math.acos";
    } else if (match("Alive", keyword)){
    } else if (match("Anim", keyword)){
    } else if (match("AnimElem", keyword)){
    } else if (match("AnimElemNo", keyword)){
    } else if (match("AnimElemTime", keyword)){
    } else if (match("AnimExist", keyword)){
    } else if (match("AnimTime", keyword)){
    } else if (match("Asin", keyword)){
        return "math.asin";
    } else if (match("Atan", keyword)){
        return "math.atan";
    } else if (match("AuthorName", keyword)){
        //return "paintown";
    } else if (match("BackEdgeBodyDist", keyword)){
    } else if (match("BackEdgeDist", keyword)){

    } else if (match("CanRecover", keyword)){
    } else if (match("Ceil", keyword)){
        return "math.ceil";
    } else if (match("Command", keyword)){
    } else if (match("Const", keyword)){
    } else if (match("Const240p", keyword)){
    } else if (match("Const480p", keyword)){
    } else if (match("Const720p", keyword)){
    } else if (match("Cos", keyword)){
        return "math.cos";
    } else if (match("Ctrl", keyword)){
    } else if (match("D", keyword)){
    } else if (match("DrawGame", keyword)){

    } else if (match("E", keyword)){
    } else if (match("Exp", keyword)){

    } else if (match("FVar", keyword)){
    } else if (match("Facing", keyword)){
    } else if (match("Floor", keyword)){
        return "math.floor";
    } else if (match("FrontEdgeBodyDist", keyword)){
    } else if (match("FrontEdgeDist", keyword)){

    } else if (match("GameTime", keyword)){
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
    } else if (match("InGuardDist", keyword)){
    } else if (match("IsHelper", keyword)){
    } else if (match("IsHomeTeam", keyword)){

    } else if (match("Life", keyword)){
    } else if (match("LifeMax", keyword)){
    } else if (match("Ln", keyword)){
        return "math.log";
    } else if (match("Log", keyword)){
        return "math.log";
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
        return "math.pi";
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
        return "random()";
    } else if (match("RootDist", keyword)){
    } else if (match("RoundNo", keyword)){
    } else if (match("RoundState", keyword)){
    } else if (match("RoundsExisted", keyword)){

    } else if (match("ScreenPos", keyword)){
    } else if (match("SelfAnimExist", keyword)){
    } else if (match("Sin", keyword)){
        return "math.sin";
    } else if (match("StateNo", keyword)){
    } else if (match("StateType", keyword)){
    } else if (match("SysFVar", keyword)){
    } else if (match("SysVar", keyword)){

    } else if (match("Tan", keyword)){
        return "math.tan";
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


const std::string TriggerHandler::convert(const Ast::Value & value){
    class ExpressionWalker : public Ast::Walker{
    public:
        ExpressionWalker(){
        }
        virtual void onValueList(const Ast::ValueList & values){
            for (unsigned int i = 0;;++i){
                Ast::Value * value = values.get(i);
                if (value){
                    convert(*value);
                } else {
                    break;
                }
            }
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
            convert(*expression.getLeft());
            convert(*expression.getRight());
        }
        virtual void onExpressionUnary(const Ast::ExpressionUnary & expression){
            std::cout << "Found Unary: " <<  expression.toString() << std::endl;
            convert(*expression.getExpression());
        }
        virtual void onIdentifier(const Ast::Identifier & identifier){
            std::cout << "Found Identifier: " << identifier.toString() << "' translated as: " << handleKeyWord(identifier.toString()) << std::endl;
        }
        
        virtual void onHelper(const Ast::Helper & helper){
            std::cout << "Found Helper: " << helper.toString() << std::endl;
        }

        virtual void onString(const Ast::String & string){
            std::cout << "Found String: " << string.toString() << std::endl;
        }
        
        virtual void onFunction(const Ast::Function & string){
            std::cout << "Found Function: " << string.toString() << std::endl;
        }

        virtual void onKeyword(const Ast::Keyword & keyword){
            std::cout << "Found Keyword '" << keyword.toString() << "' translated as: " << handleKeyWord(keyword.toString()) << std::endl;
            
        }
        
        virtual void onNumber(const Ast::Number & keyword){
            std::cout << "Found Number: " << keyword.toString() << std::endl;
        }
        
    };
    ExpressionWalker walker;
    value.walk(walker);
    
    return "";
}
#include "triggers.h"

#include <iostream>

#include "ast/all.h"

using namespace Mugen;
using namespace TriggerHandler;

const std::string handleKeyWord(const Ast::AttributeSimple & simple){
    /*
AILevel
Abs
Acos
Alive
Anim
AnimElem
AnimElemNo
AnimElemTime
AnimExist
AnimTime
Asin
Atan
AuthorName
B
BackEdgeBodyDist
BackEdgeDist
C
CanRecover
Ceil
Command
Const
Const240p
Const480p
Const720p
Cos
Ctrl
D
DrawGame
E
E
Exp
F
FVar
Facing
Floor
FrontEdgeBodyDist
FrontEdgeDist
G
GameTime
GetHitVar
H
HitCount
HitDefAttr
HitFall
HitOver
HitPauseTime
HitShakeOver
HitVel
I
ID
IfElse
InGuardDist
IsHelper
IsHomeTeam
L
Life
LifeMax
Ln
Log
Lose
M
MatchNo
MatchOver
MoveContact
MoveGuarded
MoveHit
MoveReversed
MoveType
N
Name
NumEnemy
NumExplod
NumHelper
NumPartner
NumProj
NumProjID
NumTarget
P
P1Name
P2BodyDist
P2Dist
P2Life
P2MoveType
P2Name
P2StateNo
P2StateType
P3Name
P4Name
PalNo
ParentDist
Pi
PlayerIDExist
Pos
Power
PowerMax
PrevStateNo
ProjCancelTime
ProjContact
ProjContactTime
ProjGuarded
ProjGuardedTime
ProjHit
ProjHitTime
R
Random
RootDist
RoundNo
RoundState
RoundsExisted
S
ScreenPos
SelfAnimExist
Sin
StateNo
StateType
SysFVar
SysVar
T
Tan
TeamMode
TeamSide
TicksPerSecond
Time
TimeMod
U
UniqHitCount
V
Var
Vel
W
Win
    */
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
            std::cout << "Found Identifier: " << identifier.toString() << std::endl;
        }
        
        virtual void onHelper(const Ast::Helper & identifier){
            std::cout << "Found Helper: " << identifier.toString() << std::endl;
        }

        virtual void onString(const Ast::String & string){
            std::cout << "Found String: " << string.toString() << std::endl;
        }
        
        virtual void onFunction(const Ast::Function & string){
            std::cout << "Found Function: " << string.toString() << std::endl;
        }

        virtual void onKeyword(const Ast::Keyword & keyword){
            std::cout << "Found Keyword: " << keyword.toString() << std::endl;
        }
        
        virtual void onNumber(const Ast::Number & keyword){
            std::cout << "Found Number: " << keyword.toString() << std::endl;
        }
        
    };
    ExpressionWalker walker;
    value.walk(walker);
    
    return "";
}
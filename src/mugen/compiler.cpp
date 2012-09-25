/*

Triggers

A
-----------------------
AILevel
Abs - done
Acos - done
Alive - done
Anim - done
AnimElem - done
AnimElemNo - done
AnimElemTime - done
AnimExist - done
AnimTime - done
Asin - done
Atan - done
AuthorName - done

B
-----------------------
BackEdgeBodyDist - done
BackEdgeDist - done

C
-----------------------
CanRecover - done
Ceil - done
Command - done
Const - partially done
Const240p
Const480p
Const720p
Cos - done
Ctrl - done

D
-----------------------
DrawGame

E
-----------------------
E - done
Exp - done

F
-----------------------
FVar - done
Facing - done
Floor - done
FrontEdgeBodyDist - done
FrontEdgeDist - done

G
-----------------------
GameTime - done
GetHitVar - partially done

H
-----------------------
HitCount - done
HitDefAttr
HitFall - done
HitOver - done
HitPauseTime - done
HitShakeOver - done
HitVel - done

I
-----------------------
ID
IfElse - done
InGuardDist - done
IsHelper - done
IsHomeTeam

L
-----------------------
Life - done
LifeMax - done
Ln - done
Log - done
Lose

M
-----------------------
MatchNo - done
MatchOver - done
MoveContact
MoveGuarded - done
MoveHit
MoveReversed
MoveType - done

N
-----------------------
Name - done
NumEnemy
NumExplod - done
NumHelper
NumPartner
NumProj
NumProjID
NumTarget

P
-----------------------
P1Name - done
P2BodyDist - done
P2Dist - done
P2Life - done
P2MoveType - done
P2Name - done
P2StateNo - done
P2StateType - done
P3Name
P4Name
PalNo - done
ParentDist
Pi - done
PlayerIDExist
Pos - done
Power - done
PowerMax - done
PrevStateNo - done
ProjCancelTime
ProjContact
ProjContactTime
ProjGuarded
ProjGuardedTime
ProjHit
ProjHitTime

R
-----------------------
Random - done
RootDist
RoundNo - done
RoundState - done
RoundsExisted

S
-----------------------
ScreenPos - done
SelfAnimExist - done
Sin - done
StateNo - done
StateType - done
SysFVar - done
SysVar - done

T
-----------------------
Tan - done
TeamMode
TeamSide
TicksPerSecond - done
Time - done
TimeMod

U
-----------------------
UniqHitCount - done

V
-----------------------
Var - done
Vel - done

W
-----------------------
Win

*/


#include "ast/all.h"
#include "compiler.h"
#include "exception.h"
#include "characterhud.h"
#include "character.h"
#include "helper.h"
#include "stage.h"
#include "util/funcs.h"
#include "util/regex.h"
#include <math.h>
#include <sstream>
#include <string>
#include "config.h"
#include "projectile.h"

namespace PaintownUtil = ::Util;
using std::string;
using std::ostringstream;
using std::vector;
using std::map;
    
/* This isn't so much a compiler as a partial evaluating interpreter (you can
 * tell its not a compiler because there are explicit calls to `evaluate' at runtime).
 * Here is a nice quote from Joe Marshall on the difference between interpreters
 * and compilers:
 * 
 *  "If you have a machine
 *   that runs programs written in language M, but you have a program
 *   written in language L, you can proceed in one of two ways:
 *
 *   1) You can modify the *machine* to run programs in a different language.
 *     An interpreter is a program that transforms a machine into
 *     another machine.
 *
 *   2) You can find an `equivalant' program in the language the machine does run.
 *     A compiler is a program that transforms a program into another program."
 *  -- Joe Marshall 10/14/2010
 *  http://www.mail-archive.com/users@racket-lang.org/msg02358.html
 */

namespace Mugen{

static void raise(const RuntimeValue & value, const string & expected){
    ostringstream out;
    out << "Not a " << expected << " instead was " << value.canonicalName();
    throw MugenException(out.str(), __FILE__, __LINE__);
}

string toString(const RuntimeValue & value){
    if (value.isString()){
        return value.getStringValue();
    }
    raise(value, "string");

    return "";
}

double RuntimeValue::toNumber() const {
    if (isDouble()){
        return getDoubleValue();
    }
    if (isBool()){
        if (getBoolValue()){
            return 1;
        } else {
            return 0;
        }
    }

    raise(*this, "number");

    return 0;
}

bool RuntimeValue::toBool() const {
    if (isBool()){
        return getBoolValue();
    }
    if (isDouble()){
        return getDoubleValue() != 0;
    }
    raise(*this, "bool");

    return false;
}

bool toBool(const RuntimeValue & value){
    return value.toBool();
}

double toNumber(const RuntimeValue & value){
    return value.toNumber();
}
    
RuntimeValue::RuntimeValue(Compiler::Value * value){
}

int toRangeLow(const RuntimeValue & value){
    if (value.isRange()){
        return value.getRangeLow();
    }
    raise(value, "range");

    return 0;
}

int toRangeHigh(const RuntimeValue & value){
    if (value.isRange()){
        return value.getRangeHigh();
    }
    raise(value, "range");

    return 0;
}
    
RuntimeValue RuntimeValue::operator+(const RuntimeValue & other) const {
    if (type == RuntimeValue::Double && other.type == RuntimeValue::Double){
        return RuntimeValue(this->double_value + other.double_value);
    }
    throw MugenRuntimeException("cannot add values together", __FILE__, __LINE__);
}

bool RuntimeValue::operator==(const RuntimeValue & value2) const {
    const RuntimeValue & value1 = *this;
    if (value1.type == RuntimeValue::Invalid || value2.type == RuntimeValue::Invalid){
        throw MugenRuntimeException("invalid value", __FILE__, __LINE__);
    }
    switch (value1.type){
        case RuntimeValue::ListOfString : {
            switch (value2.type){
                case RuntimeValue::String : {
                    const vector<string> & strings = value1.strings_value;
                    for (vector<string>::const_iterator it = strings.begin(); it != strings.end(); it++){
                        const string & check = *it;
                        if (check == value2.string_value){
                            return true;
                        }
                    }
                    return false;
                }
                default: return false;
            }
            break;
        }
        case RuntimeValue::String : {
            switch (value2.type){
                case RuntimeValue::ListOfString : {
                    return value2 == value1;
                }
                case RuntimeValue::String : {
                    return toString(value1) == toString(value2);
                }
                default: return false;
            }
            break;
        }
        case RuntimeValue::RangeType : {
            switch (value2.type){
                case RuntimeValue::Double : return value2 == value1;
                default: return false;
            }
        }
        case RuntimeValue::Bool:
        case RuntimeValue::Double : {
            switch (value2.type){
                case RuntimeValue::Double : {
                    double epsilon = 0.0000001;
                    return fabs(value1.toNumber() - value2.toNumber()) < epsilon;
                }
                case RuntimeValue::RangeType : {
                    return value1.toNumber() > toRangeLow(value2) &&
                           value1.toNumber() < toRangeHigh(value2);
                }
                default: return false;
            }
            break;
        }
        /* true if the first value is a subset of the second value */
        case RuntimeValue::StateType : {
            switch (value2.type){
                case RuntimeValue::StateType : {
                    return (!value1.attribute.standing || (value1.attribute.standing && value2.attribute.standing)) &&
                           (!value1.attribute.crouching || (value1.attribute.crouching && value2.attribute.crouching)) &&
                           (!value1.attribute.lying || (value1.attribute.lying && value2.attribute.lying)) &&
                           (!value1.attribute.aerial || (value1.attribute.aerial && value2.attribute.aerial));
                }
                default : return false;
            }
            break;
        }
        case RuntimeValue::AttackAttribute : {
            switch (value2.type){
                case RuntimeValue::AttackAttribute : {
                    const vector<AttackType::Attribute> & setLeft = value1.attackAttributes;
                    const vector<AttackType::Attribute> & setRight = value2.attackAttributes;
                    map<AttackType::Attribute, bool> all;
                    for (vector<AttackType::Attribute>::const_iterator it = setRight.begin(); it != setRight.end(); it++){
                        all[*it] = true;
                    }
                    
                    for (vector<AttackType::Attribute>::const_iterator it = setLeft.begin(); it != setLeft.end(); it++){
                        if (!all[*it]){
                            return false;
                        }
                    }

                    return true;
                }
                default : return false;
            }
            break;
        }
        default: return false;
    }

    return false;
}

static bool lessThanDouble(double a, double b){
    return a < b;
}

static bool lessThanEqualsDouble(double a, double b){
    return a <= b;
}

static bool greaterThanDouble(double a, double b){
    return a > b;
}

static bool greaterThanEqualsDouble(double a, double b){
    return a >= b;
}

static bool compareRuntimeValues(const RuntimeValue & value1, const RuntimeValue & value2, bool (*compareDoubles)(double a, double b)){
    if (value1.type == RuntimeValue::Invalid || value2.type == RuntimeValue::Invalid){
        throw MugenRuntimeException("invalid value", __FILE__, __LINE__);
    }
    switch (value1.type){
        case RuntimeValue::Bool:
        case RuntimeValue::Double: {
            switch (value2.type){
                case RuntimeValue::Bool:
                case RuntimeValue::Double: return compareDoubles(value1.toNumber(), value2.toNumber());
                default: break;
            }
        }
        default: break;
    }
    std::ostringstream out;
    out << "Don't know how to compare " << value1.canonicalName() << " with " << value2.canonicalName();
    throw MugenRuntimeException(out.str(), __FILE__, __LINE__);
}

bool RuntimeValue::operator<=(const RuntimeValue & value2) const {
    return compareRuntimeValues(*this, value2, lessThanEqualsDouble);
}

bool RuntimeValue::operator<(const RuntimeValue & value2) const {
    return compareRuntimeValues(*this, value2, lessThanDouble);
}

bool RuntimeValue::operator>(const RuntimeValue & value2) const {
    return compareRuntimeValues(*this, value2, greaterThanDouble);
}

bool RuntimeValue::operator>=(const RuntimeValue & value2) const {
    return compareRuntimeValues(*this, value2, greaterThanEqualsDouble);
}

const Character & EmptyEnvironment::getCharacter() const {
    throw MugenException("Cannot get a character from an empty environment", __FILE__, __LINE__);
}

const Mugen::Stage & EmptyEnvironment::getStage() const {
    throw MugenException("Cannot get a stage from an empty environment", __FILE__, __LINE__);
}

const std::vector<std::string> EmptyEnvironment::getCommands() const {
    throw MugenException("Cannot get commands from an empty environment", __FILE__, __LINE__);
}
    
RuntimeValue EmptyEnvironment::getArg1() const {
    throw MugenException("Cannot get arg1 from an empty environment", __FILE__, __LINE__);
}
                        
template<class ReturnType> Compiler::Value * getCharacterField(ReturnType (Character::*getter)() const, const std::string & name){
    class CharacterGetter: public Compiler::Value {
    public:
        CharacterGetter(ReturnType (Character::*getter)() const, const std::string & name):
        getter(getter),
        name(name){
        }
        
        ReturnType (Character::*getter)() const;
        const std::string name;

        RuntimeValue evaluate(const Environment & environment) const {
            const Character & guy = environment.getCharacter();
            return RuntimeValue((guy.*getter)());
        }

        std::string toString() const {
            return name;
        }

        Compiler::Value * copy() const {
            return new CharacterGetter(getter, name);
        }

    };

    return new CharacterGetter(getter, name);
}

}

namespace Mugen{
namespace Compiler{

void compileError(const std::string & fail, const std::string & where, int line){
    throw MugenException(fail, where, line);
}

void runtimeError(const std::string & fail, const std::string & where, int line){
    throw MugenNormalRuntimeException(fail, where, line);
}

namespace{

class CompileWalker: public Ast::Walker {
public:
    CompileWalker():
    compiled(NULL){
    }

    Value * compiled;

    bool isStateType(const std::string & what){
        for (unsigned int i = 0; i < what.size(); i++){
            std::string id = what.substr(i, 1);
            if (id == StateType::Stand ||
                id == StateType::Crouch ||
                id == StateType::Air || 
                id == StateType::LyingDown){
                /* */
            } else {
                return false;
            }
        }

        return true;
    }

    Value * parseStateType(const std::string & what){
        RuntimeValue::StateTypes state;

        for (unsigned int i = 0; i < what.size(); i++){
            std::string id = what.substr(i, 1);
            if (id == StateType::Stand){
                state.standing = true;
            }
            if (id == StateType::Crouch){
                state.crouching = true;
            }
            if (id == StateType::Air){
                state.aerial = true;
            }
            if (id == StateType::LyingDown){
                state.lying = true;
            }
        }

        class Result: public Value {
        public:
            Result(RuntimeValue::StateTypes value):
            value(value){
            }

            RuntimeValue::StateTypes value;

            Value * copy() const {
                return new Result(value);
            }

            virtual std::string toString() const {
                std::ostringstream out;
                if (value.standing){
                    out << "S";
                }
                if (value.crouching){
                    out << "C";
                }
                if (value.aerial){
                    out << "A";
                }
                if (value.lying){
                    out << "L";
                }
                return out.str();
            }

            RuntimeValue evaluate(const Environment & environment) const {
                return RuntimeValue(value);
            }
        };

        return new Result(state);
    }

    static RuntimeValue convertStateType(const std::string & state){
        RuntimeValue::StateTypes attribute;
        if (state == StateType::Stand){
            attribute.standing = true;
        } else if (state == StateType::Crouch){
            attribute.crouching = true;
        } else if (state == StateType::Air){
            attribute.aerial = true;
        } else if (state == StateType::LyingDown){
            attribute.lying = true;
        }

        return RuntimeValue(attribute);
    }

    static std::vector<AttackType::Attribute> convertAttackType(const std::string & attack){
        std::vector<AttackType::Attribute> types;
        if (attack.size() == 2){
            if (attack[0] == 'N'){
                if (attack[1] == 'A'){
                    types.push_back(AttackType::NormalAttack);
                } else if (attack[1] == 'T'){
                    types.push_back(AttackType::NormalThrow);
                } else if (attack[1] == 'P'){
                    types.push_back(AttackType::NormalProjectile);
                }
            } else if (attack[0] == 'S'){
                if (attack[1] == 'A'){
                    types.push_back(AttackType::SpecialAttack);
                } else if (attack[1] == 'T'){
                    types.push_back(AttackType::SpecialThrow);
                } else if (attack[1] == 'P'){
                    types.push_back(AttackType::SpecialProjectile);
                }
            } else if (attack[0] == 'H'){
                if (attack[1] == 'A'){
                    types.push_back(AttackType::HyperAttack);
                } else if (attack[1] == 'T'){
                    types.push_back(AttackType::HyperThrow);
                } else if (attack[1] == 'P'){
                    types.push_back(AttackType::HyperProjectile);
                }
            } else if (attack[0] == 'A'){
                if (attack[1] == 'A'){
                    types.push_back(AttackType::NormalAttack);
                    types.push_back(AttackType::SpecialAttack);
                    types.push_back(AttackType::HyperAttack);
                } else if (attack[1] == 'T'){
                    types.push_back(AttackType::NormalThrow);
                    types.push_back(AttackType::SpecialThrow);
                    types.push_back(AttackType::HyperThrow);
                } else if (attack[1] == 'P'){
                    types.push_back(AttackType::NormalProjectile);
                    types.push_back(AttackType::SpecialProjectile);
                    types.push_back(AttackType::HyperProjectile);
                }
            } else {
                compileError(std::string("Invalid attack type '") + attack + "'", __FILE__, __LINE__);
            }
        }

        return types;

        /*
        std::map<std::string, AttackType::Attribute> attributes;
        attributes["NA"] = AttackType::NormalAttack;
        attributes["NT"] = AttackType::NormalThrow;
        attributes["NP"] = AttackType::NormalProjectile;
        attributes["SA"] = AttackType::SpecialAttack;
        attributes["ST"] = AttackType::SpecialThrow;
        attributes["SP"] = AttackType::SpecialProjectile;
        attributes["HA"] = AttackType::HyperAttack;
        attributes["HT"] = AttackType::HyperThrow;
        attributes["HP"] = AttackType::HyperProjectile;
        if (attributes.find(attack) != attributes.end()){
            return attributes[attack];
        }

        throw MugenException(std::string("Invalid attack type '") + attack + "'");
        */
    }

    Value * compileHitDefAttackAttribute(const Ast::HitDefAttackAttribute & attribute){
        std::vector<AttackType::Attribute> attacks;
        Ast::View view = attribute.view();
        try{
            while (true){
                std::string type;
                view >> type;
                type = PaintownUtil::upperCaseAll(type);
                std::vector<AttackType::Attribute> more = convertAttackType(type);
                attacks.insert(attacks.begin(), more.begin(), more.end());
            }
        } catch (const Ast::Exception & e){
            /* */
        }

        class Result: public Value {
        public:
            Result(const std::vector<AttackType::Attribute> & attacks):
            attacks(attacks){
            }

            Value * copy() const {
                return new Result(attacks);
            }
            
            std::vector<AttackType::Attribute> attacks;

            RuntimeValue evaluate(const Environment & environment) const {
                return RuntimeValue(attacks);
            }
        };

        return new Result(attacks);
    }
    
    virtual void onHitDefAttackAttribute(const Ast::HitDefAttackAttribute & attribute){
        compiled = compileHitDefAttackAttribute(attribute);
    }

    Value * compileHitDefAttribute(const Ast::HitDefAttribute & attribute){
        std::string stuff;
        attribute.view() >> stuff;
        stuff = PaintownUtil::upperCaseAll(stuff);
        return parseStateType(stuff);
    }
    
    virtual void onHitDefAttribute(const Ast::HitDefAttribute & attribute){
        compiled = compileHitDefAttribute(attribute);
    }
       
    Value * compileIdentifier(const Ast::Identifier & identifier){
        class JustString: public Value {
        public:
            JustString(const std::string & value):
                value(value){
                }

            std::string value;

            RuntimeValue evaluate(const Environment & environment) const {
                return RuntimeValue(value);
            }

            Value * copy() const {
                return new JustString(value);
            }
        };

        if (identifier == "e"){
            return compile(exp(1.0));
        }

        if (identifier == "id"){
            class ID: public Value{
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getObjectId());
                }

                virtual std::string toString() const {
                    return "ID";
                }

                Value * copy() const {
                    return new ID();
                }
            };

            return new ID();
        }

        if (identifier == "command"){
            class Command: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCommands());
                }

                virtual std::string toString() const {
                    return "command";
                }

                Value * copy() const {
                    return new Command();
                }
            };

            return new Command();
        }
        
        if (identifier == "anim"){
            class Animation: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getAnimation());
                }

                virtual std::string toString() const {
                    return "anim";
                }

                Value * copy() const {
                    return new Animation();
                }
            };

            return new Animation();
        }

        /* Mugen 1.0 */
        if (identifier == "ailevel"){
            class AILevel: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    /* TODO */
                    return RuntimeValue(0);
                }

                virtual std::string toString() const {
                    return "AILevel";
                }

                Value * copy() const {
                    return new AILevel();
                }
            };

            return new AILevel();
        }
        
        if (identifier == "alive"){
            class Alive: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getHealth() > 0);
                }

                virtual std::string toString() const {
                    return "alive";
                }

                Value * copy() const {
                    return new Alive();
                }
            };

            return new Alive();
        }

        /*
        if (PaintownUtil::matchRegex(PaintownUtil::lowerCaseAll(identifier.toString()), "projhit\\d*")){
            return compile(0);
        }

        if (PaintownUtil::matchRegex(PaintownUtil::lowerCaseAll(identifier.toString()), "projcontact\\d*")){
            return compile(0);
        }

        if (PaintownUtil::matchRegex(PaintownUtil::lowerCaseAll(identifier.toString()), "projguarded\\d*")){
            return compile(0);
        }
        */

        if (identifier == "numproj"){
            class NumProj: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue((int) environment.getStage().findProjectile(0, &environment.getCharacter()).size());
                }

                Value * copy() const {
                    return new NumProj();
                }
            };

            return new NumProj();
        }

        if (identifier == "ishelper"){
            class IsHelper: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().isHelper());
                }

                Value * copy() const {
                    return new IsHelper();
                }
            };

            return new IsHelper();
        }
        
        if (identifier == "numhelper"){
            class NumHelper: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getStage().countMyHelpers(&environment.getCharacter()));
                }

                virtual std::string toString() const {
                    return "numhelper";
                }

                Value * copy() const {
                    return new NumHelper();
                }
            };

            return new NumHelper();
        }

        if (identifier == "numenemy"){
            /* FIXME: return more than 1 in team mode */
            return compile(1);
        }

        if (identifier == "numpartner"){
            /* FIXME: return 1 in team mode */
            return compile(0);
        }

        if (identifier == "p1name" ||
            identifier == "name"){
            class P1Name: public Value{
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getName());
                }

                Value * copy() const {
                    return new P1Name();
                }
            };

            return new P1Name();
        }

        if (identifier == "p2name"){
            class P2Name: public Value{
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    const Character * enemy = environment.getStage().getEnemy(&environment.getCharacter());
                    return RuntimeValue(enemy->getName());
                }
                
                Value * copy() const {
                    return new P2Name();
                }
            };

            return new P2Name();
        }

        if (identifier == "p3name"){
            class P3Name: public Value{
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    /* FIXME */
                    return RuntimeValue(std::string());
                }

                Value * copy() const {
                    return new P3Name();
                }
            };

            return new P3Name();
        }

        if (identifier == "p4name"){
            class P4Name: public Value{
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    /* FIXME */
                    return RuntimeValue(std::string());
                }

                Value * copy() const {
                    return new P4Name();
                }
            };

            return new P4Name();
        }

        if (identifier == "authorname"){
            class AuthorName: public Value{
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getAuthor());
                }

                Value * copy() const {
                    return new AuthorName();
                }
            };

            return new AuthorName();
        }

        if (identifier == "facing"){
            class Facing: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    if (environment.getCharacter().getFacing() == FacingLeft){
                        return RuntimeValue(-1);
                    } else if (environment.getCharacter().getFacing() == FacingRight){
                        return RuntimeValue(1);
                    }
                    return RuntimeValue(0);
                }

                Value * copy() const {
                    return new Facing();
                }
            };

            return new Facing();
        }

        if (identifier == "backedgebodydist"){
            class BackEdgeBodyDist: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    int x = environment.getCharacter().getBackX();
                    switch (environment.getCharacter().getFacing()){
                        case FacingRight: return x - environment.getStage().maximumLeft(&environment.getCharacter());
                        case FacingLeft: return environment.getStage().maximumRight(&environment.getCharacter()) - x;
                    }
                }

                Value * copy() const {
                    return new BackEdgeBodyDist();
                }
            };

            return new BackEdgeBodyDist();
        }

        if (identifier == "backedgedist"){
            class BackEdgeDist: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    int x = environment.getCharacter().getX();
                    /* FIXME: handle screenbound in case maximumLeft/Right don't */
                    switch (environment.getCharacter().getFacing()){
                        case FacingRight: return x - environment.getStage().maximumLeft(&environment.getCharacter());
                        case FacingLeft: return environment.getStage().maximumRight(&environment.getCharacter()) - x;

                    }
                }

                Value * copy() const {
                    return new BackEdgeDist();
                }
            };

            return new BackEdgeDist();
        }

        if (identifier == "frontedgebodydist"){
            class FrontEdgeBodyDist: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    int x = environment.getCharacter().getFrontX();
                    switch (environment.getCharacter().getFacing()){
                        case FacingLeft: return x - environment.getStage().maximumLeft(&environment.getCharacter());
                        case FacingRight: return environment.getStage().maximumRight(&environment.getCharacter()) - x;
                    }
                }

                Value * copy() const {
                    return new FrontEdgeBodyDist();
                }
            };

            return new FrontEdgeBodyDist();
        }

        if (identifier == "frontedgedist"){
            class FrontEdgeDist: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    int x = environment.getCharacter().getX();
                    switch (environment.getCharacter().getFacing()){
                        case FacingLeft: return x - environment.getStage().maximumLeft(&environment.getCharacter());
                        case FacingRight: return environment.getStage().maximumRight(&environment.getCharacter()) - x;
                    }
                }

                Value * copy() const {
                    return new FrontEdgeDist();
                }
            };

            return new FrontEdgeDist();
        }

        if (identifier == "hitdefattr:state"){
            class HitDefAttrState: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    /* TODO: check if the player is in an attack state */
                    return convertStateType(environment.getCharacter().getHit().attribute.state);
                }

                Value * copy() const {
                    return new HitDefAttrState();
                }
            };

            return new HitDefAttrState();
        }

        if (identifier == "hitdefattr:attribute"){
            class HitDefAttrAttribute: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    /* TODO: check if the player is in an attack state */

                    const HitDefinition & hit = environment.getCharacter().getHit();

                    std::vector<AttackType::Attribute> all = convertAttackType(PaintownUtil::upperCaseAll(hit.attribute.attackType) + PaintownUtil::upperCaseAll(hit.attribute.physics));
                    return RuntimeValue(all);
                }

                Value * copy() const {
                    return new HitDefAttrAttribute();
                }
            };

            return new HitDefAttrAttribute();
        }

        if (identifier == "life"){
            class Life: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getHealth());
                }

                virtual std::string toString() const {
                    return "life";
                }

                Value * copy() const {
                    return new Life();
                }
            };

            return new Life();
        }

        if (identifier == "lifemax"){
            class LifeMax: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getMaxHealth());
                }
                
                virtual std::string toString() const {
                    return "lifemax";
                }

                Value * copy() const {
                    return new LifeMax();
                }
            };

            return new LifeMax();

        }

        if (identifier == "matchno"){
            class Wins: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    int wins = environment.getCharacter().getWins().size()+1;
                    return RuntimeValue(wins);
                }

                Value * copy() const {
                    return new Wins();
                }
            };

            return new Wins();
        }

        if (identifier == "matchover"){
            class Over: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getStage().isMatchOver());
                }

                Value * copy() const {
                    return new Over();
                }
            };

            return new Over();
        }

        if (identifier == "movetype"){
            class MoveType: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getMoveType());
                }

                virtual std::string toString() const {
                    return "movetype";
                }

                Value * copy() const {
                    return new MoveType();
                }
            };

            return new MoveType();
        }

        if (identifier == "p2movetype"){
            class MoveType2: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    const Character * enemy = environment.getStage().getEnemy(&environment.getCharacter());
                    return RuntimeValue(enemy->getMoveType());
                }
                
                virtual std::string toString() const {
                    return "p2movetype";
                }

                Value * copy() const {
                    return new MoveType2();
                }
            };

            return new MoveType2();
        }

        if (identifier == "random"){
            class Random: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    /* Returns a random number between 0 and 999, inclusive. */
                    return RuntimeValue(PaintownUtil::rnd(1000));
                }

                Value * copy() const {
                    return new Random();
                }
            };

            return new Random();
        }

        if (identifier == "roundno"){
            class RoundNumber: public Value {
                public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getStage().getGameInfo()->getRound().getRound());
                }
                
                virtual std::string toString() const {
                    return "RoundNo";
                }

                Value * copy() const {
                    return new RoundNumber();
                }

            };

            return new RoundNumber();
        }

        if (identifier == "roundsexisted"){
            class RoundsExisted: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    /* FIXME */
                    return RuntimeValue(0);
                }
                
                virtual std::string toString() const {
                    return "RoundsExisted";
                }

                Value * copy() const {
                    return new RoundsExisted();
                }
            };

            return new RoundsExisted();
        }

        if (identifier == "I"){
            class JustI: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(string("I"));
                }

                Value * copy() const {
                    return new JustI();
                }
            };

            return new JustI();
        }

        if (identifier == "H"){
            class JustH: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(string("H"));
                }

                virtual std::string toString() const {
                    return "H";
                }

                Value * copy() const {
                    return new JustH();
                }
            };
            return new JustH();
        }

        if (identifier == "p2stateno"){
            class P2StateNo: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    const Character * enemy = environment.getStage().getEnemy(&environment.getCharacter());
                    return RuntimeValue(enemy->getCurrentState());
                }

                Value * copy() const {
                    return new P2StateNo();
                }
            };

            return new P2StateNo();
        }

        if (identifier == "hitcount"){
            class HitCount: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getHitCount());
                }
                
                virtual std::string toString() const {
                    return "hitcount";
                }

                Value * copy() const {
                    return new HitCount();
                }
            };

            return new HitCount();
        }

        if (identifier == "uniqhitcount"){
            class UniqueHitCount: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    /* FIXME */
                    return RuntimeValue(environment.getCharacter().getHitCount());
                }

                Value * copy() const {
                    return new UniqueHitCount();
                }
            };

            return new UniqueHitCount();

        }

        if (identifier == "p2statetype"){
            class P2StateType: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    const Character * enemy = environment.getStage().getEnemy(&environment.getCharacter());
                    return RuntimeValue(convertStateType(enemy->getStateType()));
                }

                Value * copy() const {
                    return new P2StateType();
                }
            };

            return new P2StateType();
        }

        if (identifier == "wintime"){
            class WinTime: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return environment.getStage().getGameInfo()->getRound().isWinnerTime(environment.getCharacter());
                }

                Value * copy() const {
                    return new WinTime();
                }
            };

            return new WinTime();
        }

        if (identifier == "winko"){
            class Winko: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return environment.getStage().getGameInfo()->getRound().isWinnerKO(environment.getCharacter());
                }

                Value * copy() const {
                    return new Winko();
                }
            };

            return new Winko();
        }

        if (identifier == "winperfect"){
            class WinPerfect: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return environment.getStage().getGameInfo()->getRound().isWinnerPerfect(environment.getCharacter());
                }

                Value * copy() const {
                    return new WinPerfect();
                }
            };

            return new WinPerfect();
        }

        if (identifier == "win"){
            class Win: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return environment.getStage().getGameInfo()->getRound().isWinner(environment.getCharacter());
                }

                Value * copy() const {
                    return new Win();
                }
            };

            return new Win();
        }

        if (identifier == "lose"){
            class Lose: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return environment.getStage().getGameInfo()->getRound().isLoser(environment.getCharacter());
                }

                Value * copy() const {
                    return new Lose();
                }
            };

            return new Lose();
        }
        
        if (identifier == "loseko"){
            class LoseKO: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return environment.getStage().getGameInfo()->getRound().isLoserKO(environment.getCharacter());
                }

                Value * copy() const {
                    return new LoseKO();
                }
            };

            return new LoseKO();
        }
        
        if (identifier == "losetime"){
            class LoseTime: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return environment.getStage().getGameInfo()->getRound().isLoserTime(environment.getCharacter());
                }

                Value * copy() const {
                    return new LoseTime();
                }
            };

            return new LoseTime();
        }

        if (identifier == "drawgame"){
            /* FIXME */
            return compile(0);
        }

        if (identifier == "roundstate"){
            class RoundState: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    switch (environment.getStage().getGameInfo()->getRound().getState()){
                        case Round::WaitForIntro : return RuntimeValue(0);
                        case Round::DisplayIntro : return RuntimeValue(1);
                        case Round::WaitForRound : 
                        case Round::DisplayRound :
                        case Round::WaitForFight :
                        case Round::DisplayFight :
                        case Round::PlayingGame : return RuntimeValue(2);
                        case Round::RoundOver : return RuntimeValue(3);
                        case Round::DoTimeOver :
                        case Round::DoWin :
                        case Round::MatchOver : return RuntimeValue(4);
                    }
                    /* FIXME */
                    // return RuntimeValue(2);
                    return RuntimeValue(3);
                }

                virtual std::string toString() const {
                    return "RoundState";
                }

                Value * copy() const {
                    return new RoundState();
                }
            };

            return new RoundState();
        }

        if (identifier == "movecontact"){
            class MoveContact: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    const Character & guy = environment.getCharacter();
                    if (guy.isAttacking()){
                        return guy.getHitState().moveContact;
                    } else {
                        return 0;
                    }
                }
                
                virtual std::string toString() const {
                    return "MoveContact";
                }

                Value * copy() const {
                    return new MoveContact();
                }
            };

            return new MoveContact();
        }
        
        if (identifier == "movereversed"){
            class MoveReversed: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    /* FIXME 
                       This trigger is valid only when the player is in an attack state. 
                       MoveReversed gives a non-zero value if P1's attack has been reversed by P2. It gives 0 otherwise.
                       */
                    return RuntimeValue(0);
                }

                Value * copy() const {
                    return new MoveReversed();
                }
            };

            return new MoveReversed();
        }

        if (identifier == "numtarget"){
            class NumTarget: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    const map<int, vector<Character*> > & targets = environment.getCharacter().getTargets();
                    double count = 0;
                    for (map<int, vector<Character* > >::const_iterator it = targets.begin(); it != targets.end(); it++){
                        const vector<Character*> & objects = it->second;
                        count += objects.size();
                    }
                    return count;
                }

                Value * copy() const {
                    return new NumTarget();
                }
            };

            return new NumTarget();
        }

        if (identifier == "inguarddist"){
            class InGuardDist: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    const Character * enemy = environment.getStage().getEnemy(&environment.getCharacter());

                    bool out = enemy->getMoveType() == Move::Attack && 
                               environment.getCharacter().withinGuardDistance(enemy);

                    return RuntimeValue(out);
                }

                Value * copy() const {
                    return new InGuardDist();
                }
            };

            return new InGuardDist();
        }

        if (identifier == "animtime"){
            class AnimTime: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    PaintownUtil::ReferenceCount<Animation> animation = environment.getCharacter().getCurrentAnimation();
                    if (animation == NULL){
                        std::ostringstream out;
                        out << "No animation for position " << environment.getCharacter().getAnimation() << std::endl;
                        runtimeError(out.str(), __FILE__, __LINE__);
                    }
                    return RuntimeValue(animation->animationTime());
                }

                virtual std::string toString() const {
                    return "AnimTime";
                }

                Value * copy() const {
                    return new AnimTime();
                }
            };

            return new AnimTime();
        }

        if (identifier == "palno"){
            class PalNo: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue((int) environment.getCharacter().getCurrentPalette());
                }

                Value * copy() const {
                    return new PalNo();
                }
            };

            return new PalNo();
        }

        if (identifier == "movehit"){
            class MoveHit: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    /* FIXME */
                    return RuntimeValue(0);
                }

                Value * copy() const {
                    return new MoveHit();
                }
            };

            return new MoveHit();
        }


        if (identifier == "numexplod"){
            class NumExplod: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return (int) environment.getStage().findExplode(0, (&environment.getCharacter())).size();
                }

                Value * copy() const {
                    return new NumExplod();
                }
            };

            return new NumExplod();
        }
        
        if (identifier == "gametime"){
            class GameTime: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getStage().getGameTime());
                }

                Value * copy() const {
                    return new GameTime();
                }
            };

            return new GameTime();
        }

        if (identifier == "hitshakeover"){
            class HitShakeOver: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getHitState().shakeTime <= 0);
                }

                Value * copy() const {
                    return new HitShakeOver();
                }
            };

            return new HitShakeOver();
        }

        if (identifier == "hitover"){
            class HitOver: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getHitState().hitTime <= -1);
                }

                Value * copy() const {
                    return new HitOver();
                }
            };

            return new HitOver();
        }

        if (identifier == "canrecover"){
            class CanRecover: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().canRecover());
                }

                Value * copy() const {
                    return new CanRecover();
                }
            };

            return new CanRecover();
        }

        if (identifier == "hitfall"){
            class HitFall: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    const HitState & state = environment.getCharacter().getHitState();
                    return RuntimeValue(state.fall.fall);
                }

                Value * copy() const {
                    return new HitFall();
                }
            };

            return new HitFall();
        }

        /* the mugen docs don't say anything about `statetime' but its
         * most likely the same thing as `time'
         */
        if (identifier == "time" || identifier == "statetime"){
            class Time: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getStateTime());
                }

                virtual std::string toString() const {
                    return "Time";
                }

                Value * copy() const {
                    return new Time();
                }
            };

            return new Time();
        }

        if (isStateType(PaintownUtil::upperCaseAll(identifier.toString()))){
            return parseStateType(PaintownUtil::upperCaseAll(identifier.toString()));
        }
        
        /* end things that might go away */

        if (identifier == "statetype"){
            class StateType: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    std::string state = environment.getCharacter().getStateType();
                    return convertStateType(state);
                }

                virtual std::string toString() const {
                    return "statetype";
                }

                Value * copy() const {
                    return new StateType();
                }
            };

            return new StateType();
        }

        /* true if the player has control */
        if (identifier == "ctrl"){
            class Ctrl: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().hasControl());
                }

                virtual std::string toString() const {
                    return "ctrl";
                }

                Value * copy() const {
                    return new Ctrl();
                }
            };

            return new Ctrl();
        }

        if (identifier == "HitPauseTime"){
            class HitPauseTime: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().pauseTime());
                }

                Value * copy() const {
                    return new HitPauseTime();
                }
            };

            return new HitPauseTime();
        }

        if (identifier == "MoveGuarded"){
            class MoveGuarded: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    const Character * enemy = environment.getStage().getEnemy(&environment.getCharacter());
                    return RuntimeValue(enemy->isGuarding());
                }
                
                virtual std::string toString() const {
                    return "moveguarded";
                }

                Value * copy() const {
                    return new MoveGuarded();
                }
            };

            return new MoveGuarded();
        }

        /* god I need a drink */

        if (identifier == "stateno"){
            class StateNo: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getCurrentState());
                }

                virtual std::string toString() const {
                    return "stateno";
                }

                Value * copy() const {
                    return new StateNo();
                }
            };

            return new StateNo();
        }

        if (identifier == "life"){
            class Life: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getHealth());
                }

                Value * copy() const {
                    return new Life();
                }
            };

            return new Life();
        }

        if (identifier == "p2life"){
            class P2Life: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    const Character * enemy = environment.getStage().getEnemy(&environment.getCharacter());
                    return RuntimeValue(enemy->getHealth());
                }

                Value * copy() const {
                    return new P2Life();
                }
            };

            return new P2Life();
        }

        if (identifier == "lifemax"){
            class LifeMax: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getMaxHealth());
                }

                Value * copy() const {
                    return new LifeMax();
                }
            };

            return new LifeMax();
        }

        if (identifier == "power"){
            class Power: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getPower());
                }

                virtual std::string toString() const {
                    return "power";
                }

                Value * copy() const {
                    return new Power();
                }
            };

            return new Power();
        }
        
        if (identifier == "powermax"){
            /* TODO Findout if there is a setting somewhere in MUGEN that contains this info */
            return compile(3000);
        }
        
        if (identifier == "internal:extra-jumps"){
            class ExtraJumps: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getExtraJumps());
                }

                virtual std::string toString() const {
                    return "internal:extra-jumps";
                }

                Value * copy() const {
                    return new ExtraJumps();
                }
            };

            return new ExtraJumps();
        }

        if (identifier == "internal:airjump-height"){
            class AirJump: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getAirJumpHeight());
                }

                virtual std::string toString() const {
                    return "internal:airjump-height";
                }

                Value * copy() const {
                    return new AirJump();
                }
            };

            return new AirJump();
        }

        if (identifier == "prevstateno"){
            class PrevStateNo: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getPreviousState());
                }

                virtual std::string toString() const {
                    return "PrevStateNo";
                }

                Value * copy() const {
                    return new PrevStateNo();
                }
            };

            return new PrevStateNo();
        }

        if (identifier == "ishometeam"){
            /* FIXME */
            return compile(0);
        }
        
        if (identifier == "teamside"){
            /* FIXME */
            return compile(1);
        }

        if (identifier == "pi"){
            return compile(PaintownUtil::pi);
        }
        
        if (identifier == "tickspersecond"){
            return compile(Mugen::Data::getInstance().getGameSpeed());
        }

        std::ostringstream out;
        out << "Don't know how to compile identifier '" << identifier.toString() << "' on line " << identifier.getLine() << " at column " << identifier.getColumn();
        compileError(out.str(), __FILE__, __LINE__);
        return NULL;
    }

    Value * compileHelper(const Ast::Helper & helper){
        if (helper == "parent"){
            class Parent: public Value {
            public:
                Parent(Value * argument):
                    argument(argument){
                    }

                Value * argument;

                virtual ~Parent(){
                    delete argument;
                }

                virtual std::string toString() const {
                    std::ostringstream out;
                    out << "parent, " << argument->toString();
                    return out.str();
                }

                Value * copy() const {
                    return new Parent(Compiler::copy(argument));
                }

                RuntimeValue evaluate(const Environment & environment) const {
                    const Character & guy = environment.getCharacter();
                    if (guy.isHelper()){
                        const Mugen::Helper & myhelper = *(const Mugen::Helper*)&guy;
                        Character * parent = myhelper.getParent();
                        if (parent == NULL){
                            runtimeError("Helper has no parent", __FILE__, __LINE__);
                        }
                        FullEnvironment parentEnvironment(environment.getStage(), *parent, environment.getCommands());
                        return argument->evaluate(parentEnvironment);
                    }
                    runtimeError("Cannot redirect to a parent from a non-helper", __FILE__, __LINE__);
                    return RuntimeValue();
                }
            };

            return new Parent(compile(helper.getOriginal()));
        }

        if (helper == "root"){
            class Root: public Value {
            public:
                Root(Value * argument):
                    argument(argument){
                    }

                Value * argument;

                virtual ~Root(){
                    delete argument;
                }

                Value * copy() const {
                    return new Root(Compiler::copy(argument));
                }

                RuntimeValue evaluate(const Environment & environment) const {
                    const Character & guy = environment.getCharacter();
                    const Character * parent = guy.getRoot();
                    if (parent == NULL){
                        runtimeError("object has no parent", __FILE__, __LINE__);
                    }
                    FullEnvironment parentEnvironment(environment.getStage(), *parent, environment.getCommands());
                    return argument->evaluate(parentEnvironment);
                }
            };

            return new Root(compile(helper.getOriginal()));
        }
        
        if (helper == "helper"){
            class Helper: public Value {
            public:
                Helper(Value * argument, Value * original):
                    argument(argument),
                    original(original){
                    }

                Value * argument;
                Value * original;

                virtual ~Helper(){
                    delete argument;
                    delete original;
                }

                Value * copy() const {
                    return new Helper(Compiler::copy(argument),
                                      Compiler::copy(original));
                }

                RuntimeValue evaluate(const Environment & environment) const {
                    vector<Mugen::Helper *> helpers ;
                    if (argument != NULL){
                        int id = argument->evaluate(environment).toNumber();
                        helpers = environment.getStage().findHelpers(&environment.getCharacter(), id);
                    } else {
                        helpers = environment.getStage().findHelpers(&environment.getCharacter());
                    }

                    for (vector<Mugen::Helper*>::iterator it = helpers.begin(); it != helpers.end(); it++){
                        Mugen::Helper * helper = *it;
                        FullEnvironment redirected(environment.getStage(), *helper, environment.getCommands());
                        return original->evaluate(redirected);
                    }
                    runtimeError("No helpers found", __FILE__, __LINE__);
                    return 0;
                }
            };

            Value * maybeArgument = NULL;
            if (helper.getArgument() != NULL){
                maybeArgument = compile(helper.getArgument());
            }
            return new Helper(maybeArgument, compile(helper.getOriginal()));
        }

        /* FIXME: can take an argument */
        if (helper == "target"){
            class Target: public Value {
            public:
                Target(Value * argument):
                    argument(argument){
                    }

                Value * argument;

                virtual ~Target(){
                    delete argument;
                }

                Value * copy() const {
                    return new Target(Compiler::copy(argument));
                }

                RuntimeValue evaluate(const Environment & environment) const {
                    /* FIXME */
                    return RuntimeValue(0);
                }
            };
            
            return new Target(compile(helper.getOriginal()));
        }

        if (helper == "partner"){
            class Partner: public Value {
            public:
                Partner(Value * argument):
                    argument(argument){
                    }

                Value * argument;

                virtual ~Partner(){
                    delete argument;
                }

                Value * copy() const {
                    return new Partner(Compiler::copy(argument));
                }

                RuntimeValue evaluate(const Environment & environment) const {
                    /* FIXME */
                    return RuntimeValue(0);
                }
            };

            return new Partner(compile(helper.getOriginal()));
        }

        /* FIXME: can take an argument */
        if (helper == "enemy"){
            class Enemy: public Value {
            public:
                Enemy(Value * argument):
                    argument(argument){
                    }

                Value * argument;

                virtual ~Enemy(){
                    delete argument;
                }

                Value * copy() const {
                    return new Enemy(Compiler::copy(argument));
                }

                RuntimeValue evaluate(const Environment & environment) const {
                    /* FIXME */
                    return RuntimeValue(0);
                }
            };

            return new Enemy(compile(helper.getOriginal()));
        }

        if (helper == "enemynear"){
            class EnemyNear: public Value {
            public:
                EnemyNear(Value * argument):
                    argument(argument){
                    }

                Value * argument;

                virtual ~EnemyNear(){
                    delete argument;
                }

                Value * copy() const {
                    return new EnemyNear(Compiler::copy(argument));
                }

                RuntimeValue evaluate(const Environment & environment) const {
                    const Character * enemy = environment.getStage().getEnemy(&environment.getCharacter());
                    FullEnvironment redirected(environment.getStage(), *enemy, environment.getCommands());
                    return argument->evaluate(redirected);
                }
            };
            return new EnemyNear(compile(helper.getOriginal()));
        }
        
        /* FIXME: can take an argument */
        if (helper == "playerid"){
            class PlayerID: public Value {
            public:
                PlayerID(Value * argument):
                    argument(argument){
                    }

                Value * argument;

                virtual ~PlayerID(){
                    delete argument;
                }

                Value * copy() const {
                    return new PlayerID(Compiler::copy(argument));
                }

                RuntimeValue evaluate(const Environment & environment) const {
                    /* FIXME */
                    return RuntimeValue(0);
                }
            };

            return new PlayerID(compile(helper.getOriginal()));
        }

        std::ostringstream out;
        out << "Don't know how to compile helper '" << helper.toString() << "'";
        compileError(out.str(), __FILE__, __LINE__);
        return NULL;
    }
    
    virtual void onHelper(const Ast::Helper & helper){
        compiled = compileHelper(helper);
    }
    
    virtual void onIdentifier(const Ast::Identifier & identifier){
        compiled = compileIdentifier(identifier);
    }

    Value * compileRange(const Ast::Range & range){
        class Range: public Value {
        public:
            Range(Value * const low, Value * const high, const Ast::Range::RangeType type):
                low(low),
                high(high),
                type(type){
                }

            Value * const low;
            Value * const high;
            Ast::Range::RangeType type;
            
            virtual ~Range(){
                delete low;
                delete high;
            }

            Value * copy() const {
                return new Range(Compiler::copy(low), Compiler::copy(high), type);
            }

            RuntimeValue evaluate(const Environment & environment) const {
                int low = (int) this->low->evaluate(environment).toNumber();
                int high = (int) this->high->evaluate(environment).toNumber();
                switch (type){
                    case Ast::Range::AllInclusive : return RuntimeValue(low - 1, high + 1);
                    case Ast::Range::AllExclusive : return RuntimeValue(low, high);
                    case Ast::Range::LeftInclusiveRightExclusive : return RuntimeValue(low - 1, high);
                    case Ast::Range::LeftExclusiveRightInclusive : return RuntimeValue(low, high + 1);
                    default: compileError("Can't get here", __FILE__, __LINE__); return RuntimeValue();
                }
            }

            virtual std::string toString() const {
                std::ostringstream out;
                switch (type){
                    case Ast::Range::AllInclusive: out << "[" << low->toString() << ", " << high->toString() << "]"; break;
                    case Ast::Range::AllExclusive: out << "(" << low->toString() << ", " << high->toString() << ")"; break;
                    case Ast::Range::LeftInclusiveRightExclusive: out << "[" << low->toString() << ", " << high->toString() << ")"; break;
                    case Ast::Range::LeftExclusiveRightInclusive: out << "(" << low->toString() << ", " << high->toString() << "]"; break;
                }
                return out.str();
            }
        };

        switch (range.getRangeType()){
            case Ast::Range::AllInclusive :
            case Ast::Range::AllExclusive :
            case Ast::Range::LeftInclusiveRightExclusive :
            case Ast::Range::LeftExclusiveRightInclusive : {
                break;
            }
            default: compileError("Unexpected range type", __FILE__, __LINE__);
        }

        Value * const  low = compile(range.getLow());
        Value * const high = compile(range.getHigh());
        Ast::Range::RangeType type = range.getRangeType();
        return new Range(low, high, type);
    }
    
    virtual void onRange(const Ast::Range & range){
        compiled = compileRange(range);
    }

    Value * compileString(const Ast::String & string_value){
        class JustString: public Value {
        public:
            JustString(const std::string & what):
                value(what){
                }

            JustString(const JustString & you):
            value(you.value){
            }

            Value * copy() const {
                return new JustString(*this);
            }

            virtual std::string toString() const {
                return value.getStringValue();
            }

            RuntimeValue value;

            RuntimeValue evaluate(const Environment & environment) const {
                return value;
            }
        };

        std::string out;
        string_value.view() >> out;
        return new JustString(out);
    }

    virtual void onString(const Ast::String & string){
        compiled = compileString(string);
    }

    static Value * compileNumber(const Ast::Number & number){
        class JustNumber: public Value {
        public:
            JustNumber(double what):
                value(what){
                }

            JustNumber(const JustNumber & you):
            value(you.value){
            }

            virtual std::string toString() const {
                std::ostringstream out;
                out << value.toNumber();
                return out.str();
            }

            Value * copy() const {
                return new JustNumber(*this);
            }

            RuntimeValue value;

            RuntimeValue evaluate(const Environment & environment) const {
                return value;
            }
        };

        double x;
        number.view() >> x;
        return new JustNumber(x);
    }

    virtual void onNumber(const Ast::Number & number){
        compiled = compileNumber(number);
    }

    static Value * compileArgument(const Ast::Argument & argument){
        class Argument: public Value {
        public:
            Argument(int index):
            index(index){
                if (index > 0){
                    std::stringstream out;
                    out << "Indexes above 0 are not valid";
                    compileError(out.str(), __FILE__, __LINE__);
                }
            }

            Argument(const Argument & you):
            index(you.index){
            }

            virtual std::string toString() const {
                std::ostringstream out;
                out << index;
                return out.str();
            }

            Value * copy() const {
                return new Argument(*this);
            }

            int index;

            RuntimeValue evaluate(const Environment & environment) const {
                switch (index){
                    case 0: return environment.getArg1();
                    default: runtimeError("BUG", __FILE__, __LINE__);
                }
                return RuntimeValue(0);
            }
        };

        int index;
        argument.view() >> index;
        return new Argument(index);
    }

    virtual void onArgument(const Ast::Argument & argument){
        compiled = compileArgument(argument);
    }

    Value * compileFunction(const Ast::Function & function){
        /* For use when the function accepts one argument and passes it along
         * to some C level function.
         */
        class MetaCircularArg1: public Value {
        public:
            MetaCircularArg1(const std::string & name, double (*c_level)(double), Value * argument):
                name(name),
                cFunction(c_level),
                argument(argument){
                }

            std::string name;
            double (*cFunction)(double);
            Value * argument;

            virtual ~MetaCircularArg1(){
                delete argument;
            }

            virtual std::string toString() const {
                std::ostringstream out;
                out << name << "(" << argument->toString() << ")";
                return out.str();
            }

            Value * copy() const {
                return new MetaCircularArg1(name, cFunction, Compiler::copy(argument));
            }

            RuntimeValue evaluate(const Environment & environment) const {
                double result = argument->evaluate(environment).toNumber();
                return RuntimeValue(cFunction(result));
            }
        };

        if (function == "const"){
            class ConstWalker: public Ast::Walker {
            public:
                ConstWalker():
                compiled(NULL){
                }

                Value * compiled;

                Value * compileConst(const Ast::Identifier & identifier){
                    if (identifier == "data.life"){
                        return getCharacterField(&Character::getMaxHealth, identifier.toLowerString());
                    }
                    
                    if (identifier == "data.power"){
                        return getCharacterField(&Character::getPower, identifier.toLowerString());
                    }

                    if (identifier == "data.liedown.time"){
                        /* FIXME */
                        return compile(0);
                    }
                    
                    if (identifier == "data.defence"){
                        return getCharacterField(&Character::getDefense, identifier.toLowerString());
                    }

                    if (identifier == "data.attack"){
                        return getCharacterField(&Character::getAttack, identifier.toLowerString());
                    }

                    if (identifier == "data.fall.defence_mul"){
                        class DefenseMul: public Value {
                        public:
                            RuntimeValue evaluate(const Environment & environment) const {
                                return RuntimeValue(100 / (100.0 + environment.getCharacter().getFallDefenseUp()));
                            }

                            Value * copy() const {
                                return new DefenseMul();
                            }
                        };

                        return new DefenseMul();
                    }

                    if (identifier == "data.airjuggle"){
                        return getCharacterField(&Character::getJugglePoints, identifier.toLowerString());
                    }

                    if (identifier == "data.sparkno"){
                        class SparkNo: public Value {
                        public:
                            RuntimeValue evaluate(const Environment & environment) const {
                                /* FIXME: what if the effect has 'own' set? */
                                return environment.getCharacter().getDefaultSpark().group;
                            }

                            Value * copy() const {
                                return new SparkNo();
                            }
                        };

                        return new SparkNo();
                    }

                    if (identifier == "data.guard.sparkno"){
                        class GuardSparkNo: public Value {
                        public:
                            RuntimeValue evaluate(const Environment & environment) const {
                                /* FIXME: what if the effect has 'own' set? */
                                return environment.getCharacter().getDefaultGuardSpark().group;
                            }

                            Value * copy() const {
                                return new GuardSparkNo();
                            }
                        };

                        return new GuardSparkNo();
                    }

                    if (identifier == "data.ko.echo"){
                        return getCharacterField(&Character::getKoEcho, identifier.toLowerString());
                    }

                    if (identifier == "data.intpersistindex"){
                        return getCharacterField(&Character::getIntPersistIndex, identifier.toLowerString());
                    }

                    if (identifier == "data.floatpersistindex"){
                        return getCharacterField(&Character::getFloatPersistIndex, identifier.toLowerString());
                    }

                    if (identifier == "movement.airjump.num"){
                        return getCharacterField(&Character::getExtraJumps, identifier.toLowerString());
                    }

                    if (identifier == "movement.airjump.height"){
                        return getCharacterField(&Character::getAirJumpHeight, identifier.toLowerString());
                    }

                    if (identifier == "movement.yaccel"){
                        return getCharacterField(&Character::getGravity, identifier.toLowerString());
                    }
                    
                    if (identifier == "movement.crouch.friction"){
                        return getCharacterField(&Character::getCrouchingFriction, identifier.toLowerString());
                    }

                    if (identifier == "movement.crouch.friction.threshold"){
                        return getCharacterField(&Character::getCrouchingFrictionThreshold, identifier.toLowerString());
                    }

                    if (identifier == "movement.stand.friction"){
                        return getCharacterField(&Character::getStandingFriction, identifier.toLowerString());
                    }

                    if (identifier == "movement.stand.friction.threshold"){
                        return getCharacterField(&Character::getStandingFrictionThreshold, identifier.toLowerString());
                    }

                    if (identifier == "movement.jump.changeanim.threshold"){
                        return getCharacterField(&Character::getJumpChangeAnimationThreshold, identifier.toLowerString());
                    }

                    if (identifier == "movement.air.gethit.groundlevel"){
                        return getCharacterField(&Character::getAirGetHitGroundLevel, identifier.toLowerString());
                    }

                    if (identifier == "velocity.walk.back.x"){
                        return getCharacterField(&Character::getWalkBackX, identifier.toLowerString());
                    }

                    if (identifier == "velocity.walk.fwd.x"){
                        return getCharacterField(&Character::getWalkForwardX, identifier.toLowerString());
                    }

                    if (identifier == "velocity.run.fwd.x"){
                        return getCharacterField(&Character::getRunForwardX, identifier.toLowerString());
                    }

                    if (identifier == "velocity.run.fwd.y"){
                        return getCharacterField(&Character::getRunForwardY, identifier.toLowerString());
                    }

                    if (identifier == "velocity.jump.neu.x"){
                        return getCharacterField(&Character::getNeutralJumpingX, identifier.toLowerString());
                    }

                    if (identifier == "velocity.jump.y" ||
                        /* HACK: mugen docs don't say runjump.y is the same as
                         * jump.y, but some characters use it anyway (Gouki)
                         */
                        identifier == "velocity.runjump.y"){
                        return getCharacterField(&Character::getNeutralJumpingY, identifier.toLowerString());
                    }

                    if (identifier == "velocity.runjump.back.x"){
                        return getCharacterField(&Character::getRunJumpBack, identifier.toLowerString());
                    }

                    if (identifier == "velocity.run.back.x"){
                        return getCharacterField(&Character::getRunBackX, identifier.toLowerString());
                    }

                    if (identifier == "velocity.run.back.y"){
                        return getCharacterField(&Character::getRunBackY, identifier.toLowerString());
                    }

                    if (identifier == "velocity.jump.back.x"){
                        return getCharacterField(&Character::getJumpBack, identifier.toLowerString());
                    }

                    if (identifier == "velocity.jump.fwd.x"){
                        return getCharacterField(&Character::getJumpForward, identifier.toLowerString());
                    }

                    if (identifier == "velocity.runjump.fwd.x"){
                        return getCharacterField(&Character::getRunJumpForward, identifier.toLowerString());
                    }

                    /* FIXME others
                       movement.air.gethit.groundrecover.ground.threshold: Returns value of the "air.gethit.groundrecover.ground.threshold" parameter. (float)
                       movement.air.gethit.groundrecover.groundlevel: Returns value of the "air.gethit.groundrecover.groundlevel" parameter. (float)
                       movement.air.gethit.airrecover.threshold: Returns value of the "air.gethit.airrecover.threshold" parameter. (float)
                       movement.air.gethit.airrecover.yaccel: Returns value of the "air.gethit.airrecover.yaccel" parameter. (float)
                       movement.air.gethit.trip.groundlevel: Returns value of the "air.gethit.trip.groundlevel" parameter. (float)
                       movement.down.bounce.offset.x: Returns x-component of the "down.bounce.offset.x" parameter. (float)
                       movement.down.bounce.offset.y: Returns y-component of the "down.bounce.offset.y" parameter. (float)
                       movement.down.bounce.yaccel: Returns value of the "down.bounce.yaccel" parameter. (float)
                       movement.down.bounce.groundlevel: Returns value of the "down.bounce.groundlevel" parameter. (float)
                       movement.down.friction.threshold: Returns value of the "down.friction.threshold" parameter. (float)
                       */
                    if (identifier == "velocity.air.gethit.airrecover.mul.x"){
                        return getCharacterField(&Character::getAirHitRecoverMultiplierX, identifier.toLowerString());
                    }

                    if (identifier == "velocity.air.gethit.airrecover.mul.y"){
                        return getCharacterField(&Character::getAirHitRecoverMultiplierY, identifier.toLowerString());
                    }

                    if (identifier == "velocity.air.gethit.groundrecover.x"){
                        return getCharacterField(&Character::getAirHitGroundRecoverX, identifier.toLowerString());
                    }
                    
                    if (identifier == "velocity.air.gethit.groundrecover.y"){
                        return getCharacterField(&Character::getAirHitGroundRecoverY, identifier.toLowerString());
                    }

                    if (identifier == "velocity.air.gethit.airrecover.add.x"){
                        return getCharacterField(&Character::getAirHitRecoverAddX, identifier.toLowerString());
                    }
                    
                    if (identifier == "velocity.air.gethit.airrecover.add.y"){
                        return getCharacterField(&Character::getAirHitRecoverAddY, identifier.toLowerString());
                    }

                    /*
                     * velocity.air.gethit.airrecover.back: Returns value of the "air.gethit.airrecover.back" parameter. (float)
                     * velocity.air.gethit.airrecover.fwd: Returns value of the "air.gethit.airrecover.fwd" parameter. (float)
                     * velocity.air.gethit.airrecover.up: Returns value of the "air.gethit.airrecover.up" parameter. (float)
                     * velocity.air.gethit.airrecover.down: Returns value of the "air.gethit.airrecover.down" parameter. (float)
                     */

                    if (identifier == "velocity.airjump.neu.x"){
                        return getCharacterField(&Character::getAirJumpNeutralX, identifier.toLowerString());
                    }

                    if (identifier == "velocity.airjump.y"){
                        return getCharacterField(&Character::getAirJumpNeutralY, identifier.toLowerString());
                    }

                    if (identifier == "velocity.airjump.back.x"){
                        return getCharacterField(&Character::getAirJumpBack, identifier.toLowerString());
                    }

                    if (identifier == "velocity.airjump.fwd.x"){
                        return getCharacterField(&Character::getAirJumpForward, identifier.toLowerString());
                    }
                    
                    if (identifier == "size.xscale"){
                        return getCharacterField(&Character::getXScale, identifier.toLowerString());
                    }
                    
                    if (identifier == "size.yscale"){
                        return getCharacterField(&Character::getYScale, identifier.toLowerString());
                    }
                    
                    if (identifier == "size.ground.back"){
                        /* TODO */
                        return compile(0);
                    }

                    if (identifier == "size.ground.front"){
                        /* TODO */
                        return compile(0);
                    }

                    if (identifier == "size.air.back"){
                        /* TODO */
                        return compile(0);
                    }

                    if (identifier == "size.air.front"){
                        /* TODO */
                        return compile(0);
                    }

                    if (identifier == "size.height"){
                        /* TODO */
                        return compile(0);
                    }

                    if (identifier == "size.attack.dist"){
                        /* TODO */
                        return compile(0);
                    }

                    if (identifier == "size.proj.attack.dist"){
                        /* TODO */
                        return compile(0);
                    }

                    if (identifier == "size.proj.doscale"){
                        /* TODO */
                        return compile(0);
                    }

                    if (identifier == "size.head.pos.x"){
                        /* TODO */
                        return compile(0);
                    }

                    if (identifier == "size.head.pos.y"){
                        /* TODO */
                        return compile(0);
                    }

                    if (identifier == "size.mid.pos.x"){
                        /* TODO */
                        return compile(0);
                    }
                    
                    if (identifier == "size.mid.pos.y"){
                        /* TODO */
                        return compile(0);
                    }
                    
                    if (identifier == "size.shadowoffset"){
                        /* TODO */
                        return compile(0);
                    }

                    if (identifier == "size.draw.offset.x"){
                        /* TODO */
                        return compile(0);
                    }

                    if (identifier == "size.draw.offset.y"){
                        /* TODO */
                        return compile(0);
                    }

                    return NULL;
                }

                virtual void onIdentifier(const Ast::Identifier & identifier){
                    compiled = compileConst(identifier);
                }
            };
            ConstWalker walker;
            function.getArg1()->walk(walker);
            if (walker.compiled == NULL){
                compileError("Unknown const value " + function.getArg1()->toString(), __FILE__, __LINE__);
            }
            return walker.compiled;
        }

        if (function == "asin"){
            return new MetaCircularArg1("asin", asin, compile(function.getArg1()));
        }
        
        if (function == "sin"){
            return new MetaCircularArg1("sin", sin, compile(function.getArg1()));
        }
        
        if (function == "atan"){
            return new MetaCircularArg1("atan", atan, compile(function.getArg1()));
        }
        
        if (function == "tan"){
            return new MetaCircularArg1("tan", tan, compile(function.getArg1()));
        }

        if (function == "abs"){
            return new MetaCircularArg1("abs", fabs, compile(function.getArg1()));
        }
        
        if (function == "exp"){
            return new MetaCircularArg1("exp", exp, compile(function.getArg1()));
        }

        /*
        if (PaintownUtil::matchRegex(PaintownUtil::lowerCaseAll(function.getName()), "projcontact\\d*")){
            return compile(0);
        }
        */

        if (function == "playeridexist"){
            class PlayerIdExist: public Value {
            public:
                PlayerIdExist(Value * id):
                id(id){
                }

                Value * id;

                virtual ~PlayerIdExist(){
                    delete id;
                }
              
                RuntimeValue evaluate(const Environment & environment) const {
                    int id = (int) this->id->evaluate(environment).toNumber();
                    return environment.getStage().findPlayerById(id) != NULL;
                }

                Value * copy() const {
                    return new PlayerIdExist(Compiler::copy(id));
                }
            };

            return new PlayerIdExist(compile(function.getArg1()));
        }

        if (function == "projhit"){
            class ProjHit: public Value {
            public:
                ProjHit(Value * id, Value * value, Value * compare):
                id(id), value(value), compare(compare){
                }

                Value * id;
                Value * value;
                Value * compare;

                virtual ~ProjHit(){
                    delete id;
                    delete value;
                    delete compare;
                }
                
                /* It hit is true then we are putting the last hit in some relation (compare) and
                 * getting the result, if hit is false then we are taking the negation of
                 * that result.
                 */
                bool didHit(const Environment & environment, Projectile * projectile, bool hit) const {
                    unsigned long ticks = environment.getStage().getTicks() - projectile->getLastHitTicks();

                    /* The first argument of the comparison expression should be a compiled
                     * Ast::Argument which expects to pull out arg1 from the environment. Here
                     * we set up arg1 to be the difference between the current ticks and the
                     * last time the projectile hit.
                     *
                     * We know the Ast::Argument expects arg1 because the parser sets it up
                     * that way.
                     */

                    FullEnvironment use(environment.getStage(), environment.getCharacter(), environment.getCommands(), RuntimeValue((int) ticks));
                    bool result = compare->evaluate(use).toBool();

                    if (hit){
                        return result;
                    } else {
                        return ! result;
                    }
                }

                /* 
                 * (define (projhit id hit? compare)
                 *   (if hit?
                 *     (compare (now - last))
                 *     (not (compare (now - last)))))
                 * compare should be (lambda (x) (x compare value)) where compare and value are
                 *   projhit = 1, compare value
                 *
                 * Environment should have an `arg1' field which is a Runtime value.
                 * The compare function should be created with an Ast::Argument* type
                 * that knows to pull out arg1 from the environment. arg1 will be set
                 * to (now - last) when this function is invoked.
                 */
                RuntimeValue evaluate(const Environment & environment) const {
                    int id = (int) this->id->evaluate(environment).toNumber();
                    bool hit = (int) this->value->evaluate(environment).toBool();
                    

                    /* projhit = 1
                     *   check if the projectile hit something within the last tick
                     * projhit = 0
                     *   check if the projectile didn't hit something within the last tick
                     *
                     * ticks: 20
                     * last hit: 10
                     * projhit = 0, < 20
                     *   false
                     * projhit = 0, < 5
                     *   true
                     * projhit = 0, = 10
                     *   false
                     * projhit = 0, = 9 ; this is a strange case, it asks whether the last time
                     *                  ; we didn't hit is exactly equal to 9 ticks.
                     *                  ; This is strange because we didn't at hit 0, 1, 2, 3, .. 9
                     *                  ; The last time we did hit was at 10, so all other numbers
                     *                  ; should register as a didn't hit.
                     *                  ; Ultimately this is evaluating the 1st order statement:
                     *                  ;  F -> F
                     *                  ; Which is T
                     *  true
                     *
                     * projhit = 1, < 20
                     *   true
                     * projhit = 1, < 5
                     *   false
                     * projhit = 1, = 10
                     *   true
                     * projhit = 1, = 9
                     *   false
                     *
                     * I'm pretty sure its the case that the result of the comparison operation
                     * is opposite between projhit = 0 and projhit = 1.
                     */

                    vector<Projectile*> projectiles = environment.getStage().findProjectile(id, &environment.getCharacter());
                    bool found = false;
                    for (vector<Projectile*>::iterator it = projectiles.begin(); it != projectiles.end(); it++){
                        Projectile * projectile = *it;
                        /* For projectiles that never hit their lastHitTicks will be 0
                         * so in theory there is an *ultra* small chance that this code
                         * could run before the projectile hits anything and the stage
                         * ticks is still really low such that this returns true. In practice
                         * the characters will do their starting animation for a while
                         * so a projectile won't really be fired until a few hundred ticks later.
                         */
                        found = found || didHit(environment, projectile, hit);
                    }
                    return RuntimeValue(found);
                }

                Value * copy() const {
                    return new ProjHit(Compiler::copy(id),
                                       Compiler::copy(value),
                                       Compiler::copy(compare));
                }
            };

            return new ProjHit(compile(function.getArg1()),
                               compile(function.getArg2()),
                               compile(function.getArg3()));
        }

        if (function == "projguarded"){
            /* Very similar to ProjHit */
            class ProjGuarded: public Value {
            public:
                ProjGuarded(Value * id, Value * value, Value * compare):
                id(id), value(value), compare(compare){
                }

                Value * id;
                Value * value;
                Value * compare;

                virtual ~ProjGuarded(){
                    delete id;
                    delete value;
                    delete compare;
                }

                Value * copy() const {
                    return new ProjGuarded(Compiler::copy(id),
                                           Compiler::copy(value),
                                           Compiler::copy(compare));
                }

                bool didGuard(const Environment & environment, Projectile * projectile, bool hit) const {
                    unsigned long ticks = environment.getStage().getTicks() - projectile->getLastGuardTicks();

                    FullEnvironment use(environment.getStage(), environment.getCharacter(), environment.getCommands(), RuntimeValue((int) ticks));
                    bool result = compare->evaluate(use).toBool();

                    if (hit){
                        return result;
                    } else {
                        return ! result;
                    }
                }

                RuntimeValue evaluate(const Environment & environment) const {
                    int id = (int) this->id->evaluate(environment).toNumber();
                    bool hit = (int) this->value->evaluate(environment).toBool();
                    
                    vector<Projectile*> projectiles = environment.getStage().findProjectile(id, &environment.getCharacter());
                    bool found = false;
                    for (vector<Projectile*>::iterator it = projectiles.begin(); it != projectiles.end(); it++){
                        Projectile * projectile = *it;
                        found = found || didGuard(environment, projectile, hit);
                    }
                    return RuntimeValue(found);
                }

            };
            
            return new ProjGuarded(compile(function.getArg1()),
                                   compile(function.getArg2()),
                                   compile(function.getArg3()));
        }

        if (function == "projcontact"){
            class ProjContact: public Value {
            public:
                ProjContact(Value * id, Value * value, Value * compare):
                id(id), value(value), compare(compare){
                }

                Value * id;
                Value * value;
                Value * compare;

                virtual ~ProjContact(){
                    delete id;
                    delete value;
                    delete compare;
                }

                bool didHit(const Environment & environment, Projectile * projectile, bool hit) const {
                    unsigned long ticks = environment.getStage().getTicks() - projectile->getLastContactTicks();

                    FullEnvironment use(environment.getStage(), environment.getCharacter(), environment.getCommands(), RuntimeValue((int) ticks));
                    bool result = compare->evaluate(use).toBool();

                    if (hit){
                        return result;
                    } else {
                        return ! result;
                    }
                }

                Value * copy() const {
                    return new ProjContact(Compiler::copy(id),
                                           Compiler::copy(value),
                                           Compiler::copy(compare));
                }

                RuntimeValue evaluate(const Environment & environment) const {
                    int id = (int) this->id->evaluate(environment).toNumber();
                    bool hit = (int) this->value->evaluate(environment).toBool();
                    
                    vector<Projectile*> projectiles = environment.getStage().findProjectile(id, &environment.getCharacter());
                    bool found = false;
                    for (vector<Projectile*>::iterator it = projectiles.begin(); it != projectiles.end(); it++){
                        Projectile * projectile = *it;
                        found = found || didHit(environment, projectile, hit);
                    }
                    return RuntimeValue(found);

                }
            };
            
            return new ProjContact(compile(function.getArg1()),
                                   compile(function.getArg2()),
                                   compile(function.getArg3()));
        }

        if (function == "ln"){
            class Ln: public Value {
            public: 
                Ln(Value * argument):
                    argument(argument){
                    }

                Value * argument;

                virtual ~Ln(){
                    delete argument;
                }

                Value * copy() const {
                    return new Ln(Compiler::copy(argument));
                }

                RuntimeValue evaluate(const Environment & environment) const {
                    const double num = argument->evaluate(environment).toNumber();

                    if (num <= 0){
                        std::ostringstream out;
                        out << "Argument to ln must be positive but was " << num;
                        runtimeError(out.str(), __FILE__, __LINE__);
                    }

                    const double value = log(num);

                    return RuntimeValue(value);
                }
            };
            return new Ln(compile(function.getArg1()));
        }

        if (function == "log"){
            class Log : public Value {
            public: 
                Log(Value * arg1, Value * arg2):
                    arg1(arg1),
                    arg2(arg2){
                    }

                Value * arg1;
                Value * arg2;

                virtual ~Log(){
                    delete arg1;
                    delete arg2;
                }

                Value * copy() const {
                    return new Log(Compiler::copy(arg1), Compiler::copy(arg2));
                }

                RuntimeValue evaluate(const Environment & environment) const {
                    const double base = arg1->evaluate(environment).toNumber();
                    const double value = arg2->evaluate(environment).toNumber();
                    if (base <= 0){
                        std::ostringstream out;
                        out << "Base of log must be positive but was " << base;
                        runtimeError(out.str(), __FILE__, __LINE__);
                    } else if (value <= 0){
                        std::ostringstream out;
                        out << "Value of log must be positive but was " << value;
                        runtimeError(out.str(), __FILE__, __LINE__);
                    }

                    const double result = log(value) / log(base);

                    return RuntimeValue(result);
                }
            };

            return new Log(compile(function.getArg1()),compile(function.getArg2()));
        }

        if (function == "numprojid"){
            class NumProjId: public Value {
            public:
                NumProjId(Value * id):
                id(id){
                }

                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue((int) environment.getStage().findProjectile((int) id->evaluate(environment).toNumber(), &environment.getCharacter()).size());
                }

                virtual ~NumProjId(){
                    delete id;
                }

                Value * id;

                Value * copy() const {
                    return new NumProjId(Compiler::copy(id));
                }
            };

            return new NumProjId(compile(function.getArg1()));
        }

        if (function == "projhittime"){
            class ProjHitTime: public Value {
            public: 
                ProjHitTime(Value * arg1):
                    arg1(arg1){
                    }

                Value * arg1;

                virtual ~ProjHitTime(){
                    delete arg1;
                }

                Value * copy() const {
                    return new ProjHitTime(Compiler::copy(arg1));
                }

                /* Basically this will return -1 if no projectiles hit yet. Then we
                 * just need to handle cases when there is 1 projectile to check
                 * and when there are multiple.
                 * If there are multiple then return a vector of ints.
                 */
                RuntimeValue projHit(const Stage & stage, const vector<Projectile*> projectiles) const {
                    if (projectiles.size() == 0){
                        return RuntimeValue(-1);
                    } else if (projectiles.size() == 1){
                        Projectile * projectile = projectiles[0];
                        if (projectile->getLastHitTicks() > 0){
                            return RuntimeValue((int) (stage.getTicks() - projectile->getLastHitTicks()));
                        } else {
                            return RuntimeValue(-1);
                        }
                    } else {
                        vector<int> out;
                        for (vector<Projectile*>::const_iterator it = projectiles.begin(); it != projectiles.end(); it++){
                            Projectile * projectile = *it;
                            if (projectile->getLastHitTicks() > 0){
                                out.push_back(stage.getTicks() - projectile->getLastHitTicks());
                            }
                        }

                        /* Nothing hit, -1 */
                        if (out.size() == 0){
                            return RuntimeValue(-1);
                        /* One thing hit, just return that */
                        } else if (out.size() == 1){
                            return RuntimeValue(out[0]);
                        } else {
                            /* Multiple things thit, return a vector */
                            return RuntimeValue(out);
                        }
                    }
                }

                RuntimeValue evaluate(const Environment & environment) const {
                    int id = (int) arg1->evaluate(environment).toNumber();
                    if (id <= 0){
                        return projHit(environment.getStage(), environment.getStage().findProjectile(0, &environment.getCharacter()));
                    } else {
                        return projHit(environment.getStage(), environment.getStage().findProjectile(id, &environment.getCharacter()));
                    }
                }
            };

            return new ProjHitTime(compile(function.getArg1()));
        }

        if (function == "projhit"){
            /* FIXME */
            return compile(0);
        }

        if (function == "projcontacttime"){
            /* FIXME */
            return compile(-1);
        }
        
        if (function == "projcanceltime"){
            class ProjCancelTime: public Value {
            public: 
                ProjCancelTime(Value * arg1):
                    arg1(arg1){
                    }

                Value * arg1;

                virtual ~ProjCancelTime(){
                    delete arg1;
                }

                Value * copy() const {
                    return new ProjCancelTime(Compiler::copy(arg1));
                }

                /* Basically this will return -1 if no projectiles hit yet. Then we
                 * just need to handle cases when there is 1 projectile to check
                 * and when there are multiple.
                 * If there are multiple then return a vector of ints.
                 */
                RuntimeValue projCancel(const Stage & stage, const vector<Projectile*> projectiles) const {
                    if (projectiles.size() == 0){
                        return RuntimeValue(-1);
                    } else if (projectiles.size() == 1){
                        Projectile * projectile = projectiles[0];
                        if (projectile->getLastCancelTicks() > 0){
                            return RuntimeValue((int) (stage.getTicks() - projectile->getLastCancelTicks()));
                        } else {
                            return RuntimeValue(-1);
                        }
                    } else {
                        vector<int> out;
                        for (vector<Projectile*>::const_iterator it = projectiles.begin(); it != projectiles.end(); it++){
                            Projectile * projectile = *it;
                            if (projectile->getLastCancelTicks() > 0){
                                out.push_back(stage.getTicks() - projectile->getLastCancelTicks());
                            }
                        }

                        /* Nothing hit, -1 */
                        if (out.size() == 0){
                            return RuntimeValue(-1);
                        /* One thing hit, just return that */
                        } else if (out.size() == 1){
                            return RuntimeValue(out[0]);
                        } else {
                            /* Multiple things thit, return a vector */
                            return RuntimeValue(out);
                        }
                    }
                }

                RuntimeValue evaluate(const Environment & environment) const {
                    int id = (int) arg1->evaluate(environment).toNumber();
                    if (id <= 0){
                        return projCancel(environment.getStage(), environment.getStage().findProjectile(0, &environment.getCharacter()));
                    } else {
                        return projCancel(environment.getStage(), environment.getStage().findProjectile(id, &environment.getCharacter()));
                    }
                }
            };

            return new ProjCancelTime(compile(function.getArg1()));
        }
       
        if (function == "projguardedtime"){
            /* FIXME */
            return compile(-1);
        }

        if (function == "ishelper"){
            /* FIXME */
            return compile(0);
        }

        if (function == "numhelper"){
            class NumHelper: public Value {
            public:
                NumHelper(Value * argument):
                    argument(argument){
                    }

                Value * argument;

                virtual ~NumHelper(){
                    delete argument;
                }

                virtual std::string toString() const {
                    std::ostringstream out;
                    out << "numhelper(" << argument->toString() << ")";
                    return out.str();
                }

                Value * copy() const {
                    return new NumHelper(Compiler::copy(argument));
                }

                RuntimeValue evaluate(const Environment & environment) const {
                    std::vector<Mugen::Helper*> helpers = environment.getStage().findHelpers(&environment.getCharacter(), (int) argument->evaluate(environment).toNumber());
                    return RuntimeValue((int) helpers.size());
                }
            };

            return new NumHelper(compile(function.getArg1()));
        }

        if (function == "floor"){
            class FunctionFloor: public Value {
            public:
                FunctionFloor(Value * argument):
                    argument(argument){
                    }

                Value * argument;

                virtual ~FunctionFloor(){
                    delete argument;
                }

                Value * copy() const {
                    return new FunctionFloor(Compiler::copy(argument));
                }

                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue((int) argument->evaluate(environment).toNumber());
                }
            };

            return new FunctionFloor(compile(function.getArg1()));
        }

        if (function == "ifelse"){
            class FunctionIfElse: public Value{
            public:
                FunctionIfElse(Value * condition, Value * then, Value * else_):
                condition(condition),
                then(then),
                else_(else_){
                }

                Value * condition;
                Value * then;
                Value * else_;

                virtual ~FunctionIfElse(){
                    delete condition;
                    delete then;
                    delete else_;
                }

                Value * copy() const {
                    return new FunctionIfElse(Compiler::copy(condition),
                                              Compiler::copy(then),
                                              Compiler::copy(else_));
                }

                RuntimeValue evaluate(const Environment & environment) const {
                    if (condition->evaluate(environment).toBool()){
                        return then->evaluate(environment);
                    } else {
                        return else_->evaluate(environment);
                    }
                }
            };

            return new FunctionIfElse(compile(function.getArg1()),
                                      compile(function.getArg2()),
                                      compile(function.getArg3()));
        }

        if (function == "gethitvar"){
            class HitVar: public Value {
            public:
                HitVar(){
                }

                virtual const HitState & state(const Environment & environment) const {
                    return environment.getCharacter().getHitState();
                }
            };

            if (function.getArg1() == 0){
                compileError("No argument given to gethitvar", __FILE__, __LINE__);
            }

            std::string var = PaintownUtil::lowerCaseAll(function.getArg1()->toString());
            if (var == "xveladd"){
                /* TODO: I am unsure xveladd and yveladd are right here. As far as
                 * I can tell these values are computed by the engine. JMugen uses
                 * the x/y velocities from the hit state, so I am doing the same for now.
                 */
                class HitVarXVelAdd: public HitVar {
                public:
                    RuntimeValue evaluate(const Environment & environment) const {
                        return RuntimeValue(-state(environment).xVelocity);
                    }

                    Value * copy() const {
                        return new HitVarXVelAdd();
                    }
                };

                return new HitVarXVelAdd();
            } else if (var == "yveladd"){
                class HitVarYVelAdd: public HitVar {
                public:
                    RuntimeValue evaluate(const Environment & environment) const {
                        return RuntimeValue(state(environment).yVelocity);
                    }

                    Value * copy() const {
                        return new HitVarYVelAdd();
                    }
                };

                return new HitVarYVelAdd();
            } else if (var == "type"){
                class HitVarType: public HitVar {
                public:
                    RuntimeValue evaluate(const Environment & environment) const {
                        if (state(environment).hitType == AttackType::None){
                            return RuntimeValue(0);
                        } else if (state(environment).hitType == AttackType::High){
                            return RuntimeValue(1);
                        } else if (state(environment).hitType == AttackType::Low){
                            return RuntimeValue(2);
                        } else if (state(environment).hitType == AttackType::Trip){
                            return RuntimeValue(3);
                        }
                        compileError("Invalid hit type", __FILE__, __LINE__);
                        return RuntimeValue();
                    }

                    Value * copy() const {
                        return new HitVarType();
                    }
                };

                return new HitVarType();
            } else if (var == "animtype"){
                class HitVarAnimType: public HitVar {
                public:
                    RuntimeValue evaluate(const Environment & environment) const {
                        return RuntimeValue(state(environment).animationType);
                    }

                    Value * copy() const {
                        return new HitVarAnimType();
                    }
                };

                return new HitVarAnimType();
            } else if (var == "airtype"){
                class HitVarAirType: public HitVar {
                public:
                    RuntimeValue evaluate(const Environment & environment) const {
                        return RuntimeValue(state(environment).airType);
                    }

                    Value * copy() const {
                        return new HitVarAirType();
                    }
                };

                return new HitVarAirType();
            } else if (var == "groundtype"){
                class HitVarGroundType: public HitVar {
                public:
                    RuntimeValue evaluate(const Environment & environment) const {
                        return RuntimeValue(state(environment).groundType);
                    }

                    Value * copy() const {
                        return new HitVarGroundType();
                    }
                };

                return new HitVarGroundType();
            } else if (var == "damage"){
                class HitVarDamage: public HitVar {
                public:
                    RuntimeValue evaluate(const Environment & environment) const {
                        return RuntimeValue(state(environment).damage);
                    }

                    Value * copy() const {
                        return new HitVarDamage();
                    }
                };

                return new HitVarDamage();
            } else if (var == "hitcount"){
                class HitVarHitCount: public HitVar {
                public:
                    RuntimeValue evaluate(const Environment & environment) const {
                        return RuntimeValue(state(environment).hits);
                    }

                    Value * copy() const {
                        return new HitVarHitCount();
                    }
                };

                return new HitVarHitCount();
            } else if (var == "fallcount"){
                /* TODO */
            } else if (var == "hitshaketime"){
                class HitVarShakeTime: public HitVar {
                public:
                    RuntimeValue evaluate(const Environment & environment) const {
                        return RuntimeValue(environment.getCharacter().pauseTime());
                    }

                    Value * copy() const {
                        return new HitVarShakeTime();
                    }
                };
                return new HitVarShakeTime();
            } else if (var == "hittime"){
                class HitVarHitTime: public HitVar {
                public:
                    RuntimeValue evaluate(const Environment & environment) const {
                        return RuntimeValue(state(environment).hitTime);
                    }

                    Value * copy() const {
                        return new HitVarHitTime();
                    }
                };

                return new HitVarHitTime();
            } else if (var == "slidetime"){
                class HitVarSlideTime: public HitVar {
                public:
                    RuntimeValue evaluate(const Environment & environment) const {
                        return RuntimeValue(state(environment).slideTime);
                    }

                    Value * copy() const {
                        return new HitVarSlideTime();
                    }
                };

                return new HitVarSlideTime();
            } else if (var == "ctrltime"){
                /* ctrltime: Returns time before player regains control
                 * after guarding the hit. (int)
                 */
                class HitVarCtrlTime: public HitVar {
                public:
                    RuntimeValue evaluate(const Environment & environment) const {
                        return RuntimeValue(state(environment).returnControlTime);
                    }

                    Value * copy() const {
                        return new HitVarCtrlTime();
                    }
                };

                return new HitVarCtrlTime();
            } else if (var == "recovertime"){
                class HitVarRecoverTime: public HitVar {
                public:
                    RuntimeValue evaluate(const Environment & environment) const {
                        return RuntimeValue(state(environment).recoverTime);
                    }

                    Value * copy() const {
                        return new HitVarRecoverTime();
                    }
                };

                return new HitVarRecoverTime();
            } else if (var == "isbound"){
                /* isbound: True if the player is the subject of an
                 * attacker's TargetBind controller. Useful to prevent being stuck
                 * in thrown states. (int)
                 */
                class HitVarIsBound: public HitVar {
                public:
                    RuntimeValue evaluate(const Environment & environment) const {
                        return RuntimeValue(environment.getCharacter().isBound());
                    }

                    virtual std::string toString() const {
                        return "gethitvar(isbound)";
                    }

                    Value * copy() const {
                        return new HitVarIsBound();
                    }
                };

                return new HitVarIsBound();
            } else if (var == "xoff"){
                /* TODO */
            } else if (var == "yoff"){
                /* TODO */
            } else if (var == "zoff"){
                /* TODO */
            } else if (var == "xvel"){
                class HitVarXVel: public HitVar {
                public:
                    RuntimeValue evaluate(const Environment & environment) const {
                        return RuntimeValue(state(environment).xVelocity);
                    }

                    Value * copy() const {
                        return new HitVarXVel();
                    }
                };

                return new HitVarXVel();
            } else if (var == "yvel"){
                class HitVarYVel: public HitVar {
                public:
                    RuntimeValue evaluate(const Environment & environment) const {
                        return RuntimeValue(state(environment).yVelocity);
                    }

                    Value * copy() const {
                        return new HitVarYVel();
                    }
                };

                return new HitVarYVel();
            } else if (var == "yaccel"){
                class HitVarYAccel: public HitVar {
                public:
                    RuntimeValue evaluate(const Environment & environment) const {
                        return RuntimeValue(state(environment).yAcceleration);
                    }

                    Value * copy() const {
                        return new HitVarYAccel();
                    }
                };

                return new HitVarYAccel();
            } else if (var == "hitid"){
                /* TODO */
            } else if (var == "chainid"){
                class ChainId: public HitVar {
                public:
                    RuntimeValue evaluate(const Environment & environment) const {
                        return RuntimeValue(state(environment).chainId);
                    }

                    Value * copy() const {
                        return new ChainId();
                    }
                };

                return new ChainId();
            } else if (var == "guarded"){
                class HitVarGuarded: public HitVar {
                public:
                    RuntimeValue evaluate(const Environment & environment) const {
                        return RuntimeValue(state(environment).guarded);
                    }

                    Value * copy() const {
                        return new HitVarGuarded();
                    }
                };

                return new HitVarGuarded();
            } else if (var == "fall"){
                class HitVarFall: public HitVar {
                public:
                    RuntimeValue evaluate(const Environment & environment) const {
                        return RuntimeValue(state(environment).fall.fall);
                    }

                    Value * copy() const {
                        return new HitVarFall();
                    }
                };

                return new HitVarFall();
            } else if (var == "fall.damage"){
                /* TODO */
            } else if (var == "fall.xvel"){
                class HitVarFallXVel: public HitVar {
                public:
                    RuntimeValue evaluate(const Environment & environment) const {
                        return RuntimeValue(state(environment).fall.xVelocity);
                    }

                    Value * copy() const {
                        return new HitVarFallXVel();
                    }
                };

                return new HitVarFallXVel();
            } else if (var == "fall.yvel"){
                class HitVarFallYVel: public HitVar {
                public:
                    RuntimeValue evaluate(const Environment & environment) const {
                        return RuntimeValue(state(environment).fall.yVelocity);
                    }

                    Value * copy() const {
                        return new HitVarFallYVel();
                    }
                };

                return new HitVarFallYVel();
            } else if (var == "fall.recover"){
                class HitVarFallRecover: public HitVar {
                public:
                    RuntimeValue evaluate(const Environment & environment) const {
                        return RuntimeValue(state(environment).fall.recover);
                    }

                    Value * copy() const {
                        return new HitVarFallRecover();
                    }
                };

                return new HitVarFallRecover();

            } else if (var == "fall.time"){
                /* TODO */
            } else if (var == "fall.recovertime"){
                class HitVarFallRecoverTime: public HitVar {
                public:
                    RuntimeValue evaluate(const Environment & environment) const {
                        return RuntimeValue(state(environment).fall.recoverTime);
                    }

                    Value * copy() const {
                        return new HitVarFallRecoverTime();
                    }
                };

                return new HitVarFallRecoverTime();
            }

            compileError("Unknown gethitvar variable " + var, __FILE__, __LINE__);
        }

        if (function == "teammode!="){
            /* FIXME */
            return compile(0);
        }
        
        if (function == "teammode"){
            /* FIXME compare if current mode is single, simul or turns return 1 otherwise return 0 */
            class TeamMode: public Value {
            public:
                enum Mode{
                    Single,
                    Turns,
                    Simul
                };

                TeamMode(const std::string & type):
                check(Single){
                    if (type == "single"){
                        check = Single;
                    } else if (type == "turns"){
                        check = Turns;
                    } else if (type == "simul"){
                        check = Simul;
                    }
                }

                TeamMode(const TeamMode & you):
                    check(you.check){
                }

                Value * copy() const {
                    return new TeamMode(*this);
                }

                Mode check;

                RuntimeValue evaluate(const Environment & environment) const {
                    /* FIXME: replace this with a check against the stage's
                     * current mode. something like
                     * check == environment.getStage().getMode()
                     */

                    switch (check){
                        case Single : return RuntimeValue(true);
                        case Turns : return RuntimeValue(false);
                        case Simul : return RuntimeValue(false);
                    } 

                    return RuntimeValue(false);
                }
            };

            return new TeamMode(function.getArg1()->toString());
        }

        /* it would be nice to combine var/fvar/sysvar */
        if (function == "var"){
            class FunctionVar: public Value{
            public:
                FunctionVar(int index):
                    index(index){
                    }

                int index;

                Value * copy() const {
                    return new FunctionVar(index);
                }

                virtual std::string toString() const {
                    std::ostringstream out;
                    out << "var(" << index << ")";
                    return out.str();
                }

                RuntimeValue evaluate(const Environment & environment) const {
                    return environment.getCharacter().getVariable(index);
                }
            };
            
            Value * compiled = compile(function.getArg1());
            int index = (int) compiled->evaluate(EmptyEnvironment()).toNumber();
            delete compiled;
            return new FunctionVar(index);
        }

        if (function == "fvar"){
            class FunctionFVar: public Value {
            public:
                FunctionFVar(int index):
                    index(index){
                    }

                const int index;

                Value * copy() const {
                    return new FunctionFVar(index);
                }

                RuntimeValue evaluate(const Environment & environment) const {
                    return environment.getCharacter().getFloatVariable(index);
                    /*
                    Value * value = 0;
                    // Global::debug(0) << "Fvar value is " << value << std::endl;
                    value = environment.getCharacter().getFloatVariable(index);
                    // Global::debug(0) << "Fvar value is now " << value << std::endl;
                    */
                }
            };
            
            Value * compiled = compile(function.getArg1());
            int index = (int) compiled->evaluate(EmptyEnvironment()).toNumber();
            delete compiled;
            return new FunctionFVar(index);
        }
        
        if (function == "sysfvar"){
            class FunctionSysFVar: public Value{
            public:
                FunctionSysFVar(int index):
                    index(index){
                    }

                Value * copy() const {
                    return new FunctionSysFVar(index);
                }

                int index;

                RuntimeValue evaluate(const Environment & environment) const {
                    return environment.getCharacter().getSystemVariable(index);
                }
            };
            
            Value * compiled = compile(function.getArg1());
            double index = (double) compiled->evaluate(EmptyEnvironment()).toNumber();
            delete compiled;
            return new FunctionSysFVar(index);
        }

        if (function == "sysvar"){
            class FunctionSysVar: public Value{
            public:
                FunctionSysVar(int index):
                    index(index){
                    }

                int index;

                Value * copy() const {
                    return new FunctionSysVar(index);
                }

                virtual std::string toString() const {
                    std::ostringstream out;
                    out << "sysvar(" << index << ")";
                    return out.str();
                }

                RuntimeValue evaluate(const Environment & environment) const {
                    return environment.getCharacter().getSystemVariable(index);
                }
            };
            
            Value * compiled = compile(function.getArg1());
            int index = (int) compiled->evaluate(EmptyEnvironment()).toNumber();
            delete compiled;
            return new FunctionSysVar(index);
        }

        if (function == "selfanimexist"){
            class SelfAnimExist: public Value {
            public:
                SelfAnimExist(Value * animation):
                    animation(animation){
                    }

                Value * animation;

                virtual ~SelfAnimExist(){
                    delete animation;
                }

                Value * copy() const {
                    return new SelfAnimExist(Compiler::copy(animation));
                }

                virtual std::string toString() const {
                    std::ostringstream out;
                    out << "SelfAnimExist(" << animation->toString() << ")";
                    return out.str();
                }

                RuntimeValue evaluate(const Environment & environment) const {
                    /* FIXME this should actually be animexist... self checks if player has animation.
                       If the opponent had given animation it will not be checked */
                    int animation = (int) this->animation->evaluate(environment).toNumber();
                    return RuntimeValue(environment.getCharacter().hasAnimation(animation));
                }
            };

            return new SelfAnimExist(compile(function.getArg1()));
        }

        if (function == "ceil"){
            class Ceil: public Value {
            public:
                Ceil(Value * argument):
                    argument(argument){
                    }

                Value * argument;

                virtual ~Ceil(){
                    delete argument;
                }

                Value * copy() const {
                    return new Ceil(Compiler::copy(argument));
                }

                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(ceil(argument->evaluate(environment).toNumber()));
                }
            };

            return new Ceil(compile(function.getArg1()));
        }
        
        if (function == "acos"){
            return new MetaCircularArg1("acos", acos, compile(function.getArg1()));
        }
        
        if (function == "cos"){
            return new MetaCircularArg1("cos", cos, compile(function.getArg1()));
        }

        if (function == "animexist"){
            class AnimExist: public Value {
            public:
                AnimExist(Value * animation):
                    animation(animation){
                    }

                Value * animation;

                virtual ~AnimExist(){
                    delete animation;
                }

                Value * copy() const {
                    return new AnimExist(Compiler::copy(animation));
                }

                RuntimeValue evaluate(const Environment & environment) const {
                    int animation = (int) this->animation->evaluate(environment).toNumber();
                    return RuntimeValue(environment.getCharacter().hasAnimation(animation));
                }
            };

            return new AnimExist(compile(function.getArg1()));
        }

        /* Gets the animation-time elapsed since the start of a specified element
         * of the current animation action. Useful for synchronizing events to
         * elements of an animation action.
         *
         * (reminder: first element of an action is element 1, not 0)
         */
        if (function == "animelemtime"){
            class FunctionAnimElemTime: public Value {
            public:
                FunctionAnimElemTime(Value * index):
                    index(index){
                    }

                Value * index;

                Value * copy() const {
                    return new FunctionAnimElemTime(Compiler::copy(index));
                }

                virtual ~FunctionAnimElemTime(){
                    delete index;
                }
        
                virtual std::string toString() const {
                    std::ostringstream out;
                    out << "animelemtime(" << index->toString() << ")";
                    return out.str();
                }

                RuntimeValue evaluate(const Environment & environment) const {
                    int index = (int) this->index->evaluate(environment).toNumber();
                    PaintownUtil::ReferenceCount<Animation> animation = environment.getCharacter().getCurrentAnimation();
                    if (animation == NULL){
                        runtimeError("Current animation is NULL", __FILE__, __LINE__);
                    }
                    return RuntimeValue(animation->animationElementElapsed(index));
                }
            };

            return new FunctionAnimElemTime(compile(function.getArg1()));
        }

        /*
        if (function == "animelem"){
            class FunctionAnimElem: public Value {
            public:
                FunctionAnimElem(Value * index):
                    index(index){
                    }

                Value * index;

                virtual ~FunctionAnimElem(){
                    delete index;
                }

                RuntimeValue evaluate(const Environment & environment) const {
                    unsigned int index = (unsigned int) this->index->evaluate(environment).toNumber();
                    Animation * animation = environment.getCharacter().getCurrentAnimation();
                    if (animation->getPosition() + 1 == index){
                        / * handle the second argument of animelem here * /
                        return RuntimeValue(animation->getTicks() == 0);
                    }
                    return RuntimeValue(false);

                }
            };
            return new FunctionAnimElem(compile(function.getArg1()));
            // return RuntimeValue(environment.getCharacter().getCurrentAnimation()->getPosition() + 1 == index == 0);
        }
        */

        /*
        if (function == "animelem<="){
            class FunctionAnimElem: public Value {
            public:
                FunctionAnimElem(Value * index):
                    index(index){
                    }

                Value * index;

                virtual ~FunctionAnimElem(){
                    delete index;
                }

                RuntimeValue evaluate(const Environment & environment) const {
                    unsigned int index = (unsigned int) this->index->evaluate(environment).toNumber();
                    Animation * animation = environment.getCharacter().getCurrentAnimation();
                    if (animation->getPosition() + 1 <= index){
                        / * handle the second argument of animelem here * /
                        return RuntimeValue(animation->getTicks() == 0);
                    }
                    return RuntimeValue(false);

                }
            };
            return new FunctionAnimElem(compile(function.getArg1()));
            // return RuntimeValue(environment.getCharacter().getCurrentAnimation()->getPosition() + 1 == index == 0);
        }
        */

        if (function == "numexplod"){
            class NumExplod: public Value{
            public:
                NumExplod(Value * id):
                id(id){
                }

                Value * id;

                virtual ~NumExplod(){
                    delete id;
                }

                Value * copy() const {
                    return new NumExplod(Compiler::copy(id));
                }

                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue((int) environment.getStage().findExplode((int) id->evaluate(environment).toNumber(), &environment.getCharacter()).size());
                }
            };

            return new NumExplod(compile(function.getArg1()));
        }

        if (function == "animelemno"){
            class AnimElemNo: public Value{
            public:
                AnimElemNo(Value * index):
                    index(index){
                    }

                Value * index;

                virtual ~AnimElemNo(){
                    delete index;
                }

                Value * copy() const {
                    return new AnimElemNo(Compiler::copy(index));
                }

                RuntimeValue evaluate(const Environment & environment) const {
                    if (environment.getCharacter().getCurrentAnimation() == NULL){
                        std::ostringstream out;
                        out << "No animation for position " << environment.getCharacter().getAnimation() << std::endl;
                        runtimeError(out.str(), __FILE__, __LINE__);
                    }
                    /* FIXME */
                    unsigned int index = (unsigned int) this->index->evaluate(environment).toNumber();
                    return RuntimeValue((int) (environment.getCharacter().getCurrentAnimation()->getPosition() + 1));
                }
            };

            return new AnimElemNo(compile(function.getArg1()));
        }

        if (function == "numtarget"){
            class NumTarget: public Value{
            public:
                NumTarget(Value * index):
                    index(index){
                    }

                Value * index;

                virtual ~NumTarget(){
                    delete index;
                }
 
                virtual std::string toString() const {
                    std::ostringstream out;
                    out << "numtarget(" << index->toString() << ")";
                    return out.str();
                }

                Value * copy() const {
                    return new NumTarget(Compiler::copy(index));
                }

                RuntimeValue evaluate(const Environment & environment) const {
                    const map<int, vector<Character*> > & targets = environment.getCharacter().getTargets();
                    int index = this->index->evaluate(environment).toNumber();
                    if (targets.find(index) != targets.end()){
                        const vector<Character*> & found = targets.find(index)->second;
                        return (int) found.size();
                    }
                    return 0;
                }
            };

            return new NumTarget(compile(function.getArg1()));
        }

        std::ostringstream out;
        out << "Unknown function '" << function.toString() << "'";
        if (function.getLine() != -1){
            out << " at line " << function.getLine() << " column " << function.getColumn();
        }
        compileError(out.str(), __FILE__, __LINE__);
        return NULL;
    }

    virtual void onFunction(const Ast::Function & string){
        compiled = compileFunction(string);
    }

    Value * compileExpressionUnary(const Ast::ExpressionUnary & expression){
        class Unary: public Value {
        public:
            Unary(Value * expression, Ast::ExpressionUnary::UnaryType type):
                expression(expression),
                type(type){
                }

            Value * expression;
            Ast::ExpressionUnary::UnaryType type;

            virtual ~Unary(){
                delete expression;
            }

            virtual std::string toString() const {
                std::ostringstream out;
                switch (type){
                    case Ast::ExpressionUnary::Not : {
                        out << "!";
                        break;
                    }
                    case Ast::ExpressionUnary::Minus : {
                        out << "-";
                        break;
                    }
                    case Ast::ExpressionUnary::Negation : {
                        out << "~";
                        break;
                    }
                }
                out << expression->toString();
                return out.str();
            }

            Value * copy() const {
                return new Unary(Compiler::copy(expression), type);
            }

            RuntimeValue evaluate(const Environment & environment) const {
                switch (type){
                    case Ast::ExpressionUnary::Not : {
                        return RuntimeValue(!toBool(expression->evaluate(environment)));
                    }
                    case Ast::ExpressionUnary::Minus : {
                        return RuntimeValue(-toNumber(expression->evaluate(environment)));
                    }
                    case Ast::ExpressionUnary::Negation : {
                        return RuntimeValue(~(int)toNumber(expression->evaluate(environment)));
                    }
                    default: compileError("Can't get here", __FILE__, __LINE__); return RuntimeValue();
                }
            }
        };

        switch (expression.getExpressionType()){
            case Ast::ExpressionUnary::Not :
            case Ast::ExpressionUnary::Minus :
            case Ast::ExpressionUnary::Negation : {
                break;
            }
            default : {
                std::ostringstream out;
                out << "Unknown expression: " << expression.toString();
                compileError(out.str(), __FILE__, __LINE__);
            }
        }

        return new Unary(compile(expression.getExpression()), expression.getExpressionType());
    }
    
    virtual void onExpressionUnary(const Ast::ExpressionUnary & expression){
        compiled = compileExpressionUnary(expression);
    }

    Value * compileKeyword(const Ast::Keyword & keyword){
        if (keyword == "vel x"){
            class VelX: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getXVelocity());
                }

                virtual std::string toString() const {
                    return "vel x";
                }

                Value * copy() const {
                    return new VelX();
                }
            };

            return new VelX();
        }

        if (keyword == "vel y"){
            class VelY: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getYVelocity());
                }

                std::string toString() const {
                    return "vel y";
                }

                Value * copy() const {
                    return new VelY();
                }
            };

            return new VelY();
        }
        
        if (keyword == "hitvel x"){
            class HitVelX: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(- environment.getCharacter().getHitState().xVelocity);
                }

                Value * copy() const {
                    return new HitVelX();
                }
            };
 
            return new HitVelX();
        }
        
        if (keyword == "hitvel y"){
            class HitVelY: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getHitState().yVelocity);
                }

                Value * copy() const {
                    return new HitVelY();
                }
            };
 
            return new HitVelY();
        }

        if (keyword == "screenpos y"){
            class ScreenPosY: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    /* top of the screen is 0
                     * floor is some positive integer below that.
                     * character position: negative is above ground, positive below.
                     * absolute position is floor - character - top
                     */
                    int floor = environment.getStage().getFloor();
                    int y = environment.getCharacter().getYPosition();
                    int position = floor + y;
                    return RuntimeValue(position);
                }

                Value * copy() const {
                    return new ScreenPosY();
                }
            };

            return new ScreenPosY();
        }

        if (keyword == "screenpos x"){
            class ScreenPosX: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getX());
                }

                Value * copy() const {
                    return new ScreenPosX();
                }
            };

            return new ScreenPosX();
        }
        
        if (keyword == "pos y"){
            class PosY: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getYPosition());
                }

                virtual std::string toString() const {
                    return "pos y";
                }

                Value * copy() const {
                    return new PosY();
                }
            };

            return new PosY();
        }

        if (keyword == "pos x"){
            class PosX: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getX());
                }

                std::string toString() const {
                    return "pos x";
                }
                
                Value * copy() const {
                    return new PosX();
                }
            };

            return new PosX();
        }
        
        if (keyword == "p2bodydist x"){
            class P2BodyDistX: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    const Character * enemy = environment.getStage().getEnemy(&environment.getCharacter());
                    switch (environment.getCharacter().getFacing()){
                        case FacingLeft: return environment.getCharacter().getFrontX() - enemy->getFrontX();
                        case FacingRight: return enemy->getFrontX() - environment.getCharacter().getFrontX();
                    }
                }

                virtual std::string toString() const {
                    return "p2bodydist x";
                }

                Value * copy() const {
                    return new P2BodyDistX();
                }
            };

            return new P2BodyDistX();
        }

        if (keyword == "p2bodydist y"){
            class P2BodyDistY: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    const Character * enemy = environment.getStage().getEnemy(&environment.getCharacter());
                    return RuntimeValue(enemy->getY() - environment.getCharacter().getY());
                }
                
                virtual std::string toString() const {
                    return "p2bodydist y";
                }

                Value * copy() const {
                    return new P2BodyDistY();
                }
            };

            return new P2BodyDistY();
        }

        if (keyword == "parentdist x"){
            class ParentDistX: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    const Character & helper = environment.getCharacter();
                    if (helper.isHelper()){
                        const Helper & realHelper = *(const Helper*) &helper;
                        const Character * parent = realHelper.getParent();
                        if (parent == NULL){
                            runtimeError("Helper has no parent", __FILE__, __LINE__);
                        }
                        return parent->getX() - realHelper.getX();
                    }
                    runtimeError("Cannot use 'parentdist x' on a non-helper", __FILE__, __LINE__);
                    return RuntimeValue();
                }

                Value * copy() const {
                    return new ParentDistX();
                }
            };

            return new ParentDistX();
        }

        if (keyword == "parentdist y"){
            class ParentDistY: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    const Character & helper = environment.getCharacter();
                    if (helper.isHelper()){
                        const Helper & realHelper = *(const Helper*) &helper;
                        const Character * parent = realHelper.getParent();
                        if (parent == NULL){
                            runtimeError("Helper has no parent", __FILE__, __LINE__);
                        }
                        return realHelper.getY() - parent->getY();
                    }
                    runtimeError("Cannot use 'parentdist x' on a non-helper", __FILE__, __LINE__);
                    return RuntimeValue();
                }

                Value * copy() const {
                    return new ParentDistY();
                }
            };

            return new ParentDistY();
        }

        if (keyword == "p2dist x"){
            class P2DistX: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    const Character * enemy = environment.getStage().getEnemy(&environment.getCharacter());
                    /* FIXME: im not sure this is right, should it take into account
                     * the facing direction?
                     */
                    switch (environment.getCharacter().getFacing()){
                        case FacingLeft: return environment.getCharacter().getX() - enemy->getX();
                        case FacingRight: return enemy->getX() - environment.getCharacter().getX();
                    }
                }

                Value * copy() const {
                    return new P2DistX();
                }
            };

            return new P2DistX();
        }

        if (keyword == "p2dist y"){
            class P2DistY: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    const Character * enemy = environment.getStage().getEnemy(&environment.getCharacter());
                    return RuntimeValue(enemy->getY() - environment.getCharacter().getY());

                }

                Value * copy() const {
                    return new P2DistY();
                }
            };

            return new P2DistY();
        }

        if (keyword == "rootdist x"){
            /* This trigger is only valid for helper-type characters. RootDist returns the distance from the helper to its root. The root is the main player character who owns the helper: for instance, if you select Kumquat to play with, and Kumquat spawns a helper named Kiwi, who in turn spawns a helper named Penguin, then Penguin's root is Kumquat, and Penguin is a descendant of Kumquat. RootDist works similarly to P2Dist.
             * For comparing the Y-distance, RootDist gives the difference in the heights of the players' Y-axes. A negative value means that the root is above its descendant.
             *
             * For comparing the X-distance, ParentDist gives the X-distance of the root's axis from the descendant's axis. A positive value indicates the root is in front of its descendant.*/
            class RootDistX: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    const Character * root = environment.getCharacter().getRoot();
                    if (root == NULL){
                        runtimeError("No root", __FILE__, __LINE__);
                    }
                    return RuntimeValue(root->getX() - environment.getCharacter().getX());
                }

                Value * copy() const {
                    return new RootDistX();
                }
            };

            return new RootDistX();
        }
        
        if (keyword == "rootdist y"){
            class RootDistY: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    const Character * root = environment.getCharacter().getRoot();
                    if (root == NULL){
                        runtimeError("No root", __FILE__, __LINE__);
                    }
                    return RuntimeValue(environment.getCharacter().getY() - root->getY());
                }

                Value * copy() const {
                    return new RootDistY();
                }
            };

            return new RootDistY();
        }

        std::ostringstream out;
        out << "Unknown keyword '" << keyword.toString() << "'";
        compileError(out.str(), __FILE__, __LINE__);
        return NULL;
    }

    virtual void onKeyword(const Ast::Keyword & keyword){
        compiled = compileKeyword(keyword);
    }

    Value * compileExpressionInfix(const Ast::ExpressionInfix & expression){
        // Global::debug(1) << "Evaluate expression " << expression.toString() << endl;
        using namespace Ast;
        class Infix: public Value {
        public:
            Infix(Value * left, Value * right, ExpressionInfix::InfixType type):
                left(left),
                right(right),
                type(type){
                }

            Value * left;
            Value * right;
            ExpressionInfix::InfixType type;

            virtual ~Infix(){
                delete left;
                delete right;
            }

            Value * copy() const {
                return new Infix(Compiler::copy(left), Compiler::copy(right->copy()), type);
            }

            std::string toString() const {
                std::ostringstream out;
                out << left->toString();
                switch (type){
                    case ExpressionInfix::GreaterThanEquals : {
                        out << " >= ";
                        break;
                    }
                    case ExpressionInfix::LessThanEquals : {
                        out << " <= ";
                        break;
                    }
                    case ExpressionInfix::GreaterThan : {
                        out << " > ";
                        break;
                    }
                    case ExpressionInfix::Unequals : {
                        out << " != ";
                        break;
                    }
                    case ExpressionInfix::Or : {
                        out << " || ";
                        break;
                    }
                    case ExpressionInfix::And: {
                        out << " && ";
                        break;
                    }
                    case ExpressionInfix::LessThan: {
                        out << " < ";
                        break;
                    }
                    case ExpressionInfix::Assignment : {
                        out << " = ";
                        break;
                    }
                    case ExpressionInfix::Add : {
                        out << " + ";
                        break;
                    }
                    case ExpressionInfix::Subtract : {
                        out << " - ";
                        break;
                    }
                    case ExpressionInfix::Multiply : {
                        out << " * ";
                        break;
                    }
                    case ExpressionInfix::Divide : {
                        out << " / ";
                        break;
                    }
                    case ExpressionInfix::Modulo : {
                        out << " % ";
                        break;
                    }
                    case ExpressionInfix::Power : {
                        out << " ** ";
                        break;
                    }
                    case ExpressionInfix::Equals : {
                        out << " = ";
                        break;
                    }
                    case ExpressionInfix::XOr : {
                        out << " ^^ ";
                        break;
                    }
                    case ExpressionInfix::BitwiseOr : {
                        out << " | ";
                        break;
                    } 
                    case ExpressionInfix::BitwiseXOr : {
                        out << " ^ ";
                        break;
                    }
                    case ExpressionInfix::BitwiseAnd : {
                        out << " & ";
                        break;
                    }
                }
                out << right->toString();
                return out.str();
            }

            RuntimeValue evaluate(const Environment & environment) const {
                switch (type){
                    case ExpressionInfix::Or : {
                        return RuntimeValue(left->evaluate(environment).toBool() ||
                                            right->evaluate(environment).toBool());
                    }
                    case ExpressionInfix::XOr : {
                        return RuntimeValue(left->evaluate(environment).toBool() ^
                                            right->evaluate(environment).toBool());
                    }
                    case ExpressionInfix::And : {
                        return RuntimeValue(left->evaluate(environment).toBool() &&
                                            right->evaluate(environment).toBool());
                    }
                    case ExpressionInfix::BitwiseOr : {
                        return RuntimeValue(((int) left->evaluate(environment).toNumber()) |
                                            ((int) right->evaluate(environment).toNumber()));
                    }
                    case ExpressionInfix::BitwiseXOr : {
                        return RuntimeValue(((int) left->evaluate(environment).toNumber()) ^
                                            ((int) right->evaluate(environment).toNumber()));
                    }
                    case ExpressionInfix::BitwiseAnd : {
                        return RuntimeValue(((int) left->evaluate(environment).toNumber()) &
                                            ((int) right->evaluate(environment).toNumber()));
                    }
                    case ExpressionInfix::Assignment : {
                        /* FIXME: is this needed? */
                        return RuntimeValue(0);
                        break;
                    }
                    case ExpressionInfix::Equals : {
                        return RuntimeValue(left->evaluate(environment) == right->evaluate(environment));
                        break;
                    }
                    case ExpressionInfix::Unequals : {
                        return RuntimeValue(!(left->evaluate(environment) == right->evaluate(environment)));
                    }
                    case ExpressionInfix::GreaterThanEquals : {
                        return RuntimeValue(left->evaluate(environment) >= right->evaluate(environment));
                    }
                    case ExpressionInfix::GreaterThan : {
                        return RuntimeValue(left->evaluate(environment) > right->evaluate(environment));
                    }
                    case ExpressionInfix::LessThanEquals : {
                        return RuntimeValue(left->evaluate(environment) <= right->evaluate(environment));
                    }
                    case ExpressionInfix::LessThan : {
                        return RuntimeValue(left->evaluate(environment) < right->evaluate(environment));
                    }
                    case ExpressionInfix::Add : {
                        return RuntimeValue(left->evaluate(environment).toNumber() + right->evaluate(environment).toNumber());
                    }
                    case ExpressionInfix::Subtract : {
                        return RuntimeValue(left->evaluate(environment).toNumber() - right->evaluate(environment).toNumber());
                    }
                    case ExpressionInfix::Multiply : {
                        return RuntimeValue(left->evaluate(environment).toNumber() * right->evaluate(environment).toNumber());
                    }
                    case ExpressionInfix::Divide : {
                        /* FIXME: catch divide by 0 */
                        return RuntimeValue(left->evaluate(environment).toNumber() / right->evaluate(environment).toNumber());
                    }
                    case ExpressionInfix::Modulo : {
                        int result_left = (int) left->evaluate(environment).toNumber();
                        int result_right = (int) right->evaluate(environment).toNumber();
                        if (result_right == 0){
                            runtimeError("mod by 0", __FILE__, __LINE__);
                        }
                        return RuntimeValue(result_left % result_right);
                    }
                    case ExpressionInfix::Power : {
                        return RuntimeValue(pow(left->evaluate(environment).toNumber(), right->evaluate(environment).toNumber()));
                    }
                }

                runtimeError("Can't get here", __FILE__, __LINE__);
                return RuntimeValue();
            }
        };

        return new Infix(compile(expression.getLeft()), compile(expression.getRight()), expression.getExpressionType());

        /*
        std::ostringstream out;
        out << "Unknown expression: " << expression.toString();
        compileError(out.str(), __FILE__, __LINE__);
        */
    }

    virtual void onExpressionInfix(const Ast::ExpressionInfix & expression){
        compiled = compileExpressionInfix(expression);
    }

    Value * compileValueList(const Ast::ValueList & value){
        class ValueList: public Value {
        public:
            RuntimeValue evaluate(const Environment & environment) const {
                /* FIXME */
                return RuntimeValue(false);
            }

            std::string toString() const {
                return "value-list";
            }

            Value * copy() const {
                return new ValueList();
            }
        };

        return new ValueList();
    }

    virtual void onValueList(const Ast::ValueList & value){
        compiled = compileValueList(value);
    }
};

}
        
Value::Value(){
}

std::string Value::toString() const {
    std::ostringstream out;
    out << this;
    return out.str();
}

Value::~Value(){
}

/* Ast object -> Compiled object
 * Caller must delete the result at some point.
 */
Value * compile(const Ast::Value * input){
    if (input == NULL){
        compileError("Tried to compile null input", __FILE__, __LINE__);
    }
    CompileWalker compiler;
    try{
        input->walk(compiler);
    } catch (const MugenException & e){
        std::ostringstream out;
        out << e.getFullReason() << " while compiling expression '" << input->toString() << "'";
        compileError(out.str(), __FILE__, __LINE__);
    }

    if (compiler.compiled == NULL){
        std::ostringstream out;
        out << "Unable to compile expression '" << input->toString() << "' at line " << input->getLine() << " column " << input->getColumn();
        compileError(out.str(), __FILE__, __LINE__);
    }
    return compiler.compiled;
}

Value * compile(const Ast::Value & input){
    return compile(&input);
}
    
Value * compileAndDelete(const Ast::Value * input){
    Value * compiled = compile(input);
    delete input;
    return compiled;
}

Value * compile(int immediate){
    Ast::Number number(-1, -1, immediate);
    return CompileWalker::compileNumber(number);
}

Value * compile(double immediate){
    Ast::Number number(-1, -1, immediate);
    return CompileWalker::compileNumber(number);
}
    
Value * copy(Value * value){
    if (value != NULL){
        return value->copy();
    }
    return NULL;
}

}
}

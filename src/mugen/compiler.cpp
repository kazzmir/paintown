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

namespace PaintownUtil = ::Util;
    
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
namespace Compiler{

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
                throw MugenException(std::string("Invalid attack type '") + attack + "'");
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
	    /* FIXME add ID's to character
	    class ID : public Value{
	    public:
		RuntimeValue evaluate(const Environment & environment) const {
		    return RuntimeValue(environment.getCharacter().getID());
		}
	    };
	    
	    return new ID(); */
	    return compile(0);
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

        if (PaintownUtil::matchRegex(PaintownUtil::lowerCaseAll(identifier.toString()), "projhit\\d*")){
            /* FIXME */
            return compile(0);
        }

        if (PaintownUtil::matchRegex(PaintownUtil::lowerCaseAll(identifier.toString()), "projcontact\\d*")){
            /* FIXME */
            return compile(0);
        }

        if (PaintownUtil::matchRegex(PaintownUtil::lowerCaseAll(identifier.toString()), "projguarded\\d*")){
            /* FIXME */
            return compile(0);
        }

        if (identifier == "numproj"){
            /* FIXME */
            return compile(0);
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

        if (identifier == "backedgebodydist" ||
            identifier == "backedgedist"){
            class BackEdgeBodyDist: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    int x = environment.getCharacter().getBackX();
                    return RuntimeValue(PaintownUtil::min(x - environment.getStage().maximumLeft(),
                                                          environment.getStage().maximumRight() - x));
                }

                Value * copy() const {
                    return new BackEdgeBodyDist();
                }
            };

            return new BackEdgeBodyDist();
        }

        /*
        if (identifier == "backedgedist"){
            class BackEdgeDist: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                }
            };
        }
        */

        if (identifier == "frontedgebodydist" ||
            identifier == "frontedgedist"){
            class FrontEdgeBodyDist: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    int x = environment.getCharacter().getFrontX();
                    return RuntimeValue(PaintownUtil::min(x - environment.getStage().maximumLeft(),
                                                          environment.getStage().maximumRight() - x));
                }

                Value * copy() const {
                    return new FrontEdgeBodyDist();
                }
            };

            return new FrontEdgeBodyDist();
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
                    return RuntimeValue("I");
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
                    return RuntimeValue("H");
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
                    /* FIXME */
                    return RuntimeValue(0);
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
                    /* FIXME */
                    return RuntimeValue(false);
                }

                Value * copy() const {
                    return new Winko();
                }
            };

            return new Winko();
        }

        if (identifier == "winperfect"){
            /* FIXME */
            return compile(0);
        }

        if (identifier == "win"){
            class Win: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    /* FIXME */
                    return RuntimeValue(0);
                }

                Value * copy() const {
                    return new Win();
                }
            };

            return new Win();
        }

        if (identifier == "lose"){
            /* FIXME */
            return compile(0);
        }
        
        if (identifier == "loseko"){
            /* FIXME */
            return compile(0);
        }
        
        if (identifier == "losetime"){
            /* FIXME */
            return compile(0);
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
                    /* FIXME */
                    return RuntimeValue(0);
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
                    /* FIXME */
                    return RuntimeValue(1);
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

                    bool out = false;
                    out |= enemy->getMoveType() == Move::Attack;

                    /* FIXME: deal with distance as well */
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
                    MugenAnimation * animation = environment.getCharacter().getCurrentAnimation();
                    if (animation == NULL){
                        std::ostringstream out;
                        out << "No animation for position " << environment.getCharacter().getAnimation() << std::endl;
                        throw MugenException(out.str());
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

        if (identifier == "projhit"){
            class ProjHit: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    /* FIXME */
                    return RuntimeValue(0);
                }

                Value * copy() const {
                    return new ProjHit();
                }
            };

            return new ProjHit();
        }

        if (identifier == "numexplod"){
            class NumExplod: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return environment.getStage().countMyEffects(&environment.getCharacter());
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
        throw MugenException(out.str());
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
                            throw MugenException("Helper has no parent");
                        }
                        FullEnvironment parentEnvironment(environment.getStage(), *parent, environment.getCommands());
                        return argument->evaluate(parentEnvironment);
                    }
                    throw MugenException("Cannot redirect to a parent from a non-helper");
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
                    /* FIXME */
                    return RuntimeValue(0);
                }
            };

            return new Root(compile(helper.getOriginal()));
        }
        
        if (helper == "helper"){
            class Helper: public Value {
            public:
                Helper(Value * argument):
                    argument(argument){
                    }

                Value * argument;

                virtual ~Helper(){
                    delete argument;
                }

                Value * copy() const {
                    return new Helper(Compiler::copy(argument));
                }

                RuntimeValue evaluate(const Environment & environment) const {
                    /* FIXME */
                    return RuntimeValue(0);
                }
            };

            return new Helper(compile(helper.getOriginal()));
        }

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
        throw MugenException(out.str());
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
                    default : throw MugenException("Can't get here");
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
            default : throw MugenException("Unexpected range type");
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
                        class DataLife: public Value {
                        public:
                            RuntimeValue evaluate(const Environment & environment) const {
                                return RuntimeValue(environment.getCharacter().getMaxHealth());
                            }

                            Value * copy() const {
                                return new DataLife();
                            }
                        };

                        return new DataLife();
                    }
                    
                    if (identifier == "data.power"){
                        class DataPower: public Value {
                        public:
                            RuntimeValue evaluate(const Environment & environment) const {
                                return RuntimeValue(environment.getCharacter().getPower());
                            }

                            Value * copy() const {
                                return new DataPower();
                            }
                        };

                        return new DataPower();
                    }

                    if (identifier == "data.liedown.time"){
                        /* FIXME */
                        return compile(0);
		    }

                    /* TODO implement rest 
                    if (identifier == "data.attack"){
		    }
                    if (identifier == "data.defence"){
		    }
                    if (identifier == "data.fall.defence_mul"){
		    }
                    if (identifier == "data.airjuggle"){
		    }
                    if (identifier == "data.sparkno"){
		    }
                    if (identifier == "data.guard.sparkno"){
		    }
                    if (identifier == "data.ko.echo"){
		    }
                    if (identifier == "data.intpersistindex"){
		    }
                    if (identifier == "data.attack"){
		    }
		    
                    if (identifier == "movement.airjump.num"){
		    }
		    
                    if (identifier == "movement.airjump.height"){
		    }
                    */ 
		    
                    if (identifier == "movement.yaccel"){
                        class MovementYAccel: public Value {
                        public:
                            RuntimeValue evaluate(const Environment & environment) const {
                                return RuntimeValue(environment.getCharacter().getGravity());
                            }

                            Value * copy() const {
                                return new MovementYAccel();
                            }
                        };

                        return new MovementYAccel();
                    }
                    
                    /* FIXME others
			movement.crouch.friction: Returns value of the "crouch.friction" parameter. (float)
			movement.stand.friction.threshold: Returns value of the "stand.friction.threshold" parameter. (float)
			movement.crouch.friction.threshold: Returns value of the "crouch.friction.threshold" parameter. (float)
			movement.jump.changeanim.threshold: Returns value of the "jump.changeanim.threshold" parameter. (float)
			movement.air.gethit.groundlevel: Returns value of the "air.gethit.groundlevel" parameter. (float)
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

                    if (identifier == "movement.stand.friction"){
                        class StandFriction: public Value {
                        public:
                            RuntimeValue evaluate(const Environment & environment) const {
                                return RuntimeValue(environment.getCharacter().getStandingFriction());
                            }

                            Value * copy() const {
                                return new StandFriction();
                            }
                        };

                        return new StandFriction();
                    }

                    if (identifier == "velocity.walk.back.x"){
                        class VelocityBackX: public Value {
                        public:
                            RuntimeValue evaluate(const Environment & environment) const {
                                return RuntimeValue(environment.getCharacter().getWalkBackX());
                            }

                            Value * copy() const {
                                return new VelocityBackX();
                            }
                        };

                        return new VelocityBackX();
                    }

                    if (identifier == "velocity.walk.fwd.x"){
                        class VelocityForwardX: public Value {
                        public:
                            RuntimeValue evaluate(const Environment & environment) const {
                                return RuntimeValue(environment.getCharacter().getWalkForwardX());
                            }

                            Value * copy() const {
                                return new VelocityForwardX();
                            }
                        };

                        return new VelocityForwardX();
                    }

                    if (identifier == "velocity.run.fwd.x"){
                        class RunForwardX: public Value {
                        public:
                            RuntimeValue evaluate(const Environment & environment) const {
                                return RuntimeValue(environment.getCharacter().getRunForwardX());
                            }

                            Value * copy() const {
                                return new RunForwardX();
                            }
                        };

                        return new RunForwardX();
                    }

                    if (identifier == "velocity.run.fwd.y"){
                        class RunForwardY: public Value {
                        public:
                            RuntimeValue evaluate(const Environment & environment) const {
                                return RuntimeValue(environment.getCharacter().getRunForwardY());
                            }

                            Value * copy() const {
                                return new RunForwardY();
                            }
                        };

                        return new RunForwardY();
                    }

                    if (identifier == "velocity.jump.neu.x"){
                        class JumpX: public Value {
                        public:
                            RuntimeValue evaluate(const Environment & environment) const {
                                return RuntimeValue(environment.getCharacter().getNeutralJumpingX());
                            }

                            Value * copy() const {
                                return new JumpX();
                            }
                        };

                        return new JumpX();
                    }

                    if (identifier == "velocity.jump.y" ||
                        /* HACK: mugen docs don't say runjump.y is the same as
                         * jump.y, but some characters use it anyway (Gouki)
                         */
                        identifier == "velocity.runjump.y"){
                        class JumpY: public Value {
                        public:
                            RuntimeValue evaluate(const Environment & environment) const {
                                return RuntimeValue(environment.getCharacter().getNeutralJumpingY());
                            }

                            Value * copy() const {
                                return new JumpY();
                            }
                        };

                        return new JumpY();
                    }

                    if (identifier == "velocity.runjump.back.x"){
                        class RunBackJumpX: public Value {
                        public:
                            RuntimeValue evaluate(const Environment & environment) const {
                                return RuntimeValue(environment.getCharacter().getRunJumpBack());
                            }

                            Value * copy() const {
                                return new RunBackJumpX();
                            }
                        };

                        return new RunBackJumpX();
                    }

                    if (identifier == "velocity.run.back.x"){
                        class RunBackX: public Value {
                        public:
                            RuntimeValue evaluate(const Environment & environment) const {
                                return RuntimeValue(environment.getCharacter().getRunBackX());
                            }

                            Value * copy() const {
                                return new RunBackX();
                            }
                        };

                        return new RunBackX();
                    }

                    if (identifier == "velocity.run.back.y"){
                        class RunBackY: public Value {
                        public:
                            RuntimeValue evaluate(const Environment & environment) const {
                                return RuntimeValue(environment.getCharacter().getRunBackY());
                            }

                            Value * copy() const {
                                return new RunBackY();
                            }
                        };

                        return new RunBackY();
                    }

                    if (identifier == "velocity.jump.back.x"){
                        class JumpBackX: public Value {
                        public:
                            RuntimeValue evaluate(const Environment & environment) const {
                                return RuntimeValue(environment.getCharacter().getJumpBack());
                            }

                            Value * copy() const {
                                return new JumpBackX();
                            }
                        };

                        return new JumpBackX();
                    }

                    if (identifier == "velocity.jump.fwd.x"){
                        class JumpForwardX: public Value {
                        public:
                            RuntimeValue evaluate(const Environment & environment) const {
                                return RuntimeValue(environment.getCharacter().getJumpForward());
                            }

                            Value * copy() const {
                                return new JumpForwardX();
                            }
                        };

                        return new JumpForwardX();
                    }

                    if (identifier == "velocity.runjump.fwd.x"){
                        class RunJumpForwardX: public Value {
                        public:
                            RuntimeValue evaluate(const Environment & environment) const {
                                return RuntimeValue(environment.getCharacter().getRunJumpForward());
                            }

                            Value * copy() const {
                                return new RunJumpForwardX();
                            }
                        };

                        return new RunJumpForwardX();
                    }

                    if (identifier == "velocity.airjump.neu.x"){
                        class AirJumpX: public Value {
                        public:
                            RuntimeValue evaluate(const Environment & environment) const {
                                return RuntimeValue(environment.getCharacter().getAirJumpNeutralX());
                            }

                            Value * copy() const {
                                return new AirJumpX();
                            }
                        };

                        return new AirJumpX();
                    }

                    if (identifier == "velocity.airjump.y"){
                        class AirJumpY: public Value {
                        public:
                            RuntimeValue evaluate(const Environment & environment) const {
                                return RuntimeValue(environment.getCharacter().getAirJumpNeutralY());
                            }

                            Value * copy() const {
                                return new AirJumpY();
                            }
                        };

                        return new AirJumpY();
                    }

                    if (identifier == "velocity.airjump.back.x"){
                        class AirJumpBackX: public Value {
                        public:
                            RuntimeValue evaluate(const Environment & environment) const {
                                return RuntimeValue(environment.getCharacter().getAirJumpBack());
                            }

                            Value * copy() const {
                                return new AirJumpBackX();
                            }
                        };

                        return new AirJumpBackX();
                    }

                    if (identifier == "velocity.airjump.fwd.x"){
                        class AirJumpForwardX: public Value {
                        public:
                            RuntimeValue evaluate(const Environment & environment) const {
                                return RuntimeValue(environment.getCharacter().getAirJumpForward());
                            }

                            Value * copy() const {
                                return new AirJumpForwardX();
                            }
                        };

                        return new AirJumpForwardX();
                    }
                    
                    if (identifier == "size.xscale"){
                        /* FIXME */
                        return compile(0);
                    }
                    
                    if (identifier == "size.yscale"){
                        /* FIXME */
                        return compile(0);
                    }
                    
                    if (identifier == "size.ground.back"){
                        /* FIXME */
                        return compile(0);
                    }
		    
                    if (identifier == "size.ground.front"){
                        /* FIXME */
                        return compile(0);
                    }
		    
                    if (identifier == "size.air.back"){
                        /* FIXME */
                        return compile(0);
                    }
		    
                    if (identifier == "size.air.front"){
                        /* FIXME */
                        return compile(0);
                    }
		    
                    if (identifier == "size.height"){
                        /* FIXME */
                        return compile(0);
                    }
                    
                    if (identifier == "size.attack.dist"){
                        /* FIXME */
                        return compile(0);
                    }
		    
                    if (identifier == "size.proj.attack.dist"){
                        /* FIXME */
                        return compile(0);
                    }
                    
                    if (identifier == "size.proj.doscale"){
                        /* FIXME */
                        return compile(0);
                    }
		    
                    if (identifier == "size.head.pos.x"){
                        /* FIXME */
                        return compile(0);
                    }

                    if (identifier == "size.head.pos.y"){
                        /* FIXME */
                        return compile(0);
                    }

                    if (identifier == "size.mid.pos.x"){
                        /* FIXME */
                        return compile(0);
                    }
                    
                    if (identifier == "size.mid.pos.y"){
                        /* FIXME */
                        return compile(0);
                    }
                    
                    if (identifier == "size.shadowoffset"){
                        /* FIXME */
                        return compile(0);
                    }

                    if (identifier == "size.draw.offset.x"){
                        /* FIXME */
                        return compile(0);
                    }

                    if (identifier == "size.draw.offset.y"){
                        /* FIXME */
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
                throw MugenException("Unknown const value " + function.getArg1()->toString());
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

        if (PaintownUtil::matchRegex(PaintownUtil::lowerCaseAll(function.getName()), "projcontact\\d*")){
            /* FIXME */
            return compile(0);
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
			throw MugenException(out.str());
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
			throw MugenException(out.str());
		    } else if (value <= 0){
                        std::ostringstream out;
                        out << "Value of log must be positive but was " << value;
			throw MugenException(out.str());
		    }

		    const double result = log(value) / log(base);
		    
                    return RuntimeValue(result);
                }
	    };

	    return new Log(compile(function.getArg1()),compile(function.getArg2()));
	}

        if (function == "numprojid"){
            /* FIXME */
            return compile(0);
        }

        if (function == "projhittime"){
            /* FIXME */
            return compile(-1);
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
            /* FIXME */
            return compile(-1);
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
                throw MugenException("No argument given to gethitvar");
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
                        throw MugenException("Invalid hit type");
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
                /* TODO */
            } else if (var == "isbound"){
                /* isbound: True if the player is the subject of an
                 * attacker's TargetBind controller. Useful to prevent being stuck
                 * in thrown states. (int)
                 */
                /* FIXME */
                class HitVarIsBound: public HitVar {
                public:
                    RuntimeValue evaluate(const Environment & environment) const {
                        return RuntimeValue(false);
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

            throw MugenException("Unknown gethitvar variable " + var);
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
                    MugenAnimation * animation = environment.getCharacter().getCurrentAnimation();
                    if (animation == NULL){
                        throw MugenException("Current animation is NULL");
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
                    MugenAnimation * animation = environment.getCharacter().getCurrentAnimation();
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
                    MugenAnimation * animation = environment.getCharacter().getCurrentAnimation();
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
            /* FIXME */
            return compile(0);
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
                        throw MugenException(out.str());
                    }
                    /* FIXME */
                    unsigned int index = (unsigned int) this->index->evaluate(environment).toNumber();
                    return RuntimeValue((int) (environment.getCharacter().getCurrentAnimation()->getPosition() + 1));
                }
            };

            return new AnimElemNo(compile(function.getArg1()));
        }

        if (function == "numtarget"){
            /* FIXME */
            return compile(0);
        }

        std::ostringstream out;
        out << "Unknown function '" << function.toString() << "'";
        if (function.getLine() != -1){
            out << " at line " << function.getLine() << " column " << function.getColumn();
        }
        throw MugenException(out.str());
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
                out << "-" << expression->toString();
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
                    default : throw MugenException("Can't get here");
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
                throw MugenException(out.str());
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
                     * character position: positive is above ground, negative below.
                     * absolute position is floor - character - top
                     */
                    int floor = environment.getStage().getFloor();
                    int y = environment.getCharacter().getYPosition();
                    int position = floor - y;
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
                    return RuntimeValue(-environment.getCharacter().getYPosition());
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
                
                Value * copy() const {
                    return new PosX();
                }
            };

            return new PosX();
        }
        
        /* FIXME: verify p2bodydist is right. for now they are copy/pastes of p2dist */
        if (keyword == "p2bodydist x"){
            class P2BodyDistX: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    const Character * enemy = environment.getStage().getEnemy(&environment.getCharacter());
                    /* FIXME: im not sure this is right, should it take into account
                     * the facing direction?
                     */
                    return RuntimeValue(enemy->getX() - environment.getCharacter().getX());

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
                    return RuntimeValue(-enemy->getY() - (-environment.getCharacter().getY()));
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
                            throw MugenException("Helper has no parent");
                        }
                        return parent->getX() - realHelper.getX();
                    }
                    throw MugenException("Cannot use 'parentdist x' on a non-helper");
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
                            throw MugenException("Helper has no parent");
                        }
                        return realHelper.getY() - parent->getY();
                    }
                    throw MugenException("Cannot use 'parentdist x' on a non-helper");
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
                    return RuntimeValue(enemy->getX() - environment.getCharacter().getX());

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
                    return RuntimeValue(-enemy->getY() - (-environment.getCharacter().getY()));

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
                        throw MugenException("No root");
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
                        throw MugenException("No root");
                    }
                    return RuntimeValue(environment.getCharacter().getX() - root->getY());
                }

                Value * copy() const {
                    return new RootDistY();
                }
            };

            return new RootDistY();
        }

        std::ostringstream out;
        out << "Unknown keyword '" << keyword.toString() << "'";
        throw MugenException(out.str());
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
                    case ExpressionInfix::Equals : {
                        out << " = ";
                        break;
                    }
                    default : break;
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
                        return RuntimeValue(!toBool(left->evaluate(environment) == right->evaluate(environment)));
                    }
                    case ExpressionInfix::GreaterThanEquals : {
                        return RuntimeValue(left->evaluate(environment).toNumber() >= right->evaluate(environment).toNumber());
                    }
                    case ExpressionInfix::GreaterThan : {
                        return RuntimeValue(left->evaluate(environment).toNumber() > right->evaluate(environment).toNumber());
                    }
                    case ExpressionInfix::LessThanEquals : {
                        return RuntimeValue(left->evaluate(environment).toNumber() <= right->evaluate(environment).toNumber());
                    }
                    case ExpressionInfix::LessThan : {
                        return RuntimeValue(left->evaluate(environment).toNumber() < right->evaluate(environment).toNumber());
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
                            throw MugenException("mod by 0");
                        }
                        return RuntimeValue(result_left % result_right);
                    }
                    case ExpressionInfix::Power : {
                        return RuntimeValue(pow(left->evaluate(environment).toNumber(), right->evaluate(environment).toNumber()));
                    }
                }

                throw MugenException("Can't get here");
            }
        };

        return new Infix(compile(expression.getLeft()), compile(expression.getRight()), expression.getExpressionType());

        std::ostringstream out;
        out << "Unknown expression: " << expression.toString();
        throw MugenException(out.str());
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
    CompileWalker compiler;
    try{
        input->walk(compiler);
    } catch (const MugenException & e){
        std::ostringstream out;
        out << e.getReason() << " while compiling expression '" << input->toString() << "'";
        throw MugenException(out.str());
    }

    if (compiler.compiled == NULL){
        std::ostringstream out;
        out << "Unable to compile expression '" << input->toString() << "'";
        throw MugenException(out.str());
    }
    return compiler.compiled;
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

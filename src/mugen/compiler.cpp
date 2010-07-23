#include "ast/all.h"
#include "compiler.h"
#include "exception.h"
#include "characterhud.h"
#include "character.h"
#include "stage.h"
#include "util/funcs.h"
#include <sstream>
#include <string>

namespace PaintownUtil = ::Util;
    
namespace Mugen{
namespace Compiler{

namespace{

class CompileWalker: public Ast::Walker {
public:
    CompileWalker():
    compiled(NULL){
    }

    Value * compiled;

    Value * compileIdentifier(const Ast::Identifier & identifier){
        if (identifier == "command"){
            class Command: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCommands());
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
            };

            return new Animation();
        }
        
        if (identifier == "alive"){
            class Alive: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getHealth() > 0);
                }
            };

            return new Alive();
        }

        if (identifier == "facing"){
            class Facing: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    if (environment.getCharacter().getFacing() == Object::FACING_LEFT){
                        return RuntimeValue(-1);
                    } else if (environment.getCharacter().getFacing() == Object::FACING_RIGHT){
                        return RuntimeValue(1);
                    }
                    return RuntimeValue(0);
                }
            };

            return new Facing();
	}

        if (identifier == "life"){
            class Life: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getHealth());
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
            };

            return new Wins();
	}
	
        if (identifier == "matchover"){
            class Over: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getStage().isMatchOver());
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
            };

            return new Random();
        }

        if (identifier == "roundno"){
            class RoundNumber: public Value {
                public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getStage().getGameInfo()->getRound().getRound());
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
            };

            return new RoundsExisted();
        }

        if (identifier == "I"){
            class JustI: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue("I");
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
            };

            return new P2StateNo();
        }

        if (identifier == "hitcount"){
            class HitCount: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getHitCount());
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
            };

            return new UniqueHitCount();

        }

        if (identifier == "p2statetype"){
            class P2StateType: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    const Character * enemy = environment.getStage().getEnemy(&environment.getCharacter());
                    return RuntimeValue(enemy->getStateType());
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
            };

            return new WinTime();
        }

        if (identifier == "win"){
            class Win: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    /* FIXME */
                    return RuntimeValue(0);
                }
            };

            return new Win();
        }

        if (identifier == "roundstate"){
            class RoundState: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    /* FIXME */
                    return RuntimeValue(2);
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
            };

            return new MoveContact();
        }

        if (identifier == "numtarget"){
            class NumTarget: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(1);
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
            };

            return new InGuardDist();
        }

        if (identifier == "animtime"){
            class AnimTime: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getCurrentAnimation()->animationTime());
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
            };

            return new PalNo();
        }

        if (identifier == "winko"){
            class Winko: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    /* FIXME */
                    return RuntimeValue(false);
                }
            };

            return new Winko();
        }

        if (identifier == "movehit"){
            class MoveHit: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    /* FIXME */
                    return RuntimeValue(0);
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
            };

            return new ProjHit();
        }

        if (identifier == "numexplod"){
            class NumExplod: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    /* FIXME: return total number of "explode" objects owned by the player */
                    return RuntimeValue(0);
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
            };

            return new GameTime();
	}

        if (identifier == "hitshakeover"){
            class HitShakeOver: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getHitState().shakeTime <= 0);
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
            };

            return new HitOver();
        }

        if (identifier == "canrecover"){
            class CanRecover: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().canRecover());
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
            };

            return new Time();
        }

        if (identifier == "A"){
            class JustA: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(std::string("A"));
                }
            };

            return new JustA();
        }
        
        if (identifier == "S"){
            class JustS: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    /* states are just strings */
                    return RuntimeValue(std::string("S"));
                }
            };

            return new JustS();
        }

        if (identifier == "C"){
            class JustC: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(std::string("C"));
                }
            };

            return new JustC();
        }
        
        if (identifier == "L"){
            class JustL: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(std::string("L"));
                }
            };

            return new JustL();
        }

        if (identifier == "statetype"){
            class StateType: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getStateType());
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
            };

            return new Ctrl();
        }

        /* god I need a drink */

        if (identifier == "stateno"){
            class StateNo: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getCurrentState());
                }
            };

            return new StateNo();
        }

        if (identifier == "power"){
            class Power: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getPower());
                }
            };

            return new Power();
        }
        
        if (identifier == "internal:extra-jumps"){
            class ExtraJumps: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getExtraJumps());
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
            };

            return new AirJump();
        }

        if (identifier == "velocity.walk.back.x"){
            class VelocityBackX: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getWalkBackX());
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
            };

            return new VelocityForwardX();
        }

        if (identifier == "velocity.run.fwd.x"){
            class RunForwardX: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getRunForwardX());
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
            };

            return new RunForwardY();
        }
        
        if (identifier == "velocity.jump.neu.x"){
            class JumpX: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getNeutralJumpingX());
                }
            };

            return new JumpX();
        }
        
        if (identifier == "velocity.jump.y"){
            class JumpY: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getNeutralJumpingY());
                }
            };

            return new JumpY();
        }

        if (identifier == "prevstateno"){
            class PrevStateNo: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getPreviousState());
                }
            };

            return new PrevStateNo();
        }
        
        if (identifier == "velocity.run.back.x"){
            class RunBackX: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getRunBackX());
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
            };

            return new RunBackY();
        }

        if (identifier == "velocity.jump.back.x"){
            class JumpBackX: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getJumpBack());
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
            };

            return new JumpForwardX();
        }

        if (identifier == "velocity.runjump.fwd.x"){
            class RunJumpForwardX: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getRunJumpForward());
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
            };

            return new AirJumpX();
        }
        
        if (identifier == "velocity.airjump.y"){
            class AirJumpY: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getAirJumpNeutralY());
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
            };

            return new AirJumpBackX();
        }
        
        if (identifier == "velocity.airjump.fwd.x"){
            class AirJumpForwardX: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getAirJumpForward());
                }
            };

            return new AirJumpForwardX();
        }

        if (identifier == "movement.yaccel"){
            class MovemntYAccel: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getGravity());
                }
            };

            return new MovemntYAccel();
        }

        std::ostringstream out;
        out << "Don't know how to compile identifier '" << identifier.toString() << "'";
        throw MugenException(out.str());
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
};

}
        
Value::Value(){
}

Value::~Value(){
}

/* Ast object -> Compiled object
 * Caller must delete the result at some point.
 */
Value * compile(const Ast::Value * input){
    CompileWalker compiler;
    input->walk(compiler);
    if (compiler.compiled == NULL){
        std::ostringstream out;
        out << "Unable to compile expression '" << input->toString() << "'";
        throw MugenException(out.str());
    }
    return compiler.compiled;
}

}
}

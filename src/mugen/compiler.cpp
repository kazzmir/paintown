#include "ast/all.h"
#include "compiler.h"
#include "exception.h"
#include "character.h"
#include <sstream>
    
namespace Mugen{
namespace Compiler{

namespace{

class CompileWalker: public Ast::Walker{
public:

    CompileWalker():
    compiled(NULL){
    }

    Value * compileIdentifier(const Ast::Identifier & identifier){
        if (identifier == "command"){
            class Command: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment){
                    return RuntimeValue(environment.getCommands());
                }
            };

            return new Command();
        }
        
        if (identifier == "anim"){
            class Animation: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment){
                    return RuntimeValue(environment.getCharacter().getAnimation());
                }
            };

            return new Animation();
        }
        
        if (identifier == "alive"){
            class Alive: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment){
                    return RuntimeValue(environment.getCharacter().getHealth() > 0);
                }
            };

            return new Alive();
        }

#if 0
        if (identifier == "facing"){
	    if (environment.getCharacter().getFacing() == Object::FACING_LEFT){
		return -1;
	    } else if (environment.getCharacter().getFacing() == Object::FACING_RIGHT){
		return 1;
	    }
	    return 0;
	}

        if (identifier == "life"){
            return RuntimeValue(environment.getCharacter().getHealth());
        }
        
        if (identifier == "lifemax"){
	    return RuntimeValue(environment.getCharacter().getMaxHealth());
	}
	
	if (identifier == "matchno"){
	    int wins = environment.getCharacter().getWins().size()+1;
	    return RuntimeValue(wins);
	}
	
        if (identifier == "matchover"){
            return RuntimeValue(environment.getStage().isMatchOver());
        }

        if (identifier == "movetype"){
            return RuntimeValue(environment.getCharacter().getMoveType());
        }

        if (identifier == "p2movetype"){
            const Character * enemy = environment.getStage().getEnemy(&environment.getCharacter());
            return RuntimeValue(enemy->getMoveType());
        }

        if (identifier == "random"){
            /* Returns a random number between 0 and 999, inclusive. */
            return RuntimeValue(PaintownUtil::rnd(1000));
        }

        if (identifier == "roundno"){
            return RuntimeValue(environment.getStage().getGameInfo()->getRound().getRound());
        }

        if (identifier == "roundsexisted"){
            /* FIXME */
            return RuntimeValue(0);
        }

        if (identifier == "I"){
            return RuntimeValue("I");
        }

        if (identifier == "H"){
            return RuntimeValue("H");
        }

        if (identifier == "p2stateno"){
            const Character * enemy = environment.getStage().getEnemy(&environment.getCharacter());
            return RuntimeValue(enemy->getCurrentState());
        }

        if (identifier == "hitcount"){
            return RuntimeValue(environment.getCharacter().getHitCount());
        }

        if (identifier == "uniqhitcount"){
            /* FIXME */
            return RuntimeValue(environment.getCharacter().getHitCount());
        }

        if (identifier == "p2statetype"){
            const Character * enemy = environment.getStage().getEnemy(&environment.getCharacter());
            return RuntimeValue(enemy->getStateType());
        }

        if (identifier == "wintime"){
            return RuntimeValue(0);
        }

        if (identifier == "win"){
            /* FIXME */
            return RuntimeValue(0);
        }

        if (identifier == "roundstate"){
            /* FIXME */
            return RuntimeValue(2);
        }

        if (identifier == "movecontact"){
            /* FIXME */
            return RuntimeValue(0);
        }

        if (identifier == "numtarget"){
            return RuntimeValue(1);
        }

        if (identifier == "inguarddist"){
            const Character * enemy = environment.getStage().getEnemy(&environment.getCharacter());

            bool out = false;
            out |= enemy->getMoveType() == Move::Attack;

            /* FIXME: deal with distance as well */
            return RuntimeValue(out);
        }

        if (identifier == "animtime"){
            return RuntimeValue(environment.getCharacter().getCurrentAnimation()->animationTime());
        }

        if (identifier == "palno"){
            return RuntimeValue((int) environment.getCharacter().getCurrentPalette());
        }

        if (identifier == "winko"){
            /* FIXME */
            return RuntimeValue(false);
        }

        if (identifier == "movehit"){
            /* FIXME */
            return RuntimeValue(0);
        }

        if (identifier == "projhit"){
            /* FIXME */
            return RuntimeValue(0);
        }

        if (identifier == "numexplod"){
            /* FIXME: return total number of "explode" objects owned by the player */
            return RuntimeValue(0);
        }
        
        if (identifier == "gametime"){
	    return RuntimeValue(environment.getStage().getGameTime());
	}

        if (identifier == "hitshakeover"){
            return RuntimeValue(environment.getCharacter().getHitState().shakeTime <= 0);
        }

        if (identifier == "hitover"){
            return RuntimeValue(environment.getCharacter().getHitState().hitTime <= -1);
        }

        if (identifier == "canrecover"){
            return RuntimeValue(environment.getCharacter().canRecover());
        }

        if (identifier == "hitfall"){
            const HitState & state = environment.getCharacter().getHitState();
            return RuntimeValue(state.fall.fall);
        }

        /* the mugen docs don't say anything about `statetime' but its
         * most likely the same thing as `time'
         */
        if (identifier == "time" || identifier == "statetime"){
            return RuntimeValue(environment.getCharacter().getStateTime());
        }

        if (identifier == "A"){
            return RuntimeValue(string("A"));
        }
        
        if (identifier == "S"){
            /* states are just strings */
            return RuntimeValue(string("S"));
        }

        if (identifier == "C"){
            return RuntimeValue(string("C"));
        }
        
        if (identifier == "L"){
            return RuntimeValue(string("L"));
        }

        if (identifier == "statetype"){
            return RuntimeValue(environment.getCharacter().getStateType());
        }

        /* true if the player has control */
        if (identifier == "ctrl"){
            return RuntimeValue(environment.getCharacter().hasControl());
        }

        if (identifier == "stateno"){
            return RuntimeValue(environment.getCharacter().getCurrentState());
        }

        if (identifier == "power"){
            return RuntimeValue(environment.getCharacter().getPower());
        }
        
        if (identifier == "internal:extra-jumps"){
            return RuntimeValue(environment.getCharacter().getExtraJumps());
        }

        if (identifier == "internal:airjump-height"){
            return RuntimeValue(environment.getCharacter().getAirJumpHeight());
        }

        if (identifier == "velocity.walk.back.x"){
            return RuntimeValue(environment.getCharacter().getWalkBackX());
        }

        if (identifier == "velocity.walk.fwd.x"){
            return RuntimeValue(environment.getCharacter().getWalkForwardX());
        }

        if (identifier == "velocity.run.fwd.x"){
            return RuntimeValue(environment.getCharacter().getRunForwardX());
        }
        
        if (identifier == "velocity.run.fwd.y"){
            return RuntimeValue(environment.getCharacter().getRunForwardY());
        }
        
        if (identifier == "velocity.jump.neu.x"){
            return RuntimeValue(environment.getCharacter().getNeutralJumpingX());
        }
        
        if (identifier == "velocity.jump.y"){
            return RuntimeValue(environment.getCharacter().getNeutralJumpingY());
        }

        if (identifier == "prevstateno"){
            return RuntimeValue(environment.getCharacter().getPreviousState());
        }
        
        if (identifier == "velocity.run.back.x"){
            return RuntimeValue(environment.getCharacter().getRunBackX());
        }
        
        if (identifier == "velocity.run.back.y"){
            return RuntimeValue(environment.getCharacter().getRunBackY());
        }

        if (identifier == "velocity.jump.back.x"){
            return RuntimeValue(environment.getCharacter().getJumpBack());
        }

        if (identifier == "velocity.jump.fwd.x"){
            return RuntimeValue(environment.getCharacter().getJumpForward());
        }

        if (identifier == "velocity.runjump.fwd.x"){
            return RuntimeValue(environment.getCharacter().getRunJumpForward());
        }

        if (identifier == "velocity.airjump.neu.x"){
            return RuntimeValue(environment.getCharacter().getAirJumpNeutralX());
        }
        
        if (identifier == "velocity.airjump.y"){
            return RuntimeValue(environment.getCharacter().getAirJumpNeutralY());
        }
        
        if (identifier == "velocity.airjump.back.x"){
            return RuntimeValue(environment.getCharacter().getAirJumpBack());
        }
        
        if (identifier == "velocity.airjump.fwd.x"){
            return RuntimeValue(environment.getCharacter().getAirJumpForward());
        }

        if (identifier == "movement.yaccel"){
            return RuntimeValue(environment.getCharacter().getGravity());
        }
#endif

        std::ostringstream out;
        out << "Don't know how to compile identifier '" << identifier.toString() << "'";
        throw MugenException(out.str());
    }
    
    virtual void onIdentifier(const Ast::Identifier & identifier){
        compiled = compileIdentifier(identifier);
    }

    Value * compiled;
};

}
        
Value::Value(){
}

Value::~Value(){
}

/* Ast object -> Compiled object
 * Caller must delete the result at some point.
 */
Value * compile(Ast::Value * input){
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

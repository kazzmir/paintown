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
#include "stage.h"
#include "util/funcs.h"
#include "util/regex.h"
#include <math.h>
#include <sstream>
#include <string>
#include "config.h"

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
        class JustString: public Value {
        public:
            JustString(const std::string & value):
                value(value){
                }

            std::string value;

            RuntimeValue evaluate(const Environment & environment) const {
                return RuntimeValue(value);
            }
        };
	
	if (identifier == "e"){
	    return compile(exp(1));
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
            };

            return new IsHelper();
        }
        
        if (identifier == "numhelper"){
            /* FIXME */
            return compile(0);
        }

        if (identifier == "numenemy"){
            /* FIXME: return more than 1 in team mode */
            return compile(1);
        }

        if (identifier == "numpartner"){
            /* FIXME: return 1 in team mode */
            return compile(1);
        }

        if (identifier == "p1name" ||
            identifier == "name"){
            class P1Name: public Value{
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getName());
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
            };

            return new P4Name();
        }

        if (identifier == "authorname"){
            class AuthorName: public Value{
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getAuthor());
                }
            };

            return new AuthorName();
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

        if (identifier == "backedgebodydist" ||
            identifier == "backedgedist"){
            class BackEdgeBodyDist: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    int x = environment.getCharacter().getBackX();
                    return RuntimeValue(PaintownUtil::min(x - environment.getStage().maximumLeft(),
                                                          environment.getStage().maximumRight() - x));
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
            };

            return new FrontEdgeBodyDist();
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
                    return environment.getStage().countMyEffects(&environment.getCharacter());
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

        /* TODO: these things might be able to go away. Check if the comparison
         * operation can be parsed as a function
         */
        if (identifier == "A"){
            return new JustString("A");
        }
        
        if (identifier == "S"){
            return new JustString("S");
        }

        if (identifier == "C"){
            return new JustString("C");
        }
        
        if (identifier == "L"){
            return new JustString("L");
        }

        if (identifier == "SCA"){
            return new JustString("SCA");
        }

        if (identifier == "SAC"){
            return new JustString("SAC");
        }
        /* end things that might go away */

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

        if (identifier == "HitPauseTime"){
            class HitPauseTime: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().pauseTime());
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
            };

            return new StateNo();
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

        if (identifier == "p2life"){
            class P2Life: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    const Character * enemy = environment.getStage().getEnemy(&environment.getCharacter());
                    return RuntimeValue(enemy->getHealth());
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
            };

            return new LifeMax();
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

        if (identifier == "prevstateno"){
            class PrevStateNo: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getPreviousState());
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
        out << "Don't know how to compile identifier '" << identifier.toString() << "'";
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

                RuntimeValue evaluate(const Environment & environment) const {
                    /* FIXME */
                    return RuntimeValue(0);
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

    Value * compileString(const Ast::String & string_value){
        class JustString: public Value {
        public:
            JustString(const std::string & what):
                value(what){
                }

            RuntimeValue value;

            RuntimeValue evaluate(const Environment & environment) const {
                return value;
            }
        };

        std::string out;
        string_value >> out;
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

            RuntimeValue value;

            RuntimeValue evaluate(const Environment & environment) const {
                return value;
            }
        };

        double x;
        number >> x;
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
            MetaCircularArg1(double (*c_level)(double), Value * argument):
                cFunction(c_level),
                argument(argument){
                }

            double (*cFunction)(double);
            Value * argument;

            virtual ~MetaCircularArg1(){
                delete argument;
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
                compiled(false){
                }

                Value * compiled;

                Value * compileConst(const Ast::Identifier & identifier){
		    if (identifier == "data.life"){
                        class DataLife: public Value {
                        public:
                            RuntimeValue evaluate(const Environment & environment) const {
                                return RuntimeValue(environment.getCharacter().getMaxHealth());
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
                        class MovemntYAccel: public Value {
                        public:
                            RuntimeValue evaluate(const Environment & environment) const {
                                return RuntimeValue(environment.getCharacter().getGravity());
                            }
                        };

                        return new MovemntYAccel();
                    }
                    
                    /* FIXME others
		     *  movement.stand.friction: Returns value of the "stand.friction" parameter. (float)
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

                    if (identifier == "velocity.runjump.back.x"){
                        class RunBackJumpX: public Value {
                        public:
                            RuntimeValue evaluate(const Environment & environment) const {
                                return RuntimeValue(environment.getCharacter().getRunJumpBack());
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
            return new MetaCircularArg1(asin, compile(function.getArg1()));
        }
        
        if (function == "sin"){
            return new MetaCircularArg1(sin, compile(function.getArg1()));
        }
        
        if (function == "atan"){
            return new MetaCircularArg1(atan, compile(function.getArg1()));
        }
        
        if (function == "tan"){
            return new MetaCircularArg1(tan, compile(function.getArg1()));
        }

        if (function == "abs"){
            return new MetaCircularArg1(fabs, compile(function.getArg1()));
        }
        
        if (function == "exp"){
	    return new MetaCircularArg1(exp, compile(function.getArg1()));
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

                RuntimeValue evaluate(const Environment & environment) const {
                    /* FIXME */
                    return RuntimeValue(0);
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
            } else if (var == "yveladd"){
                /* TODO */
            } else if (var == "type"){
                /* TODO */
            } else if (var == "animtype"){
                class HitVarAnimType: public HitVar {
                public:
                    RuntimeValue evaluate(const Environment & environment) const {
                        return RuntimeValue(state(environment).animationType);
                    }
                };

                return new HitVarAnimType();
            } else if (var == "airtype"){
                class HitVarAirType: public HitVar {
                public:
                    RuntimeValue evaluate(const Environment & environment) const {
                        return RuntimeValue(state(environment).airType);
                    }
                };

                return new HitVarAirType();
            } else if (var == "groundtype"){
                class HitVarGroundType: public HitVar {
                public:
                    RuntimeValue evaluate(const Environment & environment) const {
                        return RuntimeValue(state(environment).groundType);
                    }
                };

                return new HitVarGroundType();
            } else if (var == "damage"){
                class HitVarDamage: public HitVar {
                public:
                    RuntimeValue evaluate(const Environment & environment) const {
                        return RuntimeValue(state(environment).damage);
                    }
                };

                return new HitVarDamage();
            } else if (var == "hitcount"){
                class HitVarHitCount: public HitVar {
                public:
                    RuntimeValue evaluate(const Environment & environment) const {
                        return RuntimeValue(state(environment).hits);
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
                };
                return new HitVarShakeTime();
            } else if (var == "hittime"){
                class HitVarHitTime: public HitVar {
                public:
                    RuntimeValue evaluate(const Environment & environment) const {
                        return RuntimeValue(state(environment).hitTime);
                    }
                };

                return new HitVarHitTime();
            } else if (var == "slidetime"){
                class HitVarSlideTime: public HitVar {
                public:
                    RuntimeValue evaluate(const Environment & environment) const {
                        return RuntimeValue(state(environment).slideTime);
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
                };

                return new HitVarXVel();
            } else if (var == "yvel"){
                class HitVarYVel: public HitVar {
                public:
                    RuntimeValue evaluate(const Environment & environment) const {
                        return RuntimeValue(state(environment).yVelocity);
                    }
                };

                return new HitVarYVel();
            } else if (var == "yaccel"){
                class HitVarYAccel: public HitVar {
                public:
                    RuntimeValue evaluate(const Environment & environment) const {
                        return RuntimeValue(state(environment).yAcceleration);
                    }
                };

                return new HitVarYAccel();
            } else if (var == "hitid"){
                /* TODO */
            } else if (var == "chainid"){
                /* TODO */
            } else if (var == "guarded"){
                class HitVarGuarded: public HitVar {
                public:
                    RuntimeValue evaluate(const Environment & environment) const {
                        return RuntimeValue(state(environment).guarded);
                    }
                };

                return new HitVarGuarded();
            } else if (var == "fall"){
                class HitVarFall: public HitVar {
                public:
                    RuntimeValue evaluate(const Environment & environment) const {
                        return RuntimeValue(state(environment).fall.fall);
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
                };

                return new HitVarFallXVel();
            } else if (var == "fall.yvel"){
                class HitVarFallYVel: public HitVar {
                public:
                    RuntimeValue evaluate(const Environment & environment) const {
                        return RuntimeValue(state(environment).fall.yVelocity);
                    }
                };

                return new HitVarFallYVel();
            } else if (var == "fall.recover"){
                class HitVarFallRecover: public HitVar {
                public:
                    RuntimeValue evaluate(const Environment & environment) const {
                        return RuntimeValue(state(environment).fall.recover);
                    }
                };

                return new HitVarFallRecover();

            } else if (var == "fall.time"){
                /* TODO */
            } else if (var == "fall.recovertime"){
                /* TODO */
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

                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(ceil(argument->evaluate(environment).toNumber()));
                }
            };

            return new Ceil(compile(function.getArg1()));
        }
        
        if (function == "acos"){
            return new MetaCircularArg1(acos, compile(function.getArg1()));
	}
        
        if (function == "cos"){
            return new MetaCircularArg1(cos, compile(function.getArg1()));
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

                virtual ~FunctionAnimElemTime(){
                    delete index;
                }

                RuntimeValue evaluate(const Environment & environment) const {
                    int index = (int) this->index->evaluate(environment).toNumber();
                    MugenAnimation * animation = environment.getCharacter().getCurrentAnimation();
                    return RuntimeValue(animation->animationElementElapsed(index));
                }
            };

            return new FunctionAnimElemTime(compile(function.getArg1()));
        }

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
                        /* handle the second argument of animelem here */
                        return RuntimeValue(animation->getTicks() == 0);
                    }
                    return RuntimeValue(false);

                }
            };
            return new FunctionAnimElem(compile(function.getArg1()));
            // return RuntimeValue(environment.getCharacter().getCurrentAnimation()->getPosition() + 1 == index == 0);
        }

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
                        /* handle the second argument of animelem here */
                        return RuntimeValue(animation->getTicks() == 0);
                    }
                    return RuntimeValue(false);

                }
            };
            return new FunctionAnimElem(compile(function.getArg1()));
            // return RuntimeValue(environment.getCharacter().getCurrentAnimation()->getPosition() + 1 == index == 0);
        }

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

                RuntimeValue evaluate(const Environment & environment) const {
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
            };

            return new VelY();
        }
        
        if (keyword == "hitvel x"){
            class HitVelX: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(- environment.getCharacter().getHitState().xVelocity);
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
            };

            return new ScreenPosY();
        }

        if (keyword == "screenpos x"){
            class ScreenPosX: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getX());
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
            };

            return new PosY();
        }

        if (keyword == "pos x"){
            class PosX: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    return RuntimeValue(environment.getCharacter().getX());
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
            };

            return new P2BodyDistY();
        }

        if (keyword == "parentdist x"){
            class ParentDistX: public Value {
            public:
                RuntimeValue evaluate(const Environment & environment) const {
                    /* FIXME */
                    return RuntimeValue(0);
                }
            };

            return new ParentDistX();
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
            };

            return new P2DistY();
        }

        if (keyword == "rootdist x"){
            /* This trigger is only valid for helper-type characters. RootDist returns the distance from the helper to its root. The root is the main player character who owns the helper: for instance, if you select Kumquat to play with, and Kumquat spawns a helper named Kiwi, who in turn spawns a helper named Penguin, then Penguin's root is Kumquat, and Penguin is a descendant of Kumquat. RootDist works similarly to P2Dist.
             * For comparing the Y-distance, RootDist gives the difference in the heights of the players' Y-axes. A negative value means that the root is above its descendant.
             *
             * For comparing the X-distance, ParentDist gives the X-distance of the root's axis from the descendant's axis. A positive value indicates the root is in front of its descendant.*/
            /* FIXME! */
            return compile(0);
        }
        
        if (keyword == "rootdist y"){
            /* FIXME! */
            return compile(0);
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

            std::string toString() const {
                std::ostringstream out;
                out << left->toString();
                switch (type){
                    case ExpressionInfix::GreaterThanEquals : {
                        out << " >= ";
                        break;
                    }
                    case ExpressionInfix::GreaterThan : {
                        out << " > ";
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
                        return RuntimeValue((int) left->evaluate(environment).toNumber() % (int) right->evaluate(environment).toNumber());
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
    Ast::Number number(immediate);
    return CompileWalker::compileNumber(number);
}

Value * compile(double immediate){
    Ast::Number number(immediate);
    return CompileWalker::compileNumber(number);
}

}
}

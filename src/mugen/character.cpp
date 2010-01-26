#include "util/bitmap.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <string>
#include <cstring>
#include <vector>
#include <ostream>
#include <sstream>
#include <iostream>

// To aid in filesize
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "util/funcs.h"
#include "util/font.h"
#include "util/file-system.h"
#include "factory/font_render.h"

#include "mugen_animation.h"
#include "mugen_item.h"
#include "mugen_item_content.h"
#include "mugen_section.h"
#include "character.h"
#include "mugen_sound.h"
#include "mugen_reader.h"
#include "mugen_sprite.h"
#include "mugen_util.h"
#include "mugen_stage.h"
#include "globals.h"
#include "state.h"
#include "evaluator.h"

#include "input/input-map.h"
#include "input/input-manager.h"

#include "parser/all.h"
#include "ast/all.h"

using namespace std;

namespace Mugen{

namespace StateType{

std::string Stand = "S";
std::string Crouch = "C";
std::string Air = "A";
std::string LyingDown = "L";

}

namespace Move{

std::string Attack = "A";
std::string Idle = "I";
std::string Hit = "H";

}

namespace AttackType{
    std::string Normal = "N";
    std::string Special = "S";
    std::string Hyper = "H";
}

namespace PhysicalAttack{
    std::string Normal = "A";
    std::string Throw = "T";
    std::string Projectile = "P";
}

namespace PaintownUtil = ::Util;

StateController::StateController(const string & name):
type(Unknown),
name(name),
changeControl(false),
control(false),
value1(NULL),
value2(NULL){
}

StateController::~StateController(){
    for (map<int, vector<Ast::Value*> >::iterator it = triggers.begin(); it != triggers.end(); it++){
        vector<Ast::Value*> values = (*it).second;
        for (vector<Ast::Value*>::iterator value_it = values.begin(); value_it != values.end(); value_it++){
            Ast::Value * value = *value_it;
            delete value;
        }
    }
    
    for (map<int, Ast::Value*>::iterator it = variables.begin(); it != variables.end(); it++){
        Ast::Value * value = (*it).second;
        delete value;
    }

    for (map<int, Ast::Value*>::iterator it = systemVariables.begin(); it != systemVariables.end(); it++){
        Ast::Value * value = (*it).second;
        delete value;
    }

    delete value1;
    delete value2;
}

void StateController::setValue1(Ast::Value * value){
    this->value1 = value;
}

void StateController::setValue2(Ast::Value * value){
    this->value2 = value;
}

void StateController::addTriggerAll(Ast::Value * trigger){
    triggers[-1].push_back(trigger);
}

void StateController::addTrigger(int number, Ast::Value * trigger){
    triggers[number].push_back(trigger);
}
    
void StateController::addVariable(int number, Ast::Value * variable){
    if (variables[number] != 0){
        delete variables[number];
    }
    variables[number] = variable;
}
    
void StateController::addSystemVariable(int number, Ast::Value * variable){
    if (systemVariables[number] != 0){
        delete systemVariables[number];
    }

    systemVariables[number] = variable;
}

bool StateController::canTrigger(const Character & character, const Ast::Value * expression, const vector<string> & commands) const {
    RuntimeValue result = evaluate(expression, Environment(character, commands));
    /* might throw an exception */
    return toBool(result);
}

bool StateController::canTrigger(const Character & character, const vector<Ast::Value*> & expressions, const vector<string> & commands) const {
    for (vector<Ast::Value*>::const_iterator it = expressions.begin(); it != expressions.end(); it++){
        const Ast::Value * value = *it;
        if (!canTrigger(character, value, commands)){
            return false;
        }
    }
    return true;
}

vector<int> StateController::sortTriggers() const {
    vector<int> out;

    for (map<int, vector<Ast::Value*> >::const_iterator it = triggers.begin(); it != triggers.end(); it++){
        int number = it->first;
        /* ignore triggerall (-1) */
        if (number != -1){
            out.push_back(number);
        }
    }

    sort(out.begin(), out.end());

    return out;
}

bool StateController::canTrigger(const Character & character, const vector<string> & commands) const {
    if (triggers.find(-1) != triggers.end()){
        vector<Ast::Value*> values = triggers.find(-1)->second;
        /* if the triggerall fails then no triggers will work */
        if (!canTrigger(character, values, commands)){
            return false;
        }
    }

    vector<int> keys = sortTriggers();
    for (vector<int>::iterator it = keys.begin(); it != keys.end(); it++){
        vector<Ast::Value*> values = triggers.find(*it)->second;
        /* if a trigger succeeds then stop processing and just return true */
        if (canTrigger(character, values, commands)){
            return true;
        }
    }

    return false;
}

void StateController::activate(Character & guy, const vector<string> & commands) const {
    Global::debug(1) << "Activate controller " << name << endl;

    if (changeControl){
        guy.setControl(control);
    }

    switch (getType()){
        case AfterImage : {
            break;
        }
        case AfterImageTime : {
            break;
        }
        case AllPalFX : {
            break;
        }
        case AngleAdd : {
            break;
        }
        case AngleDraw : {
            break;
        }
        case AngleMul : {
            break;
        }
        case AngleSet : {
            break;
        }
        case AppendToClipboard : {
            break;
        }
        case AssertSpecial : {
            break;
        }
        case AttackDist : {
            break;
        }
        case AttackMulSet : {
            break;
        }
        case BGPalFX : {
            break;
        }
        case BindToParent : {
            break;
        }
        case BindToRoot : {
            break;
        }
        case BindToTarget : {
            break;
        }
        case ChangeAnim : {
            RuntimeValue result = evaluate(value1, Environment(guy));
            if (result.isDouble()){
                int value = (int) result.getDoubleValue();
                guy.setAnimation(value);
            }
            break;
        }
        case ChangeAnim2 : {
            break;
        }
        case ChangeState : {
            RuntimeValue result = evaluate(value1, Environment(guy));
            if (result.isDouble()){
                int value = (int) result.getDoubleValue();
                guy.changeState(value, commands);
            }
            break;
        }
        case ClearClipboard : {
            break;
        }
        case CtrlSet : {
            RuntimeValue result = evaluate(value1, Environment(guy));
            guy.setControl(toBool(result));
            break;
        }
        case DefenceMulSet : {
            break;
        }
        case DestroySelf : {
            break;
        }
        case DisplayToClipboard : {
            break;
        }
        case EnvColor : {
            break;
        }
        case EnvShake : {
            break;
        }
        case Explod : {
            break;
        }
        case ExplodBindTime : {
            break;
        }
        case ForceFeedback : {
            break;
        }
        case FallEnvShake : {
            break;
        }
        case GameMakeAnim : {
            break;
        }
        case Gravity : {
            break;
        }
        case Helper : {
            break;
        }
        case HitAdd : {
            break;
        }
        case HitBy : {
            break;
        }
        case HitDef : {
            guy.setHitDef(getHit());
            guy.nextTicket();
            break;
        }
        case HitFallDamage : {
            break;
        }
        case HitFallSet : {
            break;
        }
        case HitFallVel : {
            break;
        }
        case HitOverride : {
            break;
        }
        case HitVelSet : {
            if (value1 != NULL){
                RuntimeValue result = evaluate(value1, Environment(guy));
                if (result.getBoolValue()){
                    guy.setXVelocity(guy.getHitState().xVelocity);
                }
            }

            if (value2 != NULL){
                RuntimeValue result = evaluate(value2, Environment(guy));
                if (result.getBoolValue()){
                    guy.setYVelocity(guy.getHitState().yVelocity);
                }
            }
            break;
        }
        case LifeAdd : {
            break;
        }
        case LifeSet : {
            break;
        }
        case MakeDust : {
            break;
        }
        case ModifyExplod : {
            break;
        }
        case MoveHitReset : {
            break;
        }
        case NotHitBy : {
            break;
        }
        case Null : {
            break;
        }
        case Offset : {
            break;
        }
        case PalFX : {
            break;
        }
        case ParentVarAdd : {
            break;
        }
        case ParentVarSet : {
            break;
        }
        case Pause : {
            break;
        }
        case PlayerPush : {
            break;
        }
        case PlaySnd : {
            try{
                /* FIXME: group could start with F */
                int group;
                int item;
                value1->reset();
                *value1 >> group >> item;
                MugenSound * sound = guy.getSound(group, item);
                if (sound != 0){
                    sound->play();
                } else {
                    Global::debug(0) << "Error with PlaySnd " << name << ": no sound for " << group << ", " << item << endl;
                }
            } catch (const Ast::Exception & e){
                Global::debug(0) << "Error with PlaySnd " << name << ": " << e.getReason() << endl;
            }
            break;
        }
        case PosAdd : {
            if (value1 != NULL){
                RuntimeValue result = evaluate(value1, Environment(guy));
                if (result.isDouble()){
                    guy.moveX(result.getDoubleValue());
                    // guy.setX(guy.getX() + result.getDoubleValue());
                }
            }
            if (value2 != NULL){
                RuntimeValue result = evaluate(value2, Environment(guy));
                if (result.isDouble()){
                    guy.moveYNoCheck(-result.getDoubleValue());
                    // guy.setY(guy.getY() + result.getDoubleValue());
                }
            }
            break;
        }
        case PosFreeze : {
            break;
        }
        case PosSet : {
            if (value1 != NULL){
                RuntimeValue result = evaluate(value1, Environment(guy));
                if (result.isDouble()){
                    guy.setX(result.getDoubleValue());
                }
            }
            if (value2 != NULL){
                RuntimeValue result = evaluate(value2, Environment(guy));
                if (result.isDouble()){
                    guy.setY(result.getDoubleValue());
                }
            }
            break;
        }
        case PowerAdd : {
            break;
        }
        case PowerSet : {
            break;
        }
        case Projectile : {
            break;
        }
        case RemoveExplod : {
            break;
        }
        case ReversalDef : {
            break;
        }
        case ScreenBound : {
            break;
        }
        case SelfState : {
            break;
        }
        case SprPriority : {
            break;
        }
        case StateTypeSet : {
            guy.setMoveType(moveType);
            break;
        }
        case SndPan : {
            break;
        }
        case StopSnd : {
            break;
        }
        case SuperPause : {
            break;
        }
        case TargetBind : {
            break;
        }
        case TargetDrop : {
            break;
        }
        case TargetFacing : {
            break;
        }
        case TargetLifeAdd : {
            break;
        }
        case TargetPowerAdd : {
            break;
        }
        case TargetState : {
            break;
        }
        case TargetVelAdd : {
            break;
        }
        case TargetVelSet : {
            break;
        }
        case Trans : {
            break;
        }
        case Turn : {
            break;
        }
        case VarAdd : {
            break;
        }
        case VarRandom : {
            break;
        }
        case VarRangeSet : {
            break;
        }
        case VarSet : {
            for (map<int, Ast::Value*>::const_iterator it = variables.begin(); it != variables.end(); it++){
                int index = (*it).first;
                Ast::Value * value = (*it).second;
                guy.setVariable(index, value);
            }

            for (map<int, Ast::Value*>::const_iterator it = systemVariables.begin(); it != systemVariables.end(); it++){
                int index = (*it).first;
                Ast::Value * value = (*it).second;
                guy.setSystemVariable(index, value);
            }

            break;
        }
        case VelAdd : {
            if (value1 != NULL){
                RuntimeValue result = evaluate(value1, Environment(guy));
                if (result.isDouble()){
                    guy.setXVelocity(guy.getXVelocity() + result.getDoubleValue());
                }
            }
            if (value2 != NULL){
                RuntimeValue result = evaluate(value2, Environment(guy));
                if (result.isDouble()){
                    guy.setYVelocity(guy.getYVelocity() + result.getDoubleValue());
                }
            }
            break;
        }
        case VelMul : {
            if (value1 != NULL){
                RuntimeValue result = evaluate(value1, Environment(guy));
                if (result.isDouble()){
                    guy.setXVelocity(guy.getXVelocity() * result.getDoubleValue());
                }
            }

            if (value2 != NULL){
                RuntimeValue result = evaluate(value1, Environment(guy));
                if (result.isDouble()){
                    guy.setYVelocity(guy.getYVelocity() * result.getDoubleValue());
                }
            }
            break;
        }
        case VelSet : {
            if (value1 != NULL){
                RuntimeValue result = evaluate(value1, Environment(guy));
                if (result.isDouble()){
                    guy.setXVelocity(result.getDoubleValue());
                }
            }
            if (value2 != NULL){
                RuntimeValue result = evaluate(value2, Environment(guy));
                if (result.isDouble()){
                    guy.setYVelocity(result.getDoubleValue());
                }
            }
            break;
        }
        case Width : {
            break;
        }
        case Unknown : {
            break;
        }
    }
}

State::State():
animation(0),
changeControl(false),
control(0),
changeVelocity(false),
changePhysics(false),
changePower(false),
moveType(Move::Idle){
}

void State::addController(StateController * controller){
    controllers.push_back(controller);
}

void State::setVelocity(double x, double y){
    changeVelocity = true;
    velocity_x = x;
    velocity_y = y;
}

void State::setPower(int power){
    powerAdd = power;
    changePower = true;
}
    
void State::setMoveType(const std::string & type){
    moveType = type;
}
    
void State::setPhysics(Physics::Type p){
    changePhysics = true;
    physics = p;
}

void State::transitionTo(Character & who){
    if (animation != 0){
        who.setAnimation((int) toNumber(evaluate(animation, Environment(who))));
    }

    if (changeControl){
        who.setControl(toBool(evaluate(control, Environment(who))));
    }

    who.setMoveType(moveType);

    if (changeVelocity){
        who.setXVelocity(velocity_x);
        who.setYVelocity(velocity_y);
    }

    if (changePhysics){
        who.setCurrentPhysics(physics);
    }

    switch (type){
        case Standing : {
            who.setStateType(StateType::Stand);
            break;
        }
        case Crouching : {
            who.setStateType(StateType::Crouch);
            break;
        }
        case Air : {
            who.setStateType(StateType::Air);
            break;
        }
        case LyingDown : {
            who.setStateType(StateType::LyingDown);
            break;
        }
        case Unchanged : {
            break;
        }
    }
}

State::~State(){
    for (vector<StateController*>::iterator it = controllers.begin(); it != controllers.end(); it++){
        delete (*it);
    }

    delete animation;
    delete control;
}
    
Command::Command(std::string name, Ast::KeyList * keys, int maxTime, int bufferTime):
name(name),
keys(keys),
maxTime(maxTime),
bufferTime(bufferTime),
ticks(-1),
holdKey(-1),
current(keys->getKeys().begin()),
holder(0),
successTime(0),
needRelease(0){
}

bool Command::handle(InputMap<Keys>::Output keys){
    class KeyWalker: public Ast::Walker{
    public:
        KeyWalker(InputMap<Keys>::Output & keys, const InputMap<Keys>::Output & oldKeys, int & holdKey, const Ast::Key *& holder, const Ast::Key *& needRelease):
        ok(false),
        fail(false),
        holdKey(holdKey),
        keys(keys),
        oldKeys(keys),
        holder(holder),
        needRelease(needRelease){
        }
        
        bool ok;
        bool fail;
        int & holdKey;
        InputMap<Keys>::Output & keys;
        const InputMap<Keys>::Output & oldKeys;
        const Ast::Key *& holder;
        const Ast::Key *& needRelease;

        virtual void onKeySingle(const Ast::KeySingle & key){
            if (key == "a"){
                ok = keys[A];
            } else if (key == "b"){
                ok = keys[B];
            } else if (key == "c"){
                ok = keys[C];
            } else if (key == "x"){
                ok = keys[X];
            } else if (key == "y"){
                ok = keys[Y];
            } else if (key == "z"){
                ok = keys[Z];
            } else if (key == "B"){
                ok = keys[Back];
            } else if (key == "DB"){
                ok = keys[Back] && keys[Down];
            } else if (key == "D"){
                ok = keys[Down];
            } else if (key == "DF"){
                ok = keys[Forward] && keys[Down];
            } else if (key == "F"){
                ok = keys[Forward];
            } else if (key == "UF"){
                ok = keys[Forward] && keys[Up];
            } else if (key == "U"){
                ok = keys[Up];
            } else if (key == "UB"){
                ok = keys[Back] && keys[Up];
            }
            if (ok){
                needRelease = &key;
            }
        }

        bool sameKeys(const InputMap<Keys>::Output & map1, const InputMap<Keys>::Output & map2 ){
            return map1 == map2;
        }

        virtual void onKeyModifier(const Ast::KeyModifier & key){
            needRelease = NULL;
            switch (key.getModifierType()){
                case Ast::KeyModifier::MustBeHeldDown : {
                    key.getKey()->walk(*this);
                    if (ok){
                        holder = key.getKey();
                    }
                    break;
                }
                case Ast::KeyModifier::Release : {
                    if (key.getExtra() > 0){
                        if (holdKey > 0){
                            int fake = -1;
                            const Ast::Key * fakeKey;
                            KeyWalker walker(keys, oldKeys, fake, holder, fakeKey);
                            key.getKey()->walk(walker);
                            if (walker.ok){
                                holdKey -= 1;
                            } else {
                                fail = true;
                            }
                        } else if (holdKey == 0){
                            int fake = -1;
                            const Ast::Key * fakeKey;
                            KeyWalker walker(keys, oldKeys, fake, holder, fakeKey);
                            key.getKey()->walk(walker);
                            /* if ok is true then the key is still being pressed */
                            if (!walker.ok){
                                ok = true;
                            }
                        } else if (holdKey == -1){
                            holdKey = key.getExtra();
                        }
                    } else {
                        int fake = -1;
                        KeyWalker walker(keys, oldKeys, fake, holder, needRelease);
                        key.getKey()->walk(walker);
                        if (!walker.ok){
                            ok = true;
                        }
                    }
                    break;
                }
                case Ast::KeyModifier::Direction : {
                    key.getKey()->walk(*this);
                    break;
                }
                case Ast::KeyModifier::Only : {
                    key.getKey()->walk(*this);
                    if (!ok){
                        if (!sameKeys(keys, oldKeys)){
                            fail = true;
                        }
                    }
                    break;
                }
            }
        }

        virtual void onKeyCombined(const Ast::KeyCombined & key){
            int fake = -1;
            KeyWalker left(keys, oldKeys, fake, holder, needRelease);
            KeyWalker right(keys, oldKeys, fake, holder, needRelease);
            key.getKey1()->walk(left);
            key.getKey2()->walk(right);
            ok = left.ok && right.ok;
            if (ok){
                needRelease = &key;
            }
        }
    };
    
    if (successTime > 0){
        successTime -= 1;
        Global::debug(1) << "Pressed " << name << endl;
        return true;
    }

    bool use = true;
    if (needRelease != NULL && *needRelease == *(*current)){
        const Ast::Key * fake;
        KeyWalker walker(keys, oldKeys, holdKey, holder, fake);
        needRelease->walk(walker);
        Global::debug(1) << "Waiting for key " << needRelease->toString() << " to be released: " << walker.ok << endl;

        if (walker.ok){
            /* if the key is still held down then don't continue */
            use = false;
        } else {
            /* otherwise the key is released so reset the release key */
            needRelease = NULL;
        }
    } else {
        needRelease = NULL;
    }

    bool fail = false;
    bool ok = false;

    if (use){
        KeyWalker walker(keys, oldKeys, holdKey, holder, needRelease);
        (*current)->walk(walker);

        ok = walker.ok;
        fail = walker.fail;
        /*
        if (name == "a"){
            Global::debug(0) << "Tried move " << (*current)->toString() << " was " << ok << endl;
        }
        */
        if (holder != 0){
            holder->walk(walker);
            ok &= walker.ok;
            fail |= walker.fail;
        }
    }

    oldKeys = keys;
    if (ticks > 0){
        ticks += 1;
        if (ticks > maxTime){
            fail = true;
            /*
            if (name == "a"){
                Global::debug(0) << name << " ran out of time" << endl;
            }
            */
        }
    }

    // Global::debug(0) << "Command " << name << " at ticks " << ticks << endl;

    if (fail){
        current = this->keys->getKeys().begin();
        ticks = -1;
        // needRelease = NULL;
        holdKey = -1;
        holder = 0;
    } else if (ok){
        current++;
        if (ticks == -1){
            ticks = 1;
        }
        holdKey = -1;
        if (current == this->keys->getKeys().end()){
            /* success! */
            current = this->keys->getKeys().begin();
            ticks = -1;
            // needRelease = NULL;
            holder = 0;
            successTime = bufferTime - 1;
            Global::debug(1) << "Pressed " << name << endl;
            return true;
        }
    }

    return false;
}

Command::~Command(){
    delete keys;
}


void HitState::update(const HitDefinition & hit){
    /* FIXME: choose the proper ground/air/guard types */

    shakeTime = hit.pause.player2;
    hitTime = hit.groundHitTime;
    slideTime = hit.groundSlideTime;
    animationType = hit.animationType;
    groundType = hit.groundType;
    yAcceleration = hit.yAcceleration;
    xVelocity = hit.groundVelocity.x;
    yVelocity = hit.groundVelocity.y;
    fall.fall = hit.fall.fall;
    fall.yVelocity = hit.fall.yVelocity;

    // Global::debug(0) << "Hit definition: shake time " << shakeTime << " hit time " << hitTime << endl;
}

Character::Character( const string & s ):
ObjectAttack(0){
    this->location = s;
    initialize();
}

Character::Character( const char * location ):
ObjectAttack(0){
    this->location = std::string(location);
    initialize();
}

Character::Character( const string & s, int alliance ):
ObjectAttack(alliance){
    this->location = s;
    initialize();
}

Character::Character( const string & s, const int x, const int y, int alliance ):
ObjectAttack(x,y,alliance){
    this->location = s;
    initialize();
}

Character::Character( const Character & copy ):
ObjectAttack(copy){
}

Character::~Character(){
     // Get rid of sprites
    for (std::map< unsigned int, std::map< unsigned int, MugenSprite * > >::iterator i = sprites.begin() ; i != sprites.end() ; ++i ){
      for( std::map< unsigned int, MugenSprite * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
	  if( j->second )delete j->second;
      }
    }
    
     // Get rid of bitmaps
    for( std::map< unsigned int, std::map< unsigned int, Bitmap * > >::iterator i = bitmaps.begin() ; i != bitmaps.end() ; ++i ){
      for( std::map< unsigned int, Bitmap * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
	  if( j->second )delete j->second;
      }
    }
    
    // Get rid of animation lists;
    for( std::map< int, MugenAnimation * >::iterator i = animations.begin() ; i != animations.end() ; ++i ){
	if( i->second )delete i->second;
    }
    
    // Get rid of sounds
    for( std::map< unsigned int, std::map< unsigned int, MugenSound * > >::iterator i = sounds.begin() ; i != sounds.end() ; ++i ){
      for( std::map< unsigned int, MugenSound * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
	  if( j->second )delete j->second;
      }
    }

    for (vector<Command*>::iterator it = commands.begin(); it != commands.end(); it++){
        delete (*it);
    }
        
    for (map<int, State*>::iterator it = states.begin(); it != states.end(); it++){
        State * state = (*it).second;
        delete state;
    }
}

void Character::initialize(){
    currentState = Standing;
    moveType = Move::Idle;
    previousState = currentState;
    stateType = StateType::Stand;
    currentAnimation = Standing;
    lieDownTime = 0;
    debug = false;
    has_control = true;

    lastTicket = 0;
    
    /* Load up info for the select screen */
    //loadSelectData();

    /* provide sensible defaults */
    walkfwd = 0;
    walkback = 0;
    runbackx = 0;
    runbacky = 0;
    runforwardx = 0;
    runforwardy = 0;
    power = 0;

    velocity_x = 0;
    velocity_y = 0;

    gravity = 0.1;
    standFriction = 1;

    stateTime = 0;
}

void Character::loadSelectData(){
    /* Load up info for the select screen */
    try{
	baseDir = Filesystem::find("mugen/chars/" + location + "/");
	Global::debug(1) << baseDir << endl;
	std::string str = Mugen::Util::stripDir(this->location);
	const std::string ourDefFile = Mugen::Util::fixFileName(baseDir, std::string(str + ".def"));
	
	if (ourDefFile.empty()){
	    Global::debug(1) << "Cannot locate player definition file for: " << location << endl;
	}
	
	// Set name of character
	this->name = Mugen::Util::probeDef(ourDefFile, "info", "name");
	this->displayName = Mugen::Util::probeDef(ourDefFile, "info", "displayname");
	this->sffFile = Mugen::Util::probeDef(ourDefFile, "files", "sprite");
	// Get necessary sprites 9000 & 9001 for select screen
	this->sprites[9000][0] = Mugen::Util::probeSff( Mugen::Util::fixFileName(baseDir, this->sffFile), 9000,0);
	this->sprites[9000][1] = Mugen::Util::probeSff( Mugen::Util::fixFileName(baseDir, this->sffFile), 9000,1);
	
    } catch (const MugenException &ex){
	Global::debug(1) << "Couldn't grab details for character!" << endl;
	Global::debug(1) << "Error was: " << ex.getReason() << endl;
    }
}
    
void Character::addCommand(Command * command){
    commands.push_back(command);
}

void Character::setAnimation(int animation){
    currentAnimation = animation;
    getCurrentAnimation()->reset();
}

void Character::loadCmdFile(const string & path){
    string full = Filesystem::find(baseDir + path);
    try{
        int defaultTime = 15;
        int defaultBufferTime = 1;

        Ast::AstParse parsed((list<Ast::Section*>*) Mugen::Cmd::main(full));
        for (Ast::AstParse::section_iterator section_it = parsed.getSections()->begin(); section_it != parsed.getSections()->end(); section_it++){
            Ast::Section * section = *section_it;
            std::string head = section->getName();
            /* this should really be head = Mugen::Util::fixCase(head) */
            head = Util::fixCase(head);

            if (head == "command"){
                class CommandWalker: public Ast::Walker {
                public:
                    CommandWalker(Character & self, const int defaultTime, const int defaultBufferTime):
                        self(self),
                        time(defaultTime),
                        bufferTime(defaultBufferTime),
                        key(0){
                        }

                    Character & self;
                    int time;
                    int bufferTime;
                    string name;
                    Ast::Key * key;

                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "name"){
                            simple >> name;
                        } else if (simple == "command"){
                            key = (Ast::Key*) simple.getValue()->copy();
                        } else if (simple == "time"){
                            simple >> time;
                        } else if (simple == "buffer.time"){
                            simple >> bufferTime;
                            /* Time that the command will be buffered for. If the command is done
                             * successfully, then it will be valid for this time. The simplest
                             * case is to set this to 1. That means that the command is valid
                             * only in the same tick it is performed. With a higher value, such
                             * as 3 or 4, you can get a "looser" feel to the command. The result
                             * is that combos can become easier to do because you can perform
                             * the command early.
                             */
                        }
                    }

                    virtual ~CommandWalker(){
                        if (name == ""){
                            throw MugenException("No name given for command");
                        }

                        if (key == 0){
                            throw MugenException("No key sequence given for command");
                        }

                        /* parser guarantees the key will be a KeyList */
                        self.addCommand(new Command(name, (Ast::KeyList*) key, time, bufferTime));
                    }
                };

                CommandWalker walker(*this, defaultTime, defaultBufferTime);
                section->walk(walker);
            } else if (head == "defaults"){
                class DefaultWalker: public Ast::Walker {
                public:
                    DefaultWalker(int & time, int & buffer):
                        time(time),
                        buffer(buffer){
                        }

                    int & time;
                    int & buffer;

                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "command.time"){
                            simple >> time;
                        } else if (simple == "command.buffer.time"){
                            simple >> buffer;
                        }
                    }
                };

                DefaultWalker walker(defaultTime, defaultBufferTime);
                section->walk(walker);
            } else if (PaintownUtil::matchRegex(head, "statedef")){
                parseStateDefinition(section);
            } else if (PaintownUtil::matchRegex(head, "state ")){
                parseState(section);
            }

            /* [Defaults]
             * ; Default value for the "time" parameter of a Command. Minimum 1.
             * command.time = 15
             *
             * ; Default value for the "buffer.time" parameter of a Command. Minimum 1,
             * ; maximum 30.
             * command.buffer.time = 1
             */
        }
    } catch (const Mugen::Cmd::ParseException & e){
        Global::debug(0) << "Could not parse " << path << endl;
        Global::debug(0) << e.getReason() << endl;
    }
}

static bool isStateDefSection(string name){
    name = Util::fixCase(name);
    return PaintownUtil::matchRegex(name, "state ") ||
           PaintownUtil::matchRegex(name, "statedef ");
}

bool Character::canBeHit(Character * enemy){
    return moveType != Move::Hit && lastTicket < enemy->getTicket();
}
    
void Character::setConstant(std::string name, const vector<double> & values){
    constants[name] = Constant(values);
}

void Character::setConstant(std::string name, double value){
    constants[name] = Constant(value);
}
        
void Character::setVariable(int index, Ast::Value * value){
    variables[index] = value;
}
        
Ast::Value * Character::getVariable(int index) const {
    map<int, Ast::Value*>::const_iterator found = variables.find(index);
    if (found != variables.end()){
        return (*found).second;
    }
    return 0;
}
        
void Character::setSystemVariable(int index, Ast::Value * value){
    variables[index] = value;
}

Ast::Value * Character::getSystemVariable(int index) const {
    map<int, Ast::Value*>::const_iterator found = variables.find(index);
    if (found != variables.end()){
        return (*found).second;
    }
    return 0;
}
        
void Character::resetStateTime(){
    stateTime = 0;
}
        
void Character::changeState(int stateNumber, const vector<string> & inputs){
    Global::debug(1) << "Change to state " << stateNumber << endl;
    previousState = currentState;
    currentState = stateNumber;
    resetStateTime();
    if (states[currentState] != 0){
        State * state = states[currentState];
        state->transitionTo(*this);
        doStates(inputs, currentState);
    } else {
        Global::debug(0) << "Unknown state " << currentState << endl;
    }
}

void Character::loadCnsFile(const string & path){
    string full = Filesystem::find(baseDir + path);
    try{
        /* cns can use the Cmd parser */
        Ast::AstParse parsed((list<Ast::Section*>*) Mugen::Cmd::main(full));
        for (Ast::AstParse::section_iterator section_it = parsed.getSections()->begin(); section_it != parsed.getSections()->end(); section_it++){
            Ast::Section * section = *section_it;
            std::string head = section->getName();
            /* this should really be head = Mugen::Util::fixCase(head) */
            head = Util::fixCase(head);
            if (false && !isStateDefSection(head)){
                /* I dont think this is the right way to do it */
                class AttributeWalker: public Ast::Walker {
                public:
                    AttributeWalker(Character & who):
                    self(who){
                    }

                    Character & self;

                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        string name = simple.idString();
                        if (simple.getValue() != 0 && simple.getValue()->hasMultiple()){
                            vector<double> values;
                            simple >> values;
                            self.setConstant(name, values);
                        } else {
                            double value;
                            simple >> value;
                            self.setConstant(name, value);
                        }
                    }
                };

                AttributeWalker walker(*this);
                section->walk(walker);
            } else if (head == "velocity"){
                class VelocityWalker: public Ast::Walker {
                public:
                    VelocityWalker(Character & who):
                    self(who){
                    }

                    Character & self;

                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "walk.fwd"){
                            double speed;
                            simple >> speed;
                            self.setWalkForwardX(speed);
                        } else if (simple == "walk.back"){
                            double speed;
                            simple >> speed;
                            self.setWalkBackX(speed);
                        } else if (simple == "run.fwd"){
                            double x, y;
                            simple >> x >> y;
                            self.setRunForwardX(x);
                            self.setRunForwardY(y);
                        } else if (simple == "run.back"){
                            double x, y;
                            simple >> x >> y;
                            self.setRunBackX(x);
                            self.setRunBackY(y);
                        } else if (simple == "jump.neu"){
                            double x, y;
                            simple >> x >> y;
                            self.setNeutralJumpingX(x);
                            self.setNeutralJumpingY(y);
                        } else if (simple == "jump.back"){
                            double speed;
                            simple >> speed;
                            self.setJumpBack(speed);
                        } else if (simple == "jump.fwd"){
                            double speed;
                            simple >> speed;
                            self.setJumpForward(speed);
                        } else if (simple == "runjump.back"){
                            double speed;
                            simple >> speed;
                            self.setRunJumpBack(speed);
                        } else if (simple == "runjump.fwd"){
                            double speed;
                            simple >> speed;
                            self.setRunJumpForward(speed);
                        } else if (simple == "airjump.neu"){
                            double x, y;
                            simple >> x >> y;
                            self.setAirJumpNeutralX(x);
                            self.setAirJumpNeutralY(y);
                        } else if (simple == "airjump.back"){
                            double speed;
                            simple >> speed;
                            self.setAirJumpBack(speed);
                        } else if (simple == "airjump.fwd"){
                            double speed;
                            simple >> speed;
                            self.setAirJumpForward(speed);
                        }
                    }
                };

                VelocityWalker walker(*this);
                section->walk(walker);
            } else if (head == "data"){
                class DataWalker: public Ast::Walker {
                public:
                    DataWalker(Character & who):
                    self(who){
                    }
                    
                    Character & self;

                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "liedown.time"){
                            int x;
                            simple >> x;
                            self.setLieDownTime(x);
                        }
                    }

                };
                
                DataWalker walker(*this);
                section->walk(walker);
            } else if (head == "size"){
                class SizeWalker: public Ast::Walker {
                public:
                    SizeWalker(Character & self):
                        self(self){
                        }

                    Character & self;

                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "height"){
                            int x;
                            simple >> x;
                            self.setHeight(x);
                        }
                    }
                };
                
                SizeWalker walker(*this);
                section->walk(walker);

            } else if (head == "movement"){
                class MovementWalker: public Ast::Walker {
                public:
                    MovementWalker(Character & who):
                    self(who){
                    }

                    Character & self;

                    virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                        if (simple == "yaccel"){
                            double n;
                            simple >> n;
                            self.setGravity(n);
                        } else if (simple == "stand.friction"){
                            double n;
                            simple >> n;
                            self.setStandingFriction(n);
                        }
                    }
                };

                MovementWalker walker(*this);
                section->walk(walker);
            }
        }
    } catch (const Mugen::Cmd::ParseException & e){
        Global::debug(0) << "Could not parse " << path << endl;
        Global::debug(0) << e.getReason() << endl;
    } catch (const Ast::Exception & e){
        Global::debug(0) << "Could not parse " << path << endl;
        Global::debug(0) << e.getReason() << endl;
    }
}

void Character::parseStateDefinition(Ast::Section * section){
    std::string head = section->getName();
    /* this should really be head = Mugen::Util::fixCase(head) */
    head = Util::fixCase(head);

    int state = atoi(PaintownUtil::captureRegex(head, "statedef *(-?[0-9]+)", 0).c_str());
    class StateWalker: public Ast::Walker {
        public:
            StateWalker(State * definition):
                definition(definition){
                }

            State * definition;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "type"){
                    string type;
                    simple >> type;
                    if (type == "S"){
                        definition->setType(State::Standing);
                    } else if (type == "C"){
                        definition->setType(State::Crouching);
                    } else if (type == "A"){
                        definition->setType(State::Air);
                    } else if (type == "L"){
                        definition->setType(State::LyingDown);
                    } else if (type == "U"){
                        definition->setType(State::Unchanged);
                    } else {
                        ostringstream out;
                        out << "Unknown statedef type: '" << type << "'";
                        throw MugenException(out.str());
                    }
                } else if (simple == "movetype"){
                    string type;
                    simple >> type;
                    definition->setMoveType(type);
                } else if (simple == "physics"){
                    string type;
                    simple >> type;
                    if (type == "S"){
                        definition->setPhysics(Physics::Stand);
                    } else if (type == "N"){
                        definition->setPhysics(Physics::None);
                    } else if (type == "C"){
                        definition->setPhysics(Physics::Crouch);
                    } else if (type == "A"){
                        definition->setPhysics(Physics::Air);
                    }
                } else if (simple == "anim"){
                    definition->setAnimation((Ast::Value*) simple.getValue()->copy());
                } else if (simple == "velset"){
                    double x, y;
                    simple >> x >> y;
                    definition->setVelocity(x, y);
                } else if (simple == "ctrl"){
                    definition->setControl((Ast::Value*) simple.getValue()->copy());
                } else if (simple == "poweradd"){
                    int power;
                    simple >> power;
                    definition->setPower(power);
                } else if (simple == "juggle"){
                } else if (simple == "facep2"){
                } else if (simple == "hitdefpersist"){
                } else if (simple == "movehitpersist"){
                } else if (simple == "hitcountpersist"){
                } else if (simple == "sprpriority"){
                } else {
                    Global::debug(0) << "Unhandled statedef attribute: " << simple.toString() << endl;
                }
            }
    };

    State * definition = new State();
    StateWalker walker(definition);
    section->walk(walker);
    if (states[state] != 0){
        Global::debug(1) << "Overriding state " << state << endl;
        delete states[state];
    }
    Global::debug(1) << "Adding state definition " << state << endl;
    states[state] = definition;
}

void Character::parseState(Ast::Section * section){
    std::string head = section->getName();
    /* this should really be head = Mugen::Util::fixCase(head) */
    head = Util::fixCase(head);

    int state = atoi(PaintownUtil::captureRegex(head, "state *(-?[0-9]+)", 0).c_str());
    string name = PaintownUtil::captureRegex(head, "state *-?[0-9]+ *, *(.*)", 0);

    class StateControllerWalker: public Ast::Walker {
        public:
            StateControllerWalker(StateController * controller):
                controller(controller){
                }

            StateController * controller;

            string toString(char x){
                ostringstream out;
                out << x;
                return out.str();
            }

            virtual void onAttributeArray(const Ast::AttributeArray & simple){
                if (simple == "var"){
                    int index = simple.getIndex();
                    const Ast::Value * value = simple.getValue();
                    controller->addVariable(index, (Ast::Value*) value->copy());
                } else if (simple == "sysvar"){
                    int index = simple.getIndex();
                    const Ast::Value * value = simple.getValue();
                    controller->addSystemVariable(index, (Ast::Value*) value->copy());
                }
            }

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "type"){
                    string type;
                    simple >> type;
                    type = Mugen::Util::fixCase(type);
                    map<string, StateController::Type> types;
                    types["afterimage"] = StateController::AfterImage;
                    types["afterimagetime"] = StateController::AfterImageTime;
                    types["allpalfx"] = StateController::AllPalFX;
                    types["angleadd"] = StateController::AngleAdd;
                    types["angledraw"] = StateController::AngleDraw;
                    types["anglemul"] = StateController::AngleMul;
                    types["angleset"] = StateController::AngleSet;
                    types["appendtoclipboard"] = StateController::AppendToClipboard;
                    types["assertspecial"] = StateController::AssertSpecial;
                    types["attackdist"] = StateController::AttackDist;
                    types["attackmulset"] = StateController::AttackMulSet;
                    types["bgpalfx"] = StateController::BGPalFX;
                    types["bindtoparent"] = StateController::BindToParent;
                    types["bindtoroot"] = StateController::BindToRoot;
                    types["bindtotarget"] = StateController::BindToTarget;
                    types["changeanim"] = StateController::ChangeAnim;
                    types["changeanim2"] = StateController::ChangeAnim2;
                    types["changestate"] = StateController::ChangeState;
                    types["clearclipboard"] = StateController::ClearClipboard;
                    types["ctrlset"] = StateController::CtrlSet;
                    types["defencemulset"] = StateController::DefenceMulSet;
                    types["destroyself"] = StateController::DestroySelf;
                    types["displaytoclipboard"] = StateController::DisplayToClipboard;
                    types["envcolor"] = StateController::EnvColor;
                    types["envshake"] = StateController::EnvShake;
                    types["explod"] = StateController::Explod;
                    types["explodbindtime"] = StateController::ExplodBindTime;
                    types["forcefeedback"] = StateController::ForceFeedback;
                    types["fallenvshake"] = StateController::FallEnvShake;
                    types["gamemakeanim"] = StateController::GameMakeAnim;
                    types["gravity"] = StateController::Gravity;
                    types["helper"] = StateController::Helper;
                    types["hitadd"] = StateController::HitAdd;
                    types["hitby"] = StateController::HitBy;
                    types["hitdef"] = StateController::HitDef;
                    types["hitfalldamage"] = StateController::HitFallDamage;
                    types["hitfallset"] = StateController::HitFallSet;
                    types["hitfallvel"] = StateController::HitFallVel;
                    types["hitoverride"] = StateController::HitOverride;
                    types["hitvelset"] = StateController::HitVelSet;
                    types["lifeadd"] = StateController::LifeAdd;
                    types["lifeset"] = StateController::LifeSet;
                    types["makedust"] = StateController::MakeDust;
                    types["modifyexplod"] = StateController::ModifyExplod;
                    types["movehitreset"] = StateController::MoveHitReset;
                    types["nothitby"] = StateController::NotHitBy;
                    types["null"] = StateController::Null;
                    types["offset"] = StateController::Offset;
                    types["palfx"] = StateController::PalFX;
                    types["parentvaradd"] = StateController::ParentVarAdd;
                    types["parentvarset"] = StateController::ParentVarSet;
                    types["pause"] = StateController::Pause;
                    types["playerpush"] = StateController::PlayerPush;
                    types["playsnd"] = StateController::PlaySnd;
                    types["posadd"] = StateController::PosAdd;
                    types["posfreeze"] = StateController::PosFreeze;
                    types["posset"] = StateController::PosSet;
                    types["poweradd"] = StateController::PowerAdd;
                    types["powerset"] = StateController::PowerSet;
                    types["projectile"] = StateController::Projectile;
                    types["removeexplod"] = StateController::RemoveExplod;
                    types["reversaldef"] = StateController::ReversalDef;
                    types["screenbound"] = StateController::ScreenBound;
                    types["selfstate"] = StateController::SelfState;
                    types["sprpriority"] = StateController::SprPriority;
                    types["statetypeset"] = StateController::StateTypeSet;
                    types["sndpan"] = StateController::SndPan;
                    types["stopsnd"] = StateController::StopSnd;
                    types["superpause"] = StateController::SuperPause;
                    types["targetbind"] = StateController::TargetBind;
                    types["targetdrop"] = StateController::TargetDrop;
                    types["targetfacing"] = StateController::TargetFacing;
                    types["targetlifeadd"] = StateController::TargetLifeAdd;
                    types["targetpoweradd"] = StateController::TargetPowerAdd;
                    types["targetstate"] = StateController::TargetState;
                    types["targetveladd"] = StateController::TargetVelAdd;
                    types["targetvelset"] = StateController::TargetVelSet;
                    types["trans"] = StateController::Trans;
                    types["turn"] = StateController::Turn;
                    types["varadd"] = StateController::VarAdd;
                    types["varrandom"] = StateController::VarRandom;
                    types["varrangeset"] = StateController::VarRangeSet;
                    types["varset"] = StateController::VarSet;
                    types["veladd"] = StateController::VelAdd;
                    types["velmul"] = StateController::VelMul;
                    types["velset"] = StateController::VelSet;
                    types["width"] = StateController::Width;

                    if (types.find(type) != types.end()){
                        map<string, StateController::Type>::iterator what = types.find(type);
                        controller->setType((*what).second);
                    } else {
                        Global::debug(0) << "Unknown state controller type " << type << endl;
                    }
                } else if (simple == "value"){
                    controller->setValue1((Ast::Value*) simple.getValue()->copy());
                } else if (simple == "triggerall"){
                    controller->addTriggerAll((Ast::Value*) simple.getValue()->copy());
                } else if (PaintownUtil::matchRegex(simple.idString(), "trigger[0-9]+")){
                    int trigger = atoi(PaintownUtil::captureRegex(simple.idString(), "trigger([0-9]+)", 0).c_str());
                    controller->addTrigger(trigger, (Ast::Value*) simple.getValue()->copy());
                } else if (simple == "x"){
                    controller->setValue1((Ast::Value*) simple.getValue()->copy());
                } else if (simple == "y"){
                    controller->setValue2((Ast::Value*) simple.getValue()->copy());
                } else if (simple == "movetype"){
                    string type;
                    simple >> type;
                    controller->setMoveType(type);
                } else if (simple == "ctrl"){
                    bool control;
                    simple >> control;
                    controller->setControl(control);
                } else if (simple == "attr"){
                    string type, attack;
                    simple >> type >> attack;
                    if (type == StateType::Stand ||
                        type == StateType::Crouch ||
                        type == StateType::Air){
                        controller->getHit().attribute.state = type;
                    }

                    if (attack.size() >= 2){
                        string type = toString(attack[0]);
                        string physics = toString(attack[1]);
                        if (type == AttackType::Normal ||
                            type == AttackType::Special ||
                            type == AttackType::Hyper){
                            controller->getHit().attribute.attackType = type;
                        }
                        if (physics == PhysicalAttack::Normal ||
                            physics == PhysicalAttack::Throw ||
                            physics == PhysicalAttack::Projectile){
                            controller->getHit().attribute.physics = physics;
                        }
                    }
                } else if (simple == "hitflag"){
                    string flags;
                    simple >> flags;
                    controller->getHit().hitFlag = flags;
                } else if (simple == "guardflag"){
                    string flags;
                    simple >> flags;
                    controller->getHit().guardFlag = flags;
                } else if (simple == "animtype"){
                    string anim;
                    simple >> anim;
                    anim = Util::fixCase(anim);
                    if (anim == "light"){
                        controller->getHit().animationType = AttackType::Light;
                    } else if (anim == "medium" || anim == "med"){
                        controller->getHit().animationType = AttackType::Medium;
                    } else if (anim == "hard"){
                        controller->getHit().animationType = AttackType::Hard;
                    } else if (anim == "back"){
                        controller->getHit().animationType = AttackType::Back;
                    } else if (anim == "up"){
                        controller->getHit().animationType = AttackType::Up;
                    } else if (anim == "diagup"){
                        controller->getHit().animationType = AttackType::DiagonalUp;
                    } else {
                        Global::debug(0) << "Unknown hitdef animation type " << anim << endl;
                    }
                } else if (simple == "air.animtype"){
                    string anim;
                    simple >> anim;
                    controller->getHit().animationTypeAir = anim;
                } else if (simple == "fall.animtype"){
                    string anim;
                    simple >> anim;
                    controller->getHit().animationTypeFall = anim;
                } else if (simple == "priority"){
                    int hit;
                    simple >> hit;
                    controller->getHit().priority.hit = hit;
                    try{
                        string type;
                        simple >> type;
                        controller->getHit().priority.type = type;
                    } catch (const Ast::Exception & e){
                    }
                } else if (simple == "damage"){
                    try{
                        simple >> controller->getHit().damage.damage;
                        simple >> controller->getHit().damage.guardDamage;
                    } catch (const Ast::Exception & e){
                    }
                } else if (simple == "pausetime"){
                    try{
                        simple >> controller->getHit().pause.player1;
                        simple >> controller->getHit().pause.player2;
                    } catch (const Ast::Exception & e){
                    }
                } else if (simple == "guard.pausetime"){
                    try{
                        simple >> controller->getHit().guardPause.player1;
                        simple >> controller->getHit().guardPause.player2;
                    } catch (const Ast::Exception & e){
                    }
                } else if (simple == "sparkno"){
                    simple >> controller->getHit().spark;
                } else if (simple == "guard.sparkno"){
                    simple >> controller->getHit().guardSpark;
                } else if (simple == "sparkxy"){
                    try{
                        simple >> controller->getHit().sparkPosition.x;
                        simple >> controller->getHit().sparkPosition.y;
                    } catch (const Ast::Exception & e){
                    }
                } else if (simple == "hitsound"){
                    string first;
                    bool own = false;
                    int group;
                    int item;
                    simple >> first >> item;
                    if (first[0] == 'S'){
                        own = true;
                        group = atoi(first.substr(1).c_str());
                    } else {
                        group = atoi(first.c_str());
                    }
                    controller->getHit().hitSound.own = own;
                    controller->getHit().hitSound.group = group;
                    controller->getHit().hitSound.item = item;
                } else if (simple == "guard.hitsound"){
                    string first;
                    bool own = false;
                    int group;
                    int item;
                    simple >> first >> item;
                    if (first[0] == 'S'){
                        own = true;
                        group = atoi(first.substr(1).c_str());
                    } else {
                        group = atoi(first.c_str());
                    }
                    controller->getHit().guardHitSound.own = own;
                    controller->getHit().guardHitSound.group = group;
                    controller->getHit().guardHitSound.item = item;
                } else if (simple == "ground.type"){
                    string type;
                    simple >> type;
                    type = Util::fixCase(type);
                    if (type == "low"){
                        controller->getHit().groundType = AttackType::Low;
                    } else if (type == "high"){
                        controller->getHit().groundType = AttackType::High;
                    } else if (type == "trip"){
                        controller->getHit().groundType = AttackType::Trip;
                    }
                } else if (simple == "air.type"){
                    simple >> controller->getHit().airType;
                } else if (simple == "ground.slidetime"){
                    simple >> controller->getHit().groundSlideTime;
                } else if (simple == "guard.slidetime"){
                    simple >> controller->getHit().guardSlideTime;
                } else if (simple == "ground.hittime"){
                    simple >> controller->getHit().groundHitTime;
                } else if (simple == "guard.hittime"){
                    simple >> controller->getHit().guardGroundHitTime;
                } else if (simple == "air.hittime"){
                    simple >> controller->getHit().airHitTime;
                } else if (simple == "guard.ctrltime"){
                    simple >> controller->getHit().guardControlTime;
                } else if (simple == "guard.dist"){
                    simple >> controller->getHit().guardDistance;
                } else if (simple == "yaccel"){
                    simple >> controller->getHit().yAcceleration;
                } else if (simple == "ground.velocity"){
                    if (simple.getValue()->hasMultiple()){
                        try{
                            simple >> controller->getHit().groundVelocity.x;
                            simple >> controller->getHit().groundVelocity.y;
                        } catch (const Ast::Exception & e){
                        }
                    } else {
                            simple >> controller->getHit().groundVelocity.x;
                    }
                } else if (simple == "guard.velocity"){
                    simple >> controller->getHit().guardVelocity;
                } else if (simple == "air.velocity"){
                    if (simple.getValue()->hasMultiple()){
                        try{
                            simple >> controller->getHit().airVelocity.x;
                            simple >> controller->getHit().airVelocity.y;
                        } catch (const Ast::Exception & e){
                        }
                    } else {
                        simple >> controller->getHit().airVelocity.x;
                    }
                } else if (simple == "airguard.velocity"){
                    try{
                        simple >> controller->getHit().airGuardVelocity.x;
                        simple >> controller->getHit().airGuardVelocity.y;
                    } catch (const Ast::Exception & e){
                    }
                } else if (simple == "ground.cornerpush.veloff"){
                    simple >> controller->getHit().groundCornerPushoff;
                } else if (simple == "air.cornerpush.veloff"){
                    simple >> controller->getHit().airCornerPushoff;
                } else if (simple == "down.cornerpush.veloff"){
                    simple >> controller->getHit().downCornerPushoff;
                } else if (simple == "guard.cornerpush.veloff"){
                    simple >> controller->getHit().guardCornerPushoff;
                } else if (simple == "airguard.cornerpush.veloff"){
                    simple >> controller->getHit().airGuardCornerPushoff;
                } else if (simple == "airguard.ctrltime"){
                    simple >> controller->getHit().airGuardControlTime;
                } else if (simple == "air.juggle"){
                    simple >> controller->getHit().airJuggle;
                } else if (simple == "mindist"){
                    simple >> controller->getHit().minimum.x;
                    simple >> controller->getHit().minimum.y;
                } else if (simple == "maxdist"){
                    simple >> controller->getHit().maximum.x;
                    simple >> controller->getHit().maximum.y;
                } else if (simple == "snap"){
                    simple >> controller->getHit().snap.x;
                    simple >> controller->getHit().snap.y;
                } else if (simple == "p1sprpriority"){
                    simple >> controller->getHit().player1SpritePriority;
                } else if (simple == "p2sprpriority"){
                    simple >> controller->getHit().player2SpritePriority;
                } else if (simple == "p1facing"){
                    simple >> controller->getHit().player1Facing;
                } else if (simple == "p1getp2facing"){
                    simple >> controller->getHit().player1GetPlayer2Facing;
                } else if (simple == "player2Facing"){
                    simple >> controller->getHit().player2Facing;
                } else if (simple == "p1stateno"){
                    simple >> controller->getHit().player1State;
                } else if (simple == "p2stateno"){
                    simple >> controller->getHit().player2State;
                } else if (simple == "p2getp1state"){
                    simple >> controller->getHit().player2GetPlayer1State;
                } else if (simple == "forcestand"){
                    simple >> controller->getHit().forceStand;
                } else if (simple == "fall"){
                    simple >> controller->getHit().fall.fall;
                } else if (simple == "fall.xvelocity"){
                    simple >> controller->getHit().fall.xVelocity;
                } else if (simple == "fall.yvelocity"){
                    simple >> controller->getHit().fall.yVelocity;
                } else if (simple == "fall.recover"){
                    simple >> controller->getHit().fall.recover;
                } else if (simple == "fall.recovertime"){
                    simple >> controller->getHit().fall.recoverTime;
                } else if (simple == "fall.damage"){
                    simple >> controller->getHit().fall.damage;
                } else if (simple == "air.fall"){
                    simple >> controller->getHit().fall.airFall;
                } else if (simple == "forcenofall"){
                    simple >> controller->getHit().fall.forceNoFall;
                } else {
                    Global::debug(0) << "Unhandled state controller '" << controller->getName() << "' attribute: " << simple.toString() << endl;
                }
            }
    };

    StateController * controller = new StateController(name);
    StateControllerWalker walker(controller);
    section->walk(walker);

    if (states[state] == 0){
        ostringstream out;
        out << "No StateDef for state " << state << " [" << name << "]";
        delete controller;
        throw MugenException(out.str());
    }

    states[state]->addController(controller);

    Global::debug(1) << "Adding state controller '" << name << "' to state " << state << endl;

}

static string findStateFile(const string & base, const string & path){
    try{
        return Filesystem::find(base + "/" + path);
    } catch (const Filesystem::NotFound & f){
        return Filesystem::find("mugen/data/" + path);
    }
}

void Character::loadStateFile(const std::string & base, const string & path){
    string full = findStateFile(base, path);
    // string full = Filesystem::find(base + "/" + PaintownUtil::trim(path));
    try{
        /* st can use the Cmd parser */
        Ast::AstParse parsed((list<Ast::Section*>*) Mugen::Cmd::main(full));
        for (Ast::AstParse::section_iterator section_it = parsed.getSections()->begin(); section_it != parsed.getSections()->end(); section_it++){
            Ast::Section * section = *section_it;
            std::string head = section->getName();
            /* this should really be head = Mugen::Util::fixCase(head) */
            head = Util::fixCase(head);
            if (PaintownUtil::matchRegex(head, "statedef")){
                parseStateDefinition(section);
            } else if (PaintownUtil::matchRegex(head, "state ")){
                parseState(section);
            }
        }
    } catch (const Mugen::Cmd::ParseException & e){
        Global::debug(0) << "Could not parse " << path << endl;
        Global::debug(0) << e.getReason() << endl;
    }
}

/* a container for a directory and a file */
struct Location{
    Location(string base, string file):
        base(base), file(file){
        }

    string base;
    string file;
};

void Character::load(){
#if 0
    // Lets look for our def since some people think that all file systems are case insensitive
    baseDir = Filesystem::find("mugen/chars/" + location + "/");
    Global::debug(1) << baseDir << endl;
    std::string realstr = Mugen::Util::stripDir(location);
    const std::string ourDefFile = Mugen::Util::fixFileName(baseDir, std::string(realstr + ".def"));
    
    if (ourDefFile.empty()){
        throw MugenException( "Cannot locate player definition file for: " + location );
    }
#endif
    
    baseDir = Mugen::Util::getFileDir(location);
    const std::string ourDefFile = location;
     
    Ast::AstParse parsed(Util::parseDef(ourDefFile));
    /* Extract info for our first section of our stage */
    for (Ast::AstParse::section_iterator section_it = parsed.getSections()->begin(); section_it != parsed.getSections()->end(); section_it++){
        Ast::Section * section = *section_it;
	std::string head = section->getName();
        /* this should really be head = Mugen::Util::fixCase(head) */
	head = Mugen::Util::fixCase(head);

        if (head == "info"){
            class InfoWalker: public Ast::Walker {
            public:
                InfoWalker(Character & who):
                self(who){
                }

                Character & self;

                virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                    if (simple == "name"){
                        simple >> self.name;
                    } else if (simple == "displayname"){
                        simple >> self.displayName;
                    } else if (simple == "versiondate"){
                        simple >> self.versionDate;
                    } else if (simple == "mugenversion"){
                        simple >> self.mugenVersion;
                    } else if (simple == "author"){
                        simple >> self.author;
                    } else if (simple == "pal.defaults"){
                        vector<int> numbers;
                        simple >> numbers;
                        for (vector<int>::iterator it = numbers.begin(); it != numbers.end(); it++){
                            self.palDefaults.push_back((*it) - 1);
                        }
                        // Global::debug(1) << "Pal" << self.palDefaults.size() << ": " << num << endl;
                    } else throw MugenException("Unhandled option in Info Section: " + simple.toString());
                }
            };

            InfoWalker walker(*this);
            Ast::Section * section = *section_it;
            section->walk(walker);
        } else if (head == "files"){
            class FilesWalker: public Ast::Walker {
            public:
                FilesWalker(Character & self, const string & location):
                location(location),
                self(self){
                }

                vector<Location> stateFiles;
                const string & location;

                Character & self;
                virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                    if (simple == "cmd"){
                        simple >> self.cmdFile;
                        self.loadCmdFile(self.cmdFile);
                    } else if (simple == "cns"){
                        simple >> self.constantsFile;
                        self.loadCnsFile(self.constantsFile);
                    } else if (PaintownUtil::matchRegex(simple.idString(), "st[0-9]+")){
                        int num = atoi(PaintownUtil::captureRegex(simple.idString(), "st([0-9]+)", 0).c_str());
                        if (num >= 0 && num <= 12){
                            string path;
                            simple >> path;
                            stateFiles.push_back(Location(self.baseDir, path));
                            // simple >> self.stFile[num];
                        }
                    } else if (simple == "stcommon"){
                        string path;
                        simple >> path;
                        stateFiles.insert(stateFiles.begin(), Location(self.baseDir, path));
                    } else if (simple == "st"){
                        string path;
                        simple >> path;
                        stateFiles.push_back(Location(self.baseDir, path));
                    } else if (simple == "sprite"){
                        simple >> self.sffFile;
                    } else if (simple == "anim"){
                        simple >> self.airFile;
                    } else if (simple == "sound"){
                        simple >> self.sndFile;
                        Mugen::Util::readSounds( Mugen::Util::fixFileName(self.baseDir, self.sndFile ), self.sounds);
                    } else if (PaintownUtil::matchRegex(simple.idString(), "pal[0-9]+")){
                        int num = atoi(PaintownUtil::captureRegex(simple.idString(), "pal([0-9]+)", 0).c_str());
                        string what;
                        simple >> what;
                        self.palFile[num] = what;
                    } else {
                        Global::debug(0) << "Unhandled option in Files Section: " + simple.toString() << endl;
                    }
                }
            };

            FilesWalker walker(*this, location);
            Ast::Section * section = *section_it;
            section->walk(walker);

            for (vector<Location>::iterator it = walker.stateFiles.begin(); it != walker.stateFiles.end(); it++){
                Location & where = *it;
                try{
                    loadStateFile(where.base, where.file);
                } catch (const MugenException & e){
                    ostringstream out;
                    out << "Problem loading state file " << where.file << ": " << e.getReason();
                    throw MugenException(out.str());
                }
            }

            /*
            if (commonStateFile != ""){
                loadStateFile("mugen/data/", commonStateFile);
            }
            if (stateFile != ""){
                loadStateFile("mugen/chars/" + location, stateFile);
            }
            if (
            */

	} else if (head == "arcade"){
            class ArcadeWalker: public Ast::Walker {
            public:
                ArcadeWalker(Character & self):
                self(self){
                }

                Character & self;

                virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                    if (simple == "intro.storyboard"){
                        simple >> self.introFile;
                    } else if (simple == "ending.storyboard"){
                        simple >> self.endingFile;
                    } else {
                        throw MugenException("Unhandled option in Arcade Section: " + simple.toString());
                    }
                }
            };
            
            ArcadeWalker walker(*this);
            Ast::Section * section = *section_it;
            section->walk(walker);
	}
    }

    // Current palette
    if (palDefaults.empty()){
	// Correct the palette defaults
	for (unsigned int i = 0; i < palFile.size(); ++i){
	    palDefaults.push_back(i);
	}
    }
    if (palDefaults.size() < palFile.size()){
	bool setPals[palFile.size()];
	for( unsigned int i =0;i<palFile.size();++i){
	    setPals[i] = false;
	}
	// Set the ones already set
	for (unsigned int i = 0; i < palDefaults.size(); ++i){
	    setPals[palDefaults[i]] = true;
	}
	// now add the others
	for( unsigned int i =0;i<palFile.size();++i){
	    if(!setPals[i]){
		palDefaults.push_back(i);
	    }
	}
    }

    currentPalette = 0;
    Global::debug(1) << "Current pal: " << currentPalette << " | Palette File: " << palFile[palDefaults[currentPalette]] << endl;
    Global::debug(1) << "Reading Sff (sprite) Data..." << endl; 
    /* Sprites */
    Mugen::Util::readSprites( Mugen::Util::fixFileName(baseDir, sffFile), Mugen::Util::fixFileName(baseDir, palFile[palDefaults[currentPalette]]), sprites);
    Global::debug(1) << "Reading Air (animation) Data..." << endl;
    /* Animations */
    animations = Mugen::Util::loadAnimations(Mugen::Util::fixFileName(baseDir, airFile), sprites);

    fixAssumptions();

    /*
    State * state = states[-1];
    for (vector<StateController*>::const_iterator it = state->getControllers().begin(); it != state->getControllers().end(); it++){
        Global::debug(0) << "State -1: '" << (*it)->getName() << "'" << endl;
    }
    */
}
        
bool Character::hasAnimation(int index) const {
    typedef std::map< int, MugenAnimation * > Animations;
    Animations::const_iterator it = getAnimations().find(index);
    return it != getAnimations().end();
}

void Character::fixAssumptions(){
    /* need a -1 state controller that changes to state 20 if holdfwd
     * or holdback is pressed
     */

    if (states[-1] != 0){
        /* walk */
        {
            StateController * controller = new StateController("walk");
            controller->setType(StateController::ChangeState);
            controller->setValue1(new Ast::Number(WalkingForwards));
            controller->addTriggerAll(new Ast::ExpressionInfix(Ast::ExpressionInfix::Equals,
                        new Ast::SimpleIdentifier("stateno"),
                        new Ast::Number(0)));
            controller->addTriggerAll(new Ast::SimpleIdentifier("ctrl"));
            controller->addTrigger(1, new Ast::ExpressionInfix(Ast::ExpressionInfix::Equals,
                        new Ast::SimpleIdentifier("command"),
                        new Ast::String(new string("holdfwd"))));
            controller->addTrigger(2, new Ast::ExpressionInfix(Ast::ExpressionInfix::Equals,
                        new Ast::SimpleIdentifier("command"),
                        new Ast::String(new string("holdback"))));
            states[-1]->addController(controller);
        }

        /* crouch */
        {
            StateController * controller = new StateController("crouch");
            controller->setType(StateController::ChangeState);
            controller->setValue1(new Ast::Number(StandToCrouch));
            controller->addTriggerAll(new Ast::SimpleIdentifier("ctrl"));
            controller->addTriggerAll(new Ast::ExpressionInfix(Ast::ExpressionInfix::Equals,
                        new Ast::SimpleIdentifier("stateno"),
                        new Ast::Number(0)));
            controller->addTrigger(1, new Ast::ExpressionInfix(Ast::ExpressionInfix::Equals,
                        new Ast::SimpleIdentifier("command"),
                        new Ast::String(new string("holddown"))));
            states[-1]->addController(controller);
        }

        /* jump */
        {
            StateController * controller = new StateController("jump");
            controller->setType(StateController::ChangeState);
            controller->setValue1(new Ast::Number(JumpStart));
            controller->addTriggerAll(new Ast::SimpleIdentifier("ctrl"));
            controller->addTriggerAll(new Ast::ExpressionInfix(Ast::ExpressionInfix::Equals,
                        new Ast::SimpleIdentifier("statetype"),
                        new Ast::String(new string("S"))));
            controller->addTrigger(1, new Ast::ExpressionInfix(Ast::ExpressionInfix::Equals,
                        new Ast::SimpleIdentifier("command"),
                        new Ast::String(new string("holdup"))));
            states[-1]->addController(controller);
        }
    }

    /* need a 20 state controller that changes to state 0 if holdfwd
     * or holdback is not pressed
     */
    if (states[20] != 0){
        StateController * controller = new StateController("stop walking");
        controller->setType(StateController::ChangeState);
        controller->setValue1(new Ast::Number(Standing));
        controller->addTrigger(1, new Ast::ExpressionInfix(Ast::ExpressionInfix::Unequals,
                    new Ast::SimpleIdentifier("command"),
                    new Ast::String(new string("holdfwd"))));
        controller->addTrigger(1, new Ast::ExpressionInfix(Ast::ExpressionInfix::Unequals,
                    new Ast::SimpleIdentifier("command"),
                    new Ast::String(new string("holdback"))));
        states[20]->addController(controller);
    }

    /* stand after crouching */
    if (states[11] != 0){
        StateController * controller = new StateController("stop walking");
        controller->setType(StateController::ChangeState);
        controller->setValue1(new Ast::Number(CrouchToStand));
        controller->addTrigger(1, new Ast::ExpressionInfix(Ast::ExpressionInfix::Unequals,
                    new Ast::SimpleIdentifier("command"),
                    new Ast::String(new string("holddown"))));
        states[11]->addController(controller);
    }

    /* get up kids */
    if (states[Liedown] != 0){
        StateController * controller = new StateController("get up");
        controller->setType(StateController::ChangeState);
        controller->setValue1(new Ast::Number(GetUpFromLiedown));

        /* FIXME: this is totally made up */
        controller->addTrigger(1, new Ast::ExpressionInfix(Ast::ExpressionInfix::GreaterThanEquals,
                    new Ast::SimpleIdentifier("time"),
                    new Ast::Number(getLieDownTime())));

        states[Liedown]->addController(controller);
    }

    /* standing turn state */
    {
        State * turn = new State();
        turn->setType(State::Unchanged);
        turn->setAnimation(new Ast::Number(5));
        states[5] = turn;

        StateController * controller = new StateController("stand");
        controller->setType(StateController::ChangeState);
        controller->setValue1(new Ast::Number(Standing));
        controller->addTrigger(1, new Ast::ExpressionInfix(Ast::ExpressionInfix::Equals,
                    new Ast::SimpleIdentifier("animtime"),
                    new Ast::Number(0)));
        turn->addController(controller);
    }

#if 0
    /* if y reaches 0 then auto-transition to state 52.
     * probably just add a trigger to state 50
     */
    if (states[50] != 0){
        StateController * controller = new StateController("jump land");
        controller->setType(StateController::ChangeState);
        controller->setValue1(new Ast::Number(52));
        controller->addTrigger(1, new Ast::ExpressionInfix(Ast::ExpressionInfix::GreaterThanEquals,
                    new Ast::Keyword("pos y"),
                    new Ast::Number(0)));
        controller->addTrigger(1, new Ast::ExpressionInfix(Ast::ExpressionInfix::GreaterThan,
                    new Ast::Keyword("vel y"),
                    new Ast::Number(0)));
        states[50]->addController(controller);

    }
#endif
}

// Render sprite
void Character::renderSprite(const int x, const int y, const unsigned int group, const unsigned int image, Bitmap *bmp , const int flip, const double scalex, const double scaley ){
    MugenSprite *sprite = sprites[group][image];
    if (sprite){
	Bitmap *bitmap = sprite->getBitmap();//bitmaps[group][image];
	/*if (!bitmap){
	    bitmap = new Bitmap(Bitmap::memoryPCX((unsigned char*) sprite->pcx, sprite->newlength));
	    bitmaps[group][image] = bitmap;
	}*/
	const int width = (int)(bitmap->getWidth() * scalex);
	const int height =(int)(bitmap->getHeight() * scaley);
	if (flip == 1){
	    bitmap->drawStretched(x,y, width, height, *bmp);
	} else if (flip == -1){
	    // temp bitmap to flip and crap
	    Bitmap temp = Bitmap::temporaryBitmap(bitmap->getWidth(), bitmap->getHeight());
	    temp.fill(Bitmap::MaskColor);
	    bitmap->drawHFlip(0,0,temp);
	    temp.drawStretched(x-width,y, width, height, *bmp);
	}
    }
}
        
bool Character::canRecover() const {
    /* FIXME */
    return true;
    // return getY() == 0;
}

void Character::nextPalette(){
    if (currentPalette < palDefaults.size()-1){
	currentPalette++;
    } else currentPalette = 0;
    Global::debug(1) << "Current pal: " << currentPalette << " | Location: " << palDefaults[currentPalette] << " | Palette File: " << palFile[palDefaults[currentPalette]] << endl;
   /*
    // Now replace the palettes
    unsigned char pal[768];
    if (Mugen::Util::readPalette(Mugen::Util::fixFileName(baseDir, palFile[palDefaults[currentPalette]]),pal)){
	for( std::map< unsigned int, std::map< unsigned int, MugenSprite * > >::iterator i = sprites.begin() ; i != sprites.end() ; ++i ){
	    for( std::map< unsigned int, MugenSprite * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
		if( j->second ){
		    MugenSprite *sprite = j->second;
		    if ( sprite->samePalette){
			memcpy( sprite->pcx + (sprite->reallength), pal, 768);
		    } else {
			if (!(sprite->groupNumber == 9000 && sprite->imageNumber == 1)){
			    memcpy( sprite->pcx + (sprite->reallength)-768, pal, 768);
			} 
		    }
		}
	    }
	}
	// reload with new palette
	for( std::map< int, MugenAnimation * >::iterator i = animations.begin() ; i != animations.end() ; ++i ){
	    if( i->second )i->second->reloadBitmaps();
	}
    }
    */
}

void Character::priorPalette(){
    if (currentPalette > 0){
	currentPalette--;
    } else currentPalette = palDefaults.size() -1;
    Global::debug(1) << "Current pal: " << currentPalette << " | Palette File: " << palFile[palDefaults[currentPalette]] << endl;
    // Now replace the palettes
    /*unsigned char pal[768];
    if (Mugen::Util::readPalette(Mugen::Util::fixFileName(baseDir, palFile[palDefaults[currentPalette]]),pal)){
	for( std::map< unsigned int, std::map< unsigned int, MugenSprite * > >::iterator i = sprites.begin() ; i != sprites.end() ; ++i ){
	    for( std::map< unsigned int, MugenSprite * >::iterator j = i->second.begin() ; j != i->second.end() ; ++j ){
		if( j->second ){
		    MugenSprite *sprite = j->second;
		    if ( sprite->samePalette){
			memcpy( sprite->pcx + (sprite->reallength), pal, 768);
		    } else {
			if (!(sprite->groupNumber == 9000 && sprite->imageNumber == 1)){
			    memcpy( sprite->pcx + (sprite->reallength)-768, pal, 768);
			} 
		    }
		}
	    }
	}
	// Get rid of animation lists;
	for( std::map< int, MugenAnimation * >::iterator i = animations.begin() ; i != animations.end() ; ++i ){
	    if( i->second )i->second->reloadBitmaps();
	}
    }*/
}

const Bitmap * Character::getCurrentFrame() const {
    return getCurrentAnimation()->getCurrentFrame()->getSprite()->getBitmap();
}

void Character::drawReflection(Bitmap * work, int rel_x, int intensity){
    getCurrentAnimation()->renderReflection(getFacing() == Object::FACING_LEFT, true, intensity, getRX() - rel_x, getZ() + getY(), *work);
}

MugenAnimation * Character::getCurrentAnimation() const {
    typedef std::map< int, MugenAnimation * > Animations;
    Animations::const_iterator it = getAnimations().find(currentAnimation);
    if (it != getAnimations().end()){
        MugenAnimation * animation = (*it).second;
        return animation;
    }
    return NULL;
}

/* returns all the commands that are currently active */
vector<string> Character::doInput(InputMap<Command::Keys>::Output output){
    vector<string> out;

    // if (hasControl()){
        Global::debug(2) << "Commands" << endl;
        for (vector<Command*>::iterator it = commands.begin(); it != commands.end(); it++){
            Command * command = *it;
            if (command->handle(output)){
                Global::debug(2) << "command: " << command->getName() << endl;
                out.push_back(command->getName());
            }
        }
    // }

    return out;
}

bool Character::isPaused(){
    return hitState.shakeTime > 0;
}

InputMap<Command::Keys> & Character::getInput(){
    if (getFacing() == Object::FACING_RIGHT){
        return inputLeft;
    }
    return inputRight;
}

/* Inherited members */
void Character::act(std::vector<Object*, std::allocator<Object*> >*, World*, std::vector<Object*, std::allocator<Object*> >*){

    // if (hitState.shakeTime > 0 && moveType != Move::Hit){
    if (hitState.shakeTime > 0){
        hitState.shakeTime -= 1;
        return;
    }

    MugenAnimation * animation = getCurrentAnimation();
    if (animation != 0){
	/* Check debug state */
	if (debug){
	    if (!animation->showingDefense()){
		animation->toggleDefense();
	    }
	    if (!animation->showingOffense()){
		animation->toggleOffense();
	    }
	} else {
	    if (animation->showingDefense()){
		animation->toggleDefense();
	    }
	    if (animation->showingOffense()){
		animation->toggleOffense();
	    }
	}
        animation->logic();
    }

    /* redundant for now */
    if (hitState.shakeTime > 0){
        hitState.shakeTime -= 1;
    } else if (hitState.hitTime > -1){
        hitState.hitTime -= 1;
    }

    /* if shakeTime is non-zero should we update stateTime? */
    stateTime += 1;

    /* active is the current set of commands */
    vector<string> active = doInput(InputManager::getMap(getInput()));
    /* always run through the negative states */
    doStates(active, -3);
    doStates(active, -2);
    doStates(active, -1);
    doStates(active, currentState);

    /*
    while (doStates(active, currentState)){
        / * empty * /
    }
    */
}
        
void Character::didHit(Character * enemy){
    hitState.shakeTime = getHit().pause.player1;
}

void Character::wasHit(Character * enemy, const HitDefinition & hisHit){
    hitState.update(hisHit);
    setXVelocity(hitState.xVelocity);
    setYVelocity(hitState.yVelocity);
    lastTicket = enemy->getTicket();
    
    vector<string> active;
    changeState(5000, active);

    /*
    vector<string> active;
    while (doStates(active, currentState)){
    }
    */
}

/* returns true if a state change occured */
bool Character::doStates(const vector<string> & active, int stateNumber){
    int oldState = getCurrentState();
    if (states[stateNumber] != 0){
        State * state = states[stateNumber];
        for (vector<StateController*>::const_iterator it = state->getControllers().begin(); it != state->getControllers().end(); it++){
            const StateController * controller = *it;
            Global::debug(2) << "State " << stateNumber << " check state controller " << controller->getName() << endl;

#if 0
            /* more debugging */
            bool hasFF = false;
            for (vector<string>::const_iterator it = active.begin(); it != active.end(); it++){
                if (*it == "holdup"){
                    hasFF = true;
                }
            }
            if (stateNumber == -1 && controller->getName() == "jump" && hasFF){
            // if (controller->getName() == "run fwd"){
                int x = 2;
            }
            /* for debugging
            if (stateNumber == 20 && controller->getName() == "3"){
                int x = 2;
            }
            */
#endif

            try{
                if (controller->canTrigger(*this, active)){
                    /* activate may modify the current state */
                    controller->activate(*this, active);

                    if (stateNumber >= 0 && getCurrentState() != oldState){
                        return true;
                    }
                }
            } catch (const MugenException & me){
                Global::debug(0) << "Error while processing state " << stateNumber << ", " << controller->getName() << ". Error with trigger: " << me.getReason() << endl;
            }
        }
    }
    return false;
}

void Character::draw(Bitmap * work, int cameraX, int cameraY){
    const Font & font = Font::getFont(Filesystem::find("/fonts/arial.ttf"), 18, 18);
    int x = 0;
    if (getAlliance() == MugenStage::Player2Side){
        x = 640 - font.textLength("aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa") - 1;
    }
    int y = 1;
    FontRender * render = FontRender::getInstance();
    render->addMessage(font, x, y, Bitmap::makeColor(255, 255, 255), -1, "State %d Animation %d", getCurrentState(), currentAnimation);
    y += font.getHeight();
    render->addMessage(font, x, y, Bitmap::makeColor(255, 255, 255), -1, "Vx %f Vy %f", getXVelocity(), getYVelocity());
    y += font.getHeight();
    render->addMessage(font, x, y, Bitmap::makeColor(255, 255, 255), -1, "X %f Y %f", getX(), getY());
    y += font.getHeight();
    render->addMessage(font, x, y, Bitmap::makeColor(255, 255, 255), -1, "Time %d", getStateTime());
    y += font.getHeight();
    if (getMoveType() == Move::Hit){
        render->addMessage(font, x, y, Bitmap::makeColor(255, 255, 255), -1, "HitShake %d HitTime %d", getHitState().shakeTime, getHitState().hitTime);
        y += font.getHeight();
        render->addMessage(font, x, y, Bitmap::makeColor(255, 255, 255), -1, "Hit velocity x %f y %f", getHitState().xVelocity, getHitState().yVelocity);
    }

    /*
    int color = Bitmap::makeColor(255,255,255);
    font.printf( x, y, color, *work, "State %d Animation %d", 0,  getCurrentState(), currentAnimation);
    font.printf( x, y + font.getHeight() + 1, color, *work, "X %f Y %f", 0, (float) getXVelocity(), (float) getYVelocity());
    font.printf( x, y + font.getHeight() * 2 + 1, color, *work, "Time %d", 0, getStateTime());
    */

    MugenAnimation * animation = getCurrentAnimation();
    /* this should never be 0... */
    if (animation != 0){
        int x = getRX() - cameraX;
        int y = getRY() - cameraY;

        if (isPaused() && moveType == Move::Hit){
            x += PaintownUtil::rnd(3) - 1;
        }

        animation->render(getFacing() == Object::FACING_LEFT, false, x, y, *work, 0, 0);
    }
}

bool Character::canTurn() const {
    return getCurrentState() == Standing ||
           getCurrentState() == WalkingForwards ||
           getCurrentState() == WalkingBackwards;
}
        
MugenSound * Character::getSound(int group, int item) const {
    map<unsigned int, map<unsigned int, MugenSound*> >::const_iterator findGroup = sounds.find(group);
    if (findGroup != sounds.end()){
        const map<unsigned int, MugenSound*> & found = (*findGroup).second;
        map<unsigned int, MugenSound*>::const_iterator sound = found.find(item);
        if (sound != found.end()){
            return (*sound).second;
        }
    }
    return 0;
}

void Character::doTurn(){
    vector<string> active;
    changeState(5, active);
    reverseFacing();
}

void Character::grabbed(Object*){
}

void Character::unGrab(){
}

bool Character::isGrabbed(){
    return false;
}

Object* Character::copy(){
    return this;
}

const vector<MugenArea> Character::getAttackBoxes() const {
    return getCurrentAnimation()->getAttackBoxes(getFacing() == Object::FACING_LEFT);
}

const vector<MugenArea> Character::getDefenseBoxes() const {
    return getCurrentAnimation()->getDefenseBoxes(getFacing() == Object::FACING_LEFT);
}

const std::string& Character::getAttackName(){
    return getName();
}

bool Character::collision(ObjectAttack*){
    return false;
}

int Character::getDamage() const{
    return 0;
}

bool Character::isCollidable(Object*){
    return true;
}

bool Character::isGettable(){
    return false;
}

bool Character::isGrabbable(Object*){
    return true;
}

bool Character::isAttacking(){
    return false;
}

int Character::getWidth() const{
    return groundfront;
}

Network::Message Character::getCreateMessage(){
    return Network::Message();
}

void Character::getAttackCoords(int&, int&){
}

double Character::minZDistance() const{
    return 0;
}

void Character::attacked(World*, Object*, std::vector<Object*, std::allocator<Object*> >&){
}

}

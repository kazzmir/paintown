#include "ast/all.h"
#include "state-controller.h"
#include "character.h"
#include "stage.h"
#include "sound.h"
#include <sstream>
#include "exception.h"

using namespace std;

namespace Mugen{

StateController::StateController(const string & name):
type(Unknown),
name(name){
}

StateController::StateController(const string & name, Ast::Section * section):
type(Unknown),
name(name){
    class Walker: public Ast::Walker {
    public:
        Walker(StateController & controller):
            controller(controller){
            }

        StateController & controller;

        virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
            if (simple == "triggerall"){
                controller.addTriggerAll(Compiler::compile(simple.getValue()));
            } else if (PaintownUtil::matchRegex(PaintownUtil::lowerCaseAll(simple.idString()), "trigger[0-9]+")){
                int trigger = atoi(PaintownUtil::captureRegex(PaintownUtil::lowerCaseAll(simple.idString()), "trigger([0-9]+)", 0).c_str());
                controller.addTrigger(trigger, Compiler::compile(simple.getValue()));
            }
        }
    };
    Walker walker(*this);
    section->walk(walker);
}

StateController::~StateController(){
    for (map<int, vector<Compiler::Value*> >::iterator it = triggers.begin(); it != triggers.end(); it++){
        vector<Compiler::Value*> values = (*it).second;
        for (vector<Compiler::Value*>::iterator value_it = values.begin(); value_it != values.end(); value_it++){
            Compiler::Value * value = *value_it;
            delete value;
        }
    }
}

void StateController::addTriggerAll(Compiler::Value * trigger){
    triggers[-1].push_back(trigger);
}

void StateController::addTrigger(int number, Compiler::Value * trigger){
    triggers[number].push_back(trigger);
}

bool StateController::canTrigger(const MugenStage & stage, const Character & character, const Compiler::Value * expression, const vector<string> & commands) const {
    /* this makes it easy to break in gdb */
    try{
        /*
        if (debug){
            int x = 2;
            x += 1;
        }
        */
        RuntimeValue result = expression->evaluate(FullEnvironment(stage, character, commands));
        return result.toBool();
    } catch (const MugenException & e){
        ostringstream out;
        out << "Expression `" << expression << "' " << e.getReason();
        throw MugenException(out.str());
    }
}

bool StateController::canTrigger(const MugenStage & stage, const Character & character, const vector<Compiler::Value*> & expressions, const vector<string> & commands) const {
    for (vector<Compiler::Value*>::const_iterator it = expressions.begin(); it != expressions.end(); it++){
        const Compiler::Value * value = *it;
        if (!canTrigger(stage, character, value, commands)){
            // Global::debug(2*!getDebug()) << "'" << value->toString() << "' did not trigger" << endl;
            return false;
        } else {
            // Global::debug(2*!getDebug()) << "'" << value->toString() << "' did trigger" << endl;
        }
    }
    return true;
}

vector<int> StateController::sortTriggers() const {
    vector<int> out;

    for (map<int, vector<Compiler::Value*> >::const_iterator it = triggers.begin(); it != triggers.end(); it++){
        int number = it->first;
        /* ignore triggerall (-1) */
        if (number != -1){
            out.push_back(number);
        }
    }

    sort(out.begin(), out.end());

    return out;
}

bool StateController::canTrigger(const MugenStage & stage, const Character & character, const vector<string> & commands) const {
    if (triggers.find(-1) != triggers.end()){
        vector<Compiler::Value*> values = triggers.find(-1)->second;
        /* if the triggerall fails then no triggers will work */
        if (!canTrigger(stage, character, values, commands)){
            return false;
        }
    }

    vector<int> keys = sortTriggers();
    for (vector<int>::iterator it = keys.begin(); it != keys.end(); it++){
        vector<Compiler::Value*> values = triggers.find(*it)->second;
        /* if a trigger succeeds then stop processing and just return true */
        if (canTrigger(stage, character, values, commands)){
            return true;
        }
    }

    return false;
}

class ControllerChangeAnim: public StateController {
public:
    ControllerChangeAnim(Ast::Section * section, const string & name):
        StateController(name, section),
        value(NULL){
            parse(section);
        }

    Compiler::Value * value;

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(Compiler::Value *& value):
                value(value){
                }

            Compiler::Value *& value;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "value"){
                    value = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(value);
        section->walk(walker);
        if (value == NULL){
            ostringstream out;
            out << "Expected the `value' attribute for state " << name;
            throw MugenException(out.str());
        }
    }

    virtual ~ControllerChangeAnim(){
        delete value;
    }

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
        RuntimeValue result = value->evaluate(FullEnvironment(stage, guy));
        if (result.isDouble()){
            int value = (int) result.getDoubleValue();
            guy.setAnimation(value);
        }
    }
};

class ControllerChangeState: public StateController {
public:
    ControllerChangeState(Ast::Section * section, const std::string & name):
        StateController(name, section),
        value(NULL),
        control(NULL){
            parse(section);
        }

    Compiler::Value * value;
    Compiler::Value * control;

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(Compiler::Value *& value, Compiler::Value *& control):
                value(value),
                control(control){
                }

            Compiler::Value *& value;
            Compiler::Value *& control;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "value"){
                    value = Compiler::compile(simple.getValue());
                } else if (simple == "ctrl"){
                    control = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(value, control);
        section->walk(walker);
        if (value == NULL){
            ostringstream out;
            out << "Expected the `value' attribute for state " << name;
            throw MugenException(out.str());
        }
    }

    virtual ~ControllerChangeState(){
        delete value;
        delete control;
    }

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
        if (control != NULL){
            guy.setControl(control->evaluate(FullEnvironment(stage, guy)).toBool());
        }

        RuntimeValue result = value->evaluate(FullEnvironment(stage, guy));
        if (result.isDouble()){
            int value = (int) result.getDoubleValue();
            guy.changeState(stage, value, commands);
        }
    }
};

class ControllerCtrlSet: public StateController {
public:
    ControllerCtrlSet(Ast::Section * section, const string & name):
        StateController(name, section),
        value(NULL){
            parse(section);
        }

    Compiler::Value * value;

    virtual ~ControllerCtrlSet(){
        delete value;
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(Compiler::Value *& value):
                value(value){
                }

            Compiler::Value *& value;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "value"){
                    value = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(value);
        section->walk(walker);
        if (value == NULL){
            ostringstream out;
            out << "Expected the `value' attribute for state " << name;
            throw MugenException(out.str());
        }
    }

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
        RuntimeValue result = value->evaluate(FullEnvironment(stage, guy));
        guy.setControl(toBool(result));
    }
};

class ControllerPlaySound: public StateController {
public:
    ControllerPlaySound(Ast::Section * section, const string & name):
        StateController(name, section),
        group(-1),
        own(false),
        item(NULL){
            parse(section);
        }

    int group;
    bool own;
    Compiler::Value * item;

    virtual ~ControllerPlaySound(){
        delete item;
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerPlaySound & controller):
                controller(controller){
                }

            ControllerPlaySound & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "value"){
                    controller.parseSound(simple.getValue());
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void parseSound(const Ast::Value * value){
        try{
            string group;
            const Ast::Value * item;
            *value >> group >> item;
            if (PaintownUtil::matchRegex(group, "F[0-9]+")){
                int realGroup = atoi(PaintownUtil::captureRegex(group, "F([0-9]+)", 0).c_str());
                this->group = realGroup;
                this->item = Compiler::compile(item);
                own = true;
            } else if (PaintownUtil::matchRegex(group, "[0-9]+")){
                this->group = atoi(group.c_str());
                this->item = Compiler::compile(item);
                own = false;
            }
        } catch (const MugenException & e){
            // Global::debug(0) << "Error with PlaySnd " << controller.name << ": " << e.getReason() << endl;
            Global::debug(0) << "Error with PlaySnd :" << e.getReason() << endl;
        }

    }

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
        MugenSound * sound = NULL;
        if (item != NULL){
            int itemNumber = (int) item->evaluate(FullEnvironment(stage, guy)).toNumber();
            if (own){
                sound = guy.getCommonSound(group, itemNumber);
            } else {
                sound = guy.getSound(group, itemNumber);
            }
        }

        if (sound != NULL){
            sound->play();
        }
    }
};

class ControllerVarSet: public StateController {
public:
    ControllerVarSet(Ast::Section * section, const string & name):
        StateController(name, section),
        value(NULL),
        variable(NULL){
            parse(section);
        }

    Compiler::Value * value;
    Compiler::Value * variable;
    map<int, Compiler::Value*> variables;
    map<int, Compiler::Value*> floatVariables;
    map<int, Compiler::Value*> systemVariables;

    virtual ~ControllerVarSet(){
        delete value;
        delete variable;
        delete_map(variables);
        delete_map(floatVariables);
        delete_map(systemVariables);
    }

    void delete_map(const map<int, Compiler::Value*> & values){
        for (map<int, Compiler::Value*>::const_iterator it = values.begin(); it != values.end(); it++){
            Compiler::Value * value = (*it).second;
            delete value;
        }
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerVarSet & controller):
                controller(controller){
                }

            ControllerVarSet & controller;

            virtual void onAttributeArray(const Ast::AttributeArray & simple){
                if (simple == "var"){
                    int index = simple.getIndex();
                    const Ast::Value * value = simple.getValue();
                    controller.variables[index] = Compiler::compile(value);
                } else if (simple == "fvar"){
                    int index = simple.getIndex();
                    const Ast::Value * value = simple.getValue();
                    controller.floatVariables[index] = Compiler::compile(value);
                } else if (simple == "sysvar"){
                    int index = simple.getIndex();
                    const Ast::Value * value = simple.getValue();
                    controller.systemVariables[index] = Compiler::compile(value);
                }
            }

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "value"){
                    controller.value = Compiler::compile(simple.getValue());
                } else if (simple == "v"){
                    controller.variable = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
        for (map<int, Compiler::Value*>::const_iterator it = variables.begin(); it != variables.end(); it++){
            int index = (*it).first;
            Compiler::Value * value = (*it).second;
            guy.setVariable(index, value);
        }

        for (map<int, Compiler::Value*>::const_iterator it = floatVariables.begin(); it != floatVariables.end(); it++){
            int index = (*it).first;
            Compiler::Value * value = (*it).second;
            guy.setFloatVariable(index, value);
        }

        for (map<int, Compiler::Value*>::const_iterator it = systemVariables.begin(); it != systemVariables.end(); it++){
            int index = (*it).first;
            Compiler::Value * value = (*it).second;
            guy.setSystemVariable(index, value);
        }

        if (value != NULL && variable != NULL){
            /* 'value = 23' is value1
             * 'v = 9' is value2
             */
            guy.setVariable((int) variable->evaluate(FullEnvironment(stage, guy, commands)).toNumber(), value);
        }
    }
};

class ControllerVelSet: public StateController {
public:
    ControllerVelSet(Ast::Section * section, const string & name):
        StateController(name, section),
        x(NULL),
        y(NULL){
            parse(section);
        }

    Compiler::Value * x;
    Compiler::Value * y;

    virtual ~ControllerVelSet(){
        delete x;
        delete y;
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerVelSet & controller):
                controller(controller){
                }

            ControllerVelSet & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "x"){
                    controller.x = Compiler::compile(simple.getValue());
                } else if (simple == "y"){
                    controller.y = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
        if (x != NULL){
            RuntimeValue result = x->evaluate(FullEnvironment(stage, guy));
            if (result.isDouble()){
                guy.setXVelocity(result.getDoubleValue());
            }
        }
        if (y != NULL){
            RuntimeValue result = y->evaluate(FullEnvironment(stage, guy));
            if (result.isDouble()){
                guy.setYVelocity(result.getDoubleValue());
            }
        }
    }
};

class ControllerHitVelSet: public StateController {
public:
    ControllerHitVelSet(Ast::Section * section, const string & name):
        StateController(name, section),
        x(NULL),
        y(NULL){
            parse(section);
        }

    Compiler::Value * x;
    Compiler::Value * y;

    virtual ~ControllerHitVelSet(){
        delete x;
        delete y;
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerHitVelSet & controller):
                controller(controller){
                }

            ControllerHitVelSet & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "x"){
                    controller.x = Compiler::compile(simple.getValue());
                } else if (simple == "y"){
                    controller.y = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
        if (x != NULL){
            RuntimeValue result = x->evaluate(FullEnvironment(stage, guy));
            if (result.getBoolValue()){
                guy.setXVelocity(guy.getHitState().xVelocity);
            }
        }

        if (y != NULL){
            RuntimeValue result = y->evaluate(FullEnvironment(stage, guy));
            if (result.getBoolValue()){
                guy.setYVelocity(guy.getHitState().yVelocity);
            }
        }
    }

};

class ControllerPosAdd: public StateController {
public:
    ControllerPosAdd(Ast::Section * section, const string & name):
        StateController(name, section),
        x(NULL),
        y(NULL){
            parse(section);
        }

    Compiler::Value * x;
    Compiler::Value * y;

    virtual ~ControllerPosAdd(){
        delete x;
        delete y;
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerPosAdd & controller):
                controller(controller){
                }

            ControllerPosAdd & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "x"){
                    controller.x = Compiler::compile(simple.getValue());
                } else if (simple == "y"){
                    controller.y = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
        if (x != NULL){
            RuntimeValue result = x->evaluate(FullEnvironment(stage, guy));
            if (result.isDouble()){
                guy.moveX(result.getDoubleValue());
                // guy.setX(guy.getX() + result.getDoubleValue());
            }
        }
        if (y != NULL){
            RuntimeValue result = y->evaluate(FullEnvironment(stage, guy));
            if (result.isDouble()){
                guy.moveYNoCheck(-result.getDoubleValue());
                // guy.setY(guy.getY() + result.getDoubleValue());
            }
        }
    }

};

class ControllerPosSet: public StateController {
public:
    ControllerPosSet(Ast::Section * section, const string & name):
        StateController(name, section),
        x(NULL),
        y(NULL){
            parse(section);
        }

    Compiler::Value * x;
    Compiler::Value * y;

    virtual ~ControllerPosSet(){
        delete x;
        delete y;
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerPosSet & controller):
                controller(controller){
                }

            ControllerPosSet & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "x"){
                    controller.x = Compiler::compile(simple.getValue());
                } else if (simple == "y"){
                    controller.y = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
        if (x != NULL){
            RuntimeValue result = x->evaluate(FullEnvironment(stage, guy));
            if (result.isDouble()){
                guy.setX(result.getDoubleValue());
            }
        }
        if (y != NULL){
            RuntimeValue result = y->evaluate(FullEnvironment(stage, guy));
            if (result.isDouble()){
                guy.setY(result.getDoubleValue());
            }
        }
    }

};

class ControllerVelAdd: public StateController {
public:
    ControllerVelAdd(Ast::Section * section, const string & name):
        StateController(name, section),
        x(NULL),
        y(NULL){
            parse(section);
        }

    Compiler::Value * x;
    Compiler::Value * y;

    virtual ~ControllerVelAdd(){
        delete x;
        delete y;
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerVelAdd & controller):
                controller(controller){
                }

            ControllerVelAdd & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "x"){
                    controller.x = Compiler::compile(simple.getValue());
                } else if (simple == "y"){
                    controller.y = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
        if (x != NULL){
            RuntimeValue result = x->evaluate(FullEnvironment(stage, guy));
            if (result.isDouble()){
                guy.setXVelocity(guy.getXVelocity() + result.getDoubleValue());
            }
        }
        if (y != NULL){
            RuntimeValue result = y->evaluate(FullEnvironment(stage, guy));
            if (result.isDouble()){
                guy.setYVelocity(guy.getYVelocity() + result.getDoubleValue());
            }
        }
    }
};

class ControllerVelMul: public StateController {
public:
    ControllerVelMul(Ast::Section * section, const string & name):
        StateController(name, section),
        x(NULL),
        y(NULL){
            parse(section);
        }

    Compiler::Value * x;
    Compiler::Value * y;

    virtual ~ControllerVelMul(){
        delete x;
        delete y;
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerVelMul & controller):
                controller(controller){
                }

            ControllerVelMul & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "x"){
                    controller.x = Compiler::compile(simple.getValue());
                } else if (simple == "y"){
                    controller.y = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
        if (x != NULL){
            RuntimeValue result = x->evaluate(FullEnvironment(stage, guy));
            if (result.isDouble()){
                guy.setXVelocity(guy.getXVelocity() * result.getDoubleValue());
            }
        }

        if (y != NULL){
            RuntimeValue result = y->evaluate(FullEnvironment(stage, guy));
            if (result.isDouble()){
                guy.setYVelocity(guy.getYVelocity() * result.getDoubleValue());
            }
        }
    }
};

class ControllerHitDef: public StateController {
public:
    ControllerHitDef(Ast::Section * section, const string & name):
        StateController(name, section){
            parse(section);
        }

    HitDefinition hit;

    const HitDefinition & getHit() const {
        return hit;
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(HitDefinition & hit):
                hit(hit){
                }

            HitDefinition & hit;

            string toString(char x){
                ostringstream out;
                out << x;
                return out.str();
            }

            AttackType::Animation parseAnimationType(string type){
                type = Util::fixCase(type);
                if (type == "light"){
                    return AttackType::Light;
                } else if (type == "medium" || type == "med"){
                    return AttackType::Medium;
                } else if (type == "hard" || type == "heavy"){
                    return AttackType::Hard;
                } else if (type == "back"){
                    return AttackType::Back;
                } else if (type == "up"){
                    return AttackType::Up;
                } else if (type == "diagup"){
                    return AttackType::DiagonalUp;
                } else {
                    Global::debug(0) << "Unknown hitdef animation type " << type << endl;
                }
                return AttackType::NoAnimation;
            }

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "attr"){
                    string type, attack;
                    simple >> type >> attack;
                    if (type == StateType::Stand ||
                        type == StateType::Crouch ||
                        type == StateType::Air){
                        hit.attribute.state = type;
                    }

                    if (attack.size() >= 2){
                        string type = toString(attack[0]);
                        string physics = toString(attack[1]);
                        if (type == AttackType::Normal ||
                            type == AttackType::Special ||
                            type == AttackType::Hyper){
                            hit.attribute.attackType = type;
                        }
                        if (physics == PhysicalAttack::Normal ||
                            physics == PhysicalAttack::Throw ||
                            physics == PhysicalAttack::Projectile){
                            hit.attribute.physics = physics;
                        }
                    }
                } else if (simple == "hitflag"){
                    string flags;
                    simple >> flags;
                    hit.hitFlag = flags;
                } else if (simple == "guardflag"){
                    string flags;
                    simple >> flags;
                    hit.guardFlag = flags;
                } else if (simple == "animtype"){
                    string anim;
                    simple >> anim;
                    hit.animationType = parseAnimationType(anim);
                } else if (simple == "air.animtype"){
                    string anim;
                    simple >> anim;
                    hit.animationTypeAir = parseAnimationType(anim);
                } else if (simple == "debug"){
                    // setDebug(true);
                } else if (simple == "fall.animtype"){
                    string anim;
                    simple >> anim;
                    hit.animationTypeFall = parseAnimationType(anim);
                } else if (simple == "priority"){
                    int hit;
                    simple >> hit;
                    this->hit.priority.hit = hit;
                    try{
                        string type;
                        simple >> type;
                        this->hit.priority.type = type;
                    } catch (const Ast::Exception & e){
                    }
                } else if (simple == "damage"){
                    try{
                        const Ast::Value * container = simple.getValue();
                        /*
                           container->reset();
                           Ast::Value * value;
                         *container >> value;
                         controller->getHit().damage.damage = (Ast::Value*) value->copy();
                         */

                        /* has guard */
                        if (container->hasMultiple()){
                            container->reset();
                            const Ast::Value * value;
                            *container >> value;
                            hit.damage.damage = Compiler::compile(value);
                            *container >> value;
                            hit.damage.guardDamage = Compiler::compile(value);
                        } else {
                            /* otherwise its a single expression */
                            hit.damage.damage = Compiler::compile(container);
                        }
                    } catch (const Ast::Exception & e){
                        ostringstream out;
                        out << "Could not read `damage' from '" << simple.toString() << "': " << e.getReason();
                        throw MugenException(out.str());
                    }
                } else if (simple == "pausetime"){
                    try{
                        simple >> hit.pause.player1;
                        simple >> hit.pause.player2;
                    } catch (const Ast::Exception & e){
                    }
                } else if (simple == "guard.pausetime"){
                    try{
                        simple >> hit.guardPause.player1;
                        simple >> hit.guardPause.player2;
                    } catch (const Ast::Exception & e){
                    }
                } else if (simple == "sparkno"){
                    string what;
                    simple >> what;
                    /* FIXME: either S123 or 123 */
                    if (PaintownUtil::matchRegex(what, "[0-9]+")){
                        hit.spark = atoi(what.c_str());
                    }
                } else if (simple == "guard.sparkno"){
                    string what;
                    simple >> what;
                    /* FIXME: either S123 or 123 */
                    if (PaintownUtil::matchRegex(what, "[0-9]+")){
                        hit.guardSpark = atoi(what.c_str());
                    }
                } else if (simple == "sparkxy"){
                    try{
                        simple >> hit.sparkPosition.x;
                        simple >> hit.sparkPosition.y;
                    } catch (const Ast::Exception & e){
                    }
                } else if (simple == "hitsound"){
                    string first;
                    bool own = false;
                    int group = 0;
                    int item = 0;
                    /* If not specified, assume item 0 */
                    simple >> first;
                    if (simple.getValue()->hasMultiple()){
                        simple >> item;
                    }
                    if (first[0] == 'S'){
                        own = true;
                        group = atoi(first.substr(1).c_str());
                    } else {
                        group = atoi(first.c_str());
                    }
                    hit.hitSound.own = own;
                    hit.hitSound.group = group;
                    hit.hitSound.item = item;
                } else if (simple == "guardsound"){
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
                    hit.guardHitSound.own = own;
                    hit.guardHitSound.group = group;
                    hit.guardHitSound.item = item;
                } else if (simple == "ground.type"){
                    string type;
                    simple >> type;
                    type = Util::fixCase(type);
                    if (type == "low"){
                        hit.groundType = AttackType::Low;
                    } else if (type == "high"){
                        hit.groundType = AttackType::High;
                    } else if (type == "trip"){
                        hit.groundType = AttackType::Trip;
                    }
                } else if (simple == "air.type"){
                    string type;
                    simple >> type;
                    type = Util::fixCase(type);
                    if (type == "low"){
                        hit.airType = AttackType::Low;
                    } else if (type == "high"){
                        hit.airType = AttackType::High;
                    } else if (type == "trip"){
                        hit.airType = AttackType::Trip;
                    }
                } else if (simple == "ground.slidetime"){
                    hit.groundSlideTime = Compiler::compile(simple.getValue());
                } else if (simple == "guard.slidetime"){
                    simple >> hit.guardSlideTime;
                } else if (simple == "ground.hittime"){
                    simple >> hit.groundHitTime;
                } else if (simple == "guard.hittime"){
                    simple >> hit.guardGroundHitTime;
                } else if (simple == "air.hittime"){
                    simple >> hit.airHitTime;
                } else if (simple == "guard.ctrltime"){
                    simple >> hit.guardControlTime;
                } else if (simple == "guard.dist"){
                    simple >> hit.guardDistance;
                } else if (simple == "yaccel"){
                    simple >> hit.yAcceleration;
                } else if (simple == "ground.velocity"){
                    if (simple.getValue()->hasMultiple()){
                        try{
                            simple >> hit.groundVelocity.x;
                            simple >> hit.groundVelocity.y;
                        } catch (const Ast::Exception & e){
                        }
                    } else {
                        simple >> hit.groundVelocity.x;
                    }
                } else if (simple == "guard.velocity"){
                    simple >> hit.guardVelocity;
                } else if (simple == "air.velocity"){
                    if (simple.getValue()->hasMultiple()){
                        try{
                            simple >> hit.airVelocity.x;
                            simple >> hit.airVelocity.y;
                        } catch (const Ast::Exception & e){
                        }
                    } else {
                        simple >> hit.airVelocity.x;
                    }
                } else if (simple == "airguard.velocity"){
                    try{
                        simple >> hit.airGuardVelocity.x;
                        simple >> hit.airGuardVelocity.y;
                    } catch (const Ast::Exception & e){
                    }
                } else if (simple == "ground.cornerpush.veloff"){
                    hit.groundCornerPushoff = Compiler::compile(simple.getValue());
                } else if (simple == "air.cornerpush.veloff"){
                    simple >> hit.airCornerPushoff;
                } else if (simple == "down.cornerpush.veloff"){
                    simple >> hit.downCornerPushoff;
                } else if (simple == "guard.cornerpush.veloff"){
                    simple >> hit.guardCornerPushoff;
                } else if (simple == "airguard.cornerpush.veloff"){
                    simple >> hit.airGuardCornerPushoff;
                } else if (simple == "airguard.ctrltime"){
                    simple >> hit.airGuardControlTime;
                } else if (simple == "air.juggle"){
                    simple >> hit.airJuggle;
                } else if (simple == "guardsound"){
                    try{
                        /* FIXME: parse a string and then determine if its S# or just # */
                        simple >> hit.guardHitSound.group;
                        simple >> hit.guardHitSound.item;
                    } catch (const Ast::Exception & e){
                    }
                } else if (simple == "mindist"){
                    simple >> hit.minimum.x;
                    simple >> hit.minimum.y;
                } else if (simple == "maxdist"){
                    simple >> hit.maximum.x;
                    simple >> hit.maximum.y;
                } else if (simple == "snap"){
                    simple >> hit.snap.x;
                    simple >> hit.snap.y;
                } else if (simple == "p1sprpriority"){
                    hit.player1SpritePriority = Compiler::compile(simple.getValue());
                } else if (simple == "p2sprpriority"){
                    simple >> hit.player2SpritePriority;
                } else if (simple == "p1facing"){
                    hit.player1Facing = Compiler::compile(simple.getValue());
                } else if (simple == "p2facing"){
                    hit.player2Facing = Compiler::compile(simple.getValue());
                } else if (simple == "p1getp2facing"){
                    hit.player1GetPlayer2Facing = Compiler::compile(simple.getValue());
                } else if (simple == "player2Facing"){
                    hit.player2Facing = Compiler::compile(simple.getValue());
                } else if (simple == "p1stateno"){
                    hit.player1State = Compiler::compile(simple.getValue());
                } else if (simple == "p2stateno"){
                    hit.player2State = Compiler::compile(simple.getValue());
                } else if (simple == "p2getp1state"){
                    hit.player2GetPlayer1State = Compiler::compile(simple.getValue());
                } else if (simple == "forcestand"){
                    simple >> hit.forceStand;
                } else if (simple == "fall"){
                    hit.fall.fall = Compiler::compile(simple.getValue());
                } else if (simple == "fall.xvelocity"){
                    simple >> hit.fall.xVelocity;
                } else if (simple == "fall.yvelocity"){
                    simple >> hit.fall.yVelocity;
                } else if (simple == "fall.recover"){
                    hit.fall.recover = Compiler::compile(simple.getValue());
                } else if (simple == "fall.recovertime"){
                    simple >> hit.fall.recoverTime;
                } else if (simple == "fall.damage"){
                    hit.fall.damage = Compiler::compile(simple.getValue());
                } else if (simple == "air.fall"){
                    simple >> hit.fall.airFall;
                } else if (simple == "forcenofall"){
                    simple >> hit.fall.forceNoFall;
                } else {
                    // Global::debug(0) << "Unhandled state controller '" << getName() << "' attribute: " << simple.toString() << endl;
                }
            }
        };

        Walker walker(hit);
        section->walk(walker);
    }

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
        /* prevent the same hitdef from being applied */
        if (guy.getHit() != &getHit()){
            guy.setHitDef(&getHit());
            guy.nextTicket();
        }
    }
};

class ControllerStateTypeSet: public StateController {
public:
    ControllerStateTypeSet(Ast::Section * section, const string & name):
        StateController(name, section),
        changeMoveType(false),
        changeStateType(false),
        changePhysics(false){
            parse(section);
        }

    bool changeMoveType;
    bool changeStateType;
    bool changePhysics;
    std::string moveType;
    std::string stateType;
    Physics::Type physics;

    void setMoveType(const string & type){
        changeMoveType = true;
        moveType = type;
    }

    void setStateType(const string & type){
        changeStateType = true;
        stateType = type;
    }

    void setPhysics(Physics::Type type){
        changePhysics = true;
        physics = type;
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerStateTypeSet & controller):
                controller(controller){
                }

            ControllerStateTypeSet & controller;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "movetype"){
                    string type;
                    simple >> type;
                    controller.setMoveType(type);
                } else if (simple == "physics"){
                    string type;
                    simple >> type;
                    if (type == "S"){
                        controller.setPhysics(Physics::Stand);
                    } else if (type == "N"){
                        controller.setPhysics(Physics::None);
                    } else if (type == "C"){
                        controller.setPhysics(Physics::Crouch);
                    } else if (type == "A"){
                        controller.setPhysics(Physics::Air);
                    }
                } else if (simple == "statetype"){
                    string type;
                    simple >> type;
                    controller.setStateType(type);
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
        if (changeMoveType){
            guy.setMoveType(moveType);
        }

        if (changeStateType){
            guy.setStateType(stateType);
        }

        if (changePhysics){
            guy.setCurrentPhysics(physics);
        }
    }
};

class ControllerSuperPause: public StateController {
public:
    ControllerSuperPause(Ast::Section * section, const string & name):
        StateController(name, section),
        time(NULL),
        posX(NULL),
        posY(NULL),
        animation(NULL),
        darken(NULL),
        p2defmul(NULL),
        poweradd(NULL),
        unhittable(NULL){
            parse(section);
        }

    struct Sound{
        Sound():
            group(-1),
        item(NULL),
        own(false){
        }

        ~Sound(){
            delete item;
        }

        int group;
        Compiler::Value * item;
        bool own;
    };

    Compiler::Value * time;
    Compiler::Value * posX;
    Compiler::Value * posY;
    Compiler::Value * animation;
    Compiler::Value * darken;
    Compiler::Value * p2defmul;
    Compiler::Value * poweradd;
    Compiler::Value * unhittable;
    Sound sound;

    virtual ~ControllerSuperPause(){
        delete time;
        delete posX;
        delete posY;
        delete animation;
        delete darken;
        delete p2defmul;
        delete poweradd;
        delete unhittable;
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerSuperPause & super):
                super(super){
                }

            ControllerSuperPause & super;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "time"){
                    super.time = Compiler::compile(simple.getValue());
                } else if (simple == "anim"){
                    super.animation = Compiler::compile(simple.getValue());
                } else if (simple == "darken"){
                    super.darken = Compiler::compile(simple.getValue());
                } else if (simple == "p2defmul"){
                    super.p2defmul = Compiler::compile(simple.getValue());
                } else if (simple == "poweradd"){
                    super.poweradd = Compiler::compile(simple.getValue());
                } else if (simple == "unhittable"){
                    super.unhittable = Compiler::compile(simple.getValue());
                } else if (simple == "sound"){
                    if (simple.getValue()->hasMultiple()){
                        string first;
                        bool own = false;
                        int group;
                        const Ast::Value * item;
                        simple >> first >> item;
                        if (first[0] == 'S'){
                            own = true;
                            group = atoi(first.substr(1).c_str());
                        } else {
                            group = atoi(first.c_str());
                        }
                        super.sound.own = own;
                        super.sound.group = group;
                        super.sound.item = Compiler::compile(item);
                    }
                } else if (simple == "pos"){
                    if (simple.getValue()->hasMultiple()){
                        const Ast::Value * posX;
                        const Ast::Value * posY;
                        simple >> posX >> posY;
                        super.posX = Compiler::compile(posX);
                        super.posY = Compiler::compile(posY);
                    }
                }
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    int computeX(Character & guy, const Environment & env) const {
        int pos = 0;
        if (posX != NULL){
            pos = (int) posX->evaluate(env).toNumber() * (guy.getFacing() == Object::FACING_LEFT ? -1 : 1);
        }
        return guy.getRX() + pos;
    }

    int computeY(Character & guy, const Environment & env) const {
        int pos = 0;
        if (posY != NULL){
            pos = (int) posY->evaluate(env).toNumber();
        }
        return guy.getRY() + pos;
    }

    void playSound(Character & guy, const Environment & environment) const {
        MugenSound * sound = NULL;
        if (this->sound.item != NULL){
            int itemNumber = (int) this->sound.item->evaluate(environment).toNumber();
            if (this->sound.own){
                sound = guy.getCommonSound(this->sound.group, itemNumber);
            } else {
                sound = guy.getSound(this->sound.group, itemNumber);
            }
        }

        if (sound != NULL){
            sound->play();
        }
    }

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment env(stage, guy);
        int x = computeX(guy, env);
        int y = computeY(guy, env);
        /* 30 is the default */
        int time = 30;
        if (this->time != NULL){
            time = (int) this->time->evaluate(env).toNumber();
        }
        int animation = 30;
        if (this->animation != NULL){
            animation = (int) this->animation->evaluate(env).toNumber();
        }

        /* FIXME: handle darken, p2defmul, unhittable */

        playSound(guy, env);
        stage.doSuperPause(time, animation, x, y);
    }
};

class ControllerAfterImage: public StateController {
public:
    ControllerAfterImage(Ast::Section * section, const string & name):
        StateController(name, section),
        time(NULL),
        length(NULL),
        timeGap(NULL),
        frameGap(NULL){
            parse(section);
        }

    Compiler::Value * time;
    Compiler::Value * length;
    Compiler::Value * timeGap;
    Compiler::Value * frameGap;

    virtual ~ControllerAfterImage(){
        delete time;
        delete length;
        delete timeGap;
        delete frameGap;
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(ControllerAfterImage & image):
                image(image){
                }

            ControllerAfterImage & image;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "time"){
                    image.time = Compiler::compile(simple.getValue());
                } else if (simple == "length"){
                    image.length = Compiler::compile(simple.getValue());
                } else if (simple == "palcolor"){
                } else if (simple == "palinvertall"){
                } else if (simple == "palbright"){
                } else if (simple == "palcontrast"){
                } else if (simple == "palpostbright"){
                } else if (simple == "paladd"){
                } else if (simple == "palmul"){
                } else if (simple == "timegap"){
                    image.timeGap = Compiler::compile(simple.getValue());
                } else if (simple == "framegap"){
                    image.frameGap = Compiler::compile(simple.getValue());
                } else if (simple == "trans"){
                }
                /* time = duration (int)
                 * Specifies the number of ticks that the afterimages should be displayed for. Set to -1 to display indefinitely. Defaults to 1.
                 * length = no_of_frames (int)
                 * Sets the capacity of the frame history buffer. The history will hold up to no_of_frames of the character's most recently saved frames. Assuming constant values for timegap and framegap, increasing the length can increase the number and "age" (for lack of a better term) of afterimages displayed at one time. The maximum length is 60, and the default is 20.
                 * palcolor = col (int)
                 * See below.
                 * palinvertall = invertall (bool)
                 * See below.
                 * palbright = add_r, add_g, add_b (int)
                 * See below.
                 * palcontrast = mul_r, mul_g, mul_b (int)
                 * See below.
                 * palpostbright = add2_r, add2_g, add2_b (int)
                 * These parameters determine palette effects to be applied to all afterimages. First the color level is adjusted according to the palcolor value, then if invertall is non-zero the colors are inverted. Afterwards, the palbright components are added to the corresponding component of the player's palette, then each component is multiplied by the corresponding palcontrast component divided by 256, then the palpostbright components are added to the result. The value of palcolor ranges from 0 (greyscale) to 256 (normal color). For instance, if the red component of the character's palette is denoted pal_r, then the red component of the afterimage palette is given by (pal_r + add_r) * mul_r / 256 + add2_r, assuming palcolor and palinvert are left at their default values. Valid values are 0-256 for palcolor, 0-255 for palbright and palpostbright components, and any non-negative integer for palcontrast components. The defaults are:
                 *
                 * palcolor = 256
                 * palinvertall = 0
                 * palbright = 30,30,30
                 * palcontrast = 120,120,220
                 * palpostbright = 0,0,0
                 * paladd = add_r, add_g, add_b (int)
                 * See below.
                 * palmul = mul_r, mul_g, mul_b (float)
                 * These parameters specify palette effects that are applied repeatedly to successive frames in the afterimage. In one application of these palette effects, first the paladd components are added to the afterimage palette, then the components are multiplied by the palmul multipliers. These effects are applied zero times to the most recent afterimage frame, once to the second-newest afterimage frame, twice in succession to the third-newest afterimage frame, etc. Valid values are 0-255 for the paladd components, and any non-negative float value for the palmul multipliers. The defaults are:
                 *
                 * paladd = 10,10,25
                 * palmul = .65,.65,.75
                 * timegap = value (int)
                 * This parameter controls how many frames to skip between saving player frames to the history buffer for afterimage display. The default is 1 (skip no frames). To save every third frame (for example), you would use timegap = 3.
                 * framegap = value (int)
                 * Every value'th frame in the history buffer will be displayed as an afterimage. For instance, if framegap = 4 (the default), then the first, fifth, ninth, ... frames of the history buffer will be displayed as afterimages.
                 * trans = type (string)
                 * Specifies the transparency type for the afterimages. Valid values for type are "none" for an opaque afterimage, "add", "add1", and "sub". Defaults to "none".
                 */
            }
        };

        Walker walker(*this);
        section->walk(walker);
    }

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment environment(stage, guy, commands);
        int timegap = 1;
        if (this->timeGap != NULL){
            timegap = this->timeGap->evaluate(environment).toNumber();
        }

        int framegap = 1;
        if (this->frameGap != NULL){
            framegap = this->frameGap->evaluate(environment).toNumber();
        }

        int time = 1;
        if (this->time != NULL){
            time = this->time->evaluate(environment).toNumber();
        }

        int length = 20;
        if (this->length != NULL){
            length = this->length->evaluate(environment).toNumber();
        }

        /* FIXME: handle palette and translucency */
        guy.setAfterImage(time, length, timegap, framegap);
    }
};

class ControllerAfterImageTime: public StateController {
public:
    ControllerAfterImageTime(Ast::Section * section, const string & name):
        StateController(name, section),
        time(NULL){
            parse(section);
        }

    Compiler::Value * time;

    virtual ~ControllerAfterImageTime(){
        delete time;
    }

    void parse(Ast::Section * section){
        class Walker: public Ast::Walker {
        public:
            Walker(Compiler::Value *& time):
                time(time){
                }

            Compiler::Value *& time;

            virtual void onAttributeSimple(const Ast::AttributeSimple & simple){
                if (simple == "time"){
                    time = Compiler::compile(simple.getValue());
                } else if (simple == "value"){
                    time = Compiler::compile(simple.getValue());
                }
            }
        };

        Walker walker(time);
        section->walk(walker);
        if (time == NULL){
            throw MugenException("Time not specified for AfterImageTime");
        }
    }

    virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
        FullEnvironment environment(stage, guy);
        int time = this->time->evaluate(environment).toNumber();
        guy.setAfterImageTime(time);
    }
};

StateController * StateController::compile(Ast::Section * section, const string & name, int state, StateController::Type type){
    switch (type){
        case StateController::ChangeAnim : return new ControllerChangeAnim(section, name);
        case StateController::ChangeState : return new ControllerChangeState(section, name);
        case StateController::CtrlSet : return new ControllerCtrlSet(section, name);
        case StateController::PlaySnd : return new ControllerPlaySound(section, name);
        case StateController::VarSet : return new ControllerVarSet(section, name);
        case StateController::VelSet : return new ControllerVelSet(section, name);
        case StateController::HitVelSet : return new ControllerHitVelSet(section, name);
        case StateController::PosAdd : return new ControllerPosAdd(section, name);
        case StateController::PosSet : return new ControllerPosSet(section, name);
        case StateController::VelAdd : return new ControllerVelAdd(section, name);
        case StateController::VelMul : return new ControllerVelMul(section, name);
        case StateController::HitDef : return new ControllerHitDef(section, name);
        case StateController::StateTypeSet : return new ControllerStateTypeSet(section, name);
        case StateController::SuperPause : return new ControllerSuperPause(section, name);
        case StateController::AfterImage : return new ControllerAfterImage(section, name);
        case StateController::AfterImageTime : return new ControllerAfterImageTime(section, name);
        case StateController::AllPalFX :
        case StateController::AngleAdd :
        case StateController::AngleDraw :
        case StateController::AngleMul :
        case StateController::AngleSet :
        case StateController::AppendToClipboard :
        case StateController::AssertSpecial :
        case StateController::AttackDist :
        case StateController::AttackMulSet :
        case StateController::BGPalFX :
        case StateController::BindToParent :
        case StateController::BindToRoot :
        case StateController::BindToTarget :
        case StateController::ChangeAnim2 :
        case StateController::ClearClipboard :
        case StateController::DefenceMulSet :
        case StateController::DestroySelf :
        case StateController::DisplayToClipboard :
        case StateController::EnvColor :
        case StateController::EnvShake :
        case StateController::Explod :
        case StateController::ExplodBindTime :
        case StateController::ForceFeedback :
        case StateController::FallEnvShake :
        case StateController::GameMakeAnim :
        case StateController::Gravity :
        case StateController::Helper :
        case StateController::HitAdd :
        case StateController::HitBy :
        case StateController::HitFallDamage :
        case StateController::HitFallSet :
        case StateController::HitFallVel :
        case StateController::HitOverride :
        case StateController::LifeAdd :
        case StateController::LifeSet :
        case StateController::MakeDust :
        case StateController::ModifyExplod :
        case StateController::MoveHitReset :
        case StateController::NotHitBy :
        case StateController::Null :
        case StateController::Offset :
        case StateController::PalFX :
        case StateController::ParentVarAdd :
        case StateController::ParentVarSet :
        case StateController::Pause :
        case StateController::PlayerPush :
        case StateController::PosFreeze :
        case StateController::PowerAdd :
        case StateController::PowerSet :
        case StateController::Projectile :
        case StateController::RemoveExplod :
        case StateController::ReversalDef :
        case StateController::ScreenBound :
        case StateController::SelfState :
        case StateController::SprPriority :
        case StateController::SndPan :
        case StateController::StopSnd :
        case StateController::TargetBind :
        case StateController::TargetDrop :
        case StateController::TargetFacing :
        case StateController::TargetLifeAdd :
        case StateController::TargetPowerAdd :
        case StateController::TargetState :
        case StateController::TargetVelAdd :
        case StateController::TargetVelSet :
        case StateController::Trans :
        case StateController::Turn :
        case StateController::VarAdd :
        case StateController::VarRandom :
        case StateController::VarRangeSet :
        case StateController::Width : {
            class DefaultController: public StateController {
            public:
                DefaultController(Ast::Section * section, const string & name):
                    StateController(name, section){
                    }

                virtual void activate(MugenStage & stage, Character & guy, const vector<string> & commands) const {
                    /* nothing */
                }
            };

            return new DefaultController(section, name);
        }
        case StateController::Unknown : {
            ostringstream out;
            out << "Unknown state controller for " << state << " " << name;
            throw MugenException(out.str());
            break;
        }
            /*
               case InternalCommand : {
               typedef void (Character::*func)(const MugenStage & stage, const vector<string> & inputs);
               func f = (func) controller.internal;
               (guy.*f)(stage, commands);
               break;
               }
               */
    }

    ostringstream out;
    out << "Unknown state controller for " << state << " " << name << " type (" << type << ")";
    throw MugenException(out.str());
}

}
